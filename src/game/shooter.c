#include "input.h"
#include "video.h"

#include "../utils/definitions.h"
#include "../utils/utilities.h"
#include "shooter.h"

#define MAX_BULLETS  12
#define MAX_ENEMIES  5
#define PLAYER_SPRITE_SIZE  128
#define BANKED_SPRITE_SIZE  160

struct shooter_bullet
{
    int x;
    int y;
    int dx;
    int dy;
    int active;
};

struct shooter_enemy
{
    int x;
    int y;
    int active;
};

struct shooter_game
{
    int player_x;
    int player_y;
    int player_bank;
    int move_cooldown;
    int fire_cooldown;
    int enemy_cooldown;
    int invulnerability;
    int frame;
    int score;
    int health;
    int wave;
    int wave_total;
    int wave_kills;
    int pending_enemies;
    int game_over;
    shooter_bullet[ MAX_BULLETS ] bullets;
    shooter_enemy[ MAX_ENEMIES ] enemies;
};

shooter_game game;

void shooter_spawn_enemy( int slot )
{
    game.enemies[ slot ].x = ARENA_MIN_X + 1 +
        ((slot * 11 + game.wave * 7) % (ARENA_MAX_X - ARENA_MIN_X - 1));
    game.enemies[ slot ].y = ARENA_MIN_Y;
    game.enemies[ slot ].active = true;
}

void shooter_fill_enemy_slots()
{
    for( int slot = 0; slot < MAX_ENEMIES && game.pending_enemies > 0; ++slot )
    {
        if( !game.enemies[ slot ].active )
        {
            shooter_spawn_enemy( slot );
            --game.pending_enemies;
        }
    }
}

void shooter_begin_wave()
{
    ++game.wave;
    game.wave_total = 4 + game.wave;
    game.wave_kills = 0;
    game.pending_enemies = game.wave_total;

    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
      game.enemies[ slot ].active = false;

    shooter_fill_enemy_slots();
}

void shooter_start()
{
    game.player_x = 32;
    game.player_y = ARENA_MAX_Y - 1;
    game.player_bank = 0;
    game.move_cooldown = 0;
    game.fire_cooldown = 0;
    game.enemy_cooldown = 0;
    game.invulnerability = 0;
    game.frame = 0;
    game.score = 0;
    game.health = 3;
    game.wave = 0;
    game.game_over = false;

    for( int slot = 0; slot < MAX_BULLETS; ++slot )
      game.bullets[ slot ].active = false;

    shooter_begin_wave();
}

void shooter_move_player()
{
    int delta_x = 0;

    if( gamepad_left() > 0 )
      delta_x = -1;
    else if( gamepad_right() > 0 )
      delta_x = 1;

    if( delta_x == 0 )
    {
        game.player_bank = 0;
      return;
    }

    int next_x = game.player_x + delta_x;

    if( next_x >= ARENA_MIN_X + 3 && next_x <= ARENA_MAX_X - 3 )
    {
      game.player_x = next_x;
      game.player_bank = delta_x;
    }
    else
      game.player_bank = 0;

    game.move_cooldown = 4;
}

void shooter_fire()
{
    for( int slot = 0; slot < MAX_BULLETS; ++slot )
    {
        if( !game.bullets[ slot ].active )
        {
            game.bullets[ slot ].x = game.player_x;
            game.bullets[ slot ].y = game.player_y;
            game.bullets[ slot ].dx = 0;
            game.bullets[ slot ].dy = -1;
            game.bullets[ slot ].active = true;
            game.fire_cooldown = 8;
            return;
        }
    }
}

void shooter_update_bullets()
{
    for( int bullet_slot = 0; bullet_slot < MAX_BULLETS; ++bullet_slot )
    {
        shooter_bullet* bullet = &game.bullets[ bullet_slot ];
        if( !bullet->active )
          continue;

        bullet->x += bullet->dx;
        bullet->y += bullet->dy;

        if( bullet->x < ARENA_MIN_X + 1 || bullet->x > ARENA_MAX_X - 1 ||
            bullet->y < ARENA_MIN_Y || bullet->y > ARENA_MAX_Y )
        {
            bullet->active = false;
            continue;
        }

        for( int enemy_slot = 0; enemy_slot < MAX_ENEMIES; ++enemy_slot )
        {
            shooter_enemy* enemy = &game.enemies[ enemy_slot ];
            if( enemy->active && bullet->x == enemy->x && bullet->y == enemy->y )
            {
                enemy->active = false;
                bullet->active = false;
                game.score += 100;
                ++game.wave_kills;
                shooter_fill_enemy_slots();
                break;
            }
        }
    }

    if( game.wave_kills >= game.wave_total )
      shooter_begin_wave();
}

void shooter_move_enemies()
{
    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
    {
        shooter_enemy* enemy = &game.enemies[ slot ];
        if( !enemy->active )
          continue;

        ++enemy->y;

        if( enemy->y >= game.player_y - 2 && enemy->y <= game.player_y + 1 &&
            enemy->x > game.player_x - 2 && enemy->x < game.player_x + 2 )
        {
            if( game.invulnerability == 0 )
            {
                --game.health;
                game.invulnerability = 45;

                if( game.health <= 0 )
                  game.game_over = true;
            }

            enemy->active = false;
            ++game.wave_kills;
            shooter_fill_enemy_slots();
        }
        else if( enemy->y > ARENA_MAX_Y )
        {
            enemy->active = false;
            ++game.wave_kills;
            shooter_fill_enemy_slots();
        }
    }

    if( game.wave_kills >= game.wave_total )
      shooter_begin_wave();
}

void shooter_update()
{
    if( game.move_cooldown > 0 )
      --game.move_cooldown;
    if( game.fire_cooldown > 0 )
      --game.fire_cooldown;
    if( game.enemy_cooldown > 0 )
      --game.enemy_cooldown;
    if( game.invulnerability > 0 )
      --game.invulnerability;

    if( game.move_cooldown == 0 )
      shooter_move_player();

    if( gamepad_button_b() > 0 && game.fire_cooldown == 0 )
      shooter_fire();

    if( game.frame % 3 == 0 )
      shooter_update_bullets();

    if( game.enemy_cooldown == 0 )
    {
        shooter_move_enemies();
        game.enemy_cooldown = 8;
    }

    ++game.frame;
}

void shooter_draw()
{
    draw_arena_frame( (game.frame / 4) % 40 );

    set_multiply_color( color_white );
    print_at( 12, 8, "NEON DUSK" );
    print_at( 170, 8, "WAVE" );
    draw_number( 220, 8, game.wave, 2 );
    print_at( 280, 8, "SCORE" );
    draw_number( 340, 8, game.score, 5 );
    print_at( 485, 8, "HULL" );
    draw_number( 535, 8, game.health, 1 );

    set_multiply_color( make_color_rgb( 255, 75, 85 ) );
    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
    {
        if( game.enemies[ slot ].active )
          print_at( game.enemies[ slot ].x * 10, game.enemies[ slot ].y * 20, "o" );
    }

    set_multiply_color( make_color_rgb( 255, 220, 90 ) );
    for( int slot = 0; slot < MAX_BULLETS; ++slot )
    {
        if( game.bullets[ slot ].active )
          print_at( game.bullets[ slot ].x * 10, game.bullets[ slot ].y * 20, "*" );
    }

    if( game.invulnerability == 0 || game.frame % 6 < 3 )
    {
        int texture_id = 0;
        int sprite_size = PLAYER_SPRITE_SIZE;
        float sprite_scale = 0.4375;

        if( game.player_bank < 0 )
        {
            texture_id = 1;
            sprite_size = BANKED_SPRITE_SIZE;
            sprite_scale = 0.4375;
        }
        else if( game.player_bank > 0 )
        {
            texture_id = 2;
            sprite_size = BANKED_SPRITE_SIZE;
            sprite_scale = 0.4375;
        }

        select_texture( texture_id );
        select_region( 0 );
        define_region_center( 0, 0, sprite_size, sprite_size );
        set_drawing_scale( sprite_scale, sprite_scale );
        set_multiply_color( color_white );
        draw_region_zoomed_at( game.player_x * 10 + 5, game.player_y * 20 + 10 );
        set_drawing_scale( 1, 1 );
    }
}

int shooter_is_game_over()
{
    return game.game_over;
}

int shooter_get_score()
{
    return game.score;
}

int shooter_get_wave()
{
    return game.wave;
}
