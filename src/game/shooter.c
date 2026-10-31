#include "input.h"
#include "video.h"

#include "../utils/definitions.h"
#include "../utils/utilities.h"
#include "shooter.h"

#define MAX_BULLETS  12
#define MAX_ENEMIES  5

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
    int facing_x;
    int facing_y;
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
    int edge = (game.wave + slot) % 4;
    int offset = (slot * 3 + game.wave * 2) % 13;

    if( edge == 0 )
    {
        game.enemies[ slot ].x = ARENA_MIN_X + 1;
        game.enemies[ slot ].y = ARENA_MIN_Y + offset;
    }
    else if( edge == 1 )
    {
        game.enemies[ slot ].x = ARENA_MAX_X - 1;
        game.enemies[ slot ].y = ARENA_MIN_Y + offset;
    }
    else if( edge == 2 )
    {
        game.enemies[ slot ].x = ARENA_MIN_X + 1 + offset * 4;
        game.enemies[ slot ].y = ARENA_MIN_Y;
    }
    else
    {
        game.enemies[ slot ].x = ARENA_MIN_X + 1 + offset * 4;
        game.enemies[ slot ].y = ARENA_MAX_Y;
    }

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
    game.player_y = 9;
    game.facing_x = 1;
    game.facing_y = 0;
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
    int delta_y = 0;

    if( gamepad_left() > 0 )
      delta_x = -1;
    else if( gamepad_right() > 0 )
      delta_x = 1;

    if( gamepad_up() > 0 )
      delta_y = -1;
    else if( gamepad_down() > 0 )
      delta_y = 1;

    if( delta_x == 0 && delta_y == 0 )
      return;

    game.facing_x = delta_x;
    game.facing_y = delta_y;

    int next_x = game.player_x + delta_x;
    int next_y = game.player_y + delta_y;

    if( next_x >= ARENA_MIN_X + 1 && next_x <= ARENA_MAX_X - 1 )
      game.player_x = next_x;

    if( next_y >= ARENA_MIN_Y && next_y <= ARENA_MAX_Y )
      game.player_y = next_y;

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
            game.bullets[ slot ].dx = game.facing_x;
            game.bullets[ slot ].dy = game.facing_y;
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

        int distance_x = game.player_x - enemy->x;
        int distance_y = game.player_y - enemy->y;

        if( distance_x < 0 )
          distance_x = -distance_x;
        if( distance_y < 0 )
          distance_y = -distance_y;

        if( distance_x >= distance_y && enemy->x != game.player_x )
        {
            if( game.player_x > enemy->x )
              ++enemy->x;
            else
              --enemy->x;
        }
        else if( enemy->y != game.player_y )
        {
            if( game.player_y > enemy->y )
              ++enemy->y;
            else
              --enemy->y;
        }

        if( enemy->x == game.player_x && enemy->y == game.player_y &&
            game.invulnerability == 0 )
        {
            --game.health;
            game.invulnerability = 45;

            if( game.health <= 0 )
              game.game_over = true;
        }
    }
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
    draw_arena_frame();

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
        int* player_glyph = ">";

        if( game.facing_x < 0 )
          player_glyph = "<";
        else if( game.facing_y < 0 )
          player_glyph = "^";
        else if( game.facing_y > 0 )
          player_glyph = "v";

        set_multiply_color( make_color_rgb( 60, 220, 255 ) );
        print_at( game.player_x * 10, game.player_y * 20, player_glyph );
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
