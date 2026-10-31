#include "input.h"
#include "video.h"

#include "../utils/definitions.h"
#include "../utils/utilities.h"
#include "shooter.h"

#define MAX_BULLETS  12
#define MAX_ENEMIES  5
#define MAX_EXPLOSIONS  12
#define MAX_HEALTH  20
#define PLAYER_MOVE_INTERVAL  2
#define PLAYER_SPRITE_SIZE  128
#define BANKED_SPRITE_SIZE  160
#define ENEMY_SPRITE_SIZE  128
#define ENEMY_DRAW_SIZE_PIXELS  38
#define ENEMY_SPAWN_Y  -2
#define LASER_SPRITE_SIZE  32
#define EXPLOSION_FRAME_SIZE  32
#define EXPLOSION_FRAME_COUNT  4
#define EXPLOSION_FRAME_DURATION  3

struct shooter_explosion
{
    int x;
    int y;
    int frame;
    int active;
};

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
    shooter_explosion[ MAX_EXPLOSIONS ] explosions;
};

shooter_game game;

void shooter_spawn_explosion( int x, int y )
{
    for( int slot = 0; slot < MAX_EXPLOSIONS; ++slot )
    {
        shooter_explosion* explosion = &game.explosions[ slot ];
        if( !explosion->active )
        {
            explosion->x = x;
            explosion->y = y;
            explosion->frame = 0;
            explosion->active = true;
            return;
        }
    }
}

void shooter_update_explosions()
{
    for( int slot = 0; slot < MAX_EXPLOSIONS; ++slot )
    {
        shooter_explosion* explosion = &game.explosions[ slot ];
        if( !explosion->active )
          continue;

        ++explosion->frame;
        if( explosion->frame >=
            EXPLOSION_FRAME_COUNT * EXPLOSION_FRAME_DURATION )
          explosion->active = false;
    }
}

void shooter_spawn_enemy( int slot )
{
    game.enemies[ slot ].x = ARENA_MIN_X + 1 +
        ((slot * 11 + game.wave * 7) % (ARENA_MAX_X - ARENA_MIN_X - 1));
    game.enemies[ slot ].y = ENEMY_SPAWN_Y;
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
    game.player_y = (ARENA_MAX_Y - 1) * 2;
    game.player_bank = 0;
    game.move_cooldown = 0;
    game.fire_cooldown = 0;
    game.enemy_cooldown = 0;
    game.invulnerability = 0;
    game.frame = 0;
    game.score = 0;
    game.health = MAX_HEALTH;
    game.wave = 0;
    game.game_over = false;

    for( int slot = 0; slot < MAX_BULLETS; ++slot )
      game.bullets[ slot ].active = false;
    for( int slot = 0; slot < MAX_EXPLOSIONS; ++slot )
      game.explosions[ slot ].active = false;

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
    {
        game.player_bank = 0;
        return;
    }

    int next_x = game.player_x + delta_x;
    int next_y = game.player_y + delta_y;

    if( delta_x != 0 &&
        next_x >= PLAYER_MIN_X && next_x <= PLAYER_MAX_X )
    {
        game.player_x = next_x;
        game.player_bank = delta_x;
    }
    else if( delta_x != 0 )
        game.player_bank = 0;

    if( delta_y != 0 &&
        next_y >= PLAYER_MIN_Y && next_y <= PLAYER_MAX_Y )
      game.player_y = next_y;

    game.move_cooldown = PLAYER_MOVE_INTERVAL;
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
            game.bullets[ slot ].dy = -2;
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
            bullet->y < PLAYER_MIN_Y || bullet->y > PLAYER_MAX_Y )
        {
            bullet->active = false;
            continue;
        }

        for( int enemy_slot = 0; enemy_slot < MAX_ENEMIES; ++enemy_slot )
        {
            shooter_enemy* enemy = &game.enemies[ enemy_slot ];
            int enemy_y = enemy->y * 2;
            if( enemy->active && bullet->x == enemy->x &&
                bullet->y >= enemy_y - 1 && bullet->y <= enemy_y + 1 )
            {
                shooter_spawn_explosion( enemy->x * 10 + 5, enemy_y * 10 + 10 );
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

        int enemy_y = enemy->y * 2;
        if( enemy_y >= game.player_y - 4 && enemy_y <= game.player_y + 2 &&
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
        else if( enemy->y * 20 + 10 - ENEMY_DRAW_SIZE_PIXELS / 2 >=
                 SCREEN_HEIGHT )
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

    shooter_update_explosions();

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
    int background_scroll = (game.frame / 8) % SCREEN_HEIGHT;

    select_texture( 5 );
    select_region( 0 );
    define_region_center( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    draw_region_zoomed_at( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + background_scroll );
    draw_region_zoomed_at(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2 + background_scroll - SCREEN_HEIGHT
    );

    select_texture( 7 );
    select_region( 0 );
    define_region_center( 0, 0, 16, SCREEN_HEIGHT );
    set_multiply_color( color_white );
    draw_region_zoomed_at( 8, SCREEN_HEIGHT / 2 );
    draw_region_zoomed_at( SCREEN_WIDTH - 8, SCREEN_HEIGHT / 2 );

    select_texture( 6 );
    select_region( 0 );
    define_region_center( 0, 0, 16, 24 );
    set_multiply_color( color_white );
    set_drawing_scale( 1, 0.5 );
    for( int segment = 0; segment < game.health; ++segment )
      draw_region_zoomed_at(
          SCREEN_WIDTH - 8,
          10 + (MAX_HEALTH - game.health + segment) * 17
      );
    set_drawing_scale( 1, 1 );

    select_texture( 3 );
    select_region( 0 );
    define_region_center( 0, 0, ENEMY_SPRITE_SIZE, ENEMY_SPRITE_SIZE );
    set_drawing_scale( 0.3, 0.3 );
    set_multiply_color( color_white );

    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
    {
        if( game.enemies[ slot ].active )
          draw_region_zoomed_at(
              game.enemies[ slot ].x * 10 + 5,
              game.enemies[ slot ].y * 20 + 10
          );
    }
    set_drawing_scale( 1, 1 );

    select_texture( 10 );
    select_region( 0 );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    for( int slot = 0; slot < MAX_EXPLOSIONS; ++slot )
    {
        shooter_explosion* explosion = &game.explosions[ slot ];
        if( explosion->active )
        {
            int frame = explosion->frame / EXPLOSION_FRAME_DURATION;
            int frame_x = frame * EXPLOSION_FRAME_SIZE;
            define_region_center(
                frame_x,
                0,
                frame_x + EXPLOSION_FRAME_SIZE,
                EXPLOSION_FRAME_SIZE
            );
            draw_region_zoomed_at( explosion->x, explosion->y );
        }
    }

    select_texture( 8 );
    select_region( 0 );
    define_region_center( 0, 0, LASER_SPRITE_SIZE, LASER_SPRITE_SIZE * 2 );
    set_drawing_scale( 0.5, 0.5 );
    set_multiply_color( color_white );
    for( int slot = 0; slot < MAX_BULLETS; ++slot )
    {
        if( game.bullets[ slot ].active )
          draw_region_zoomed_at(
              game.bullets[ slot ].x * 10 + 5,
              game.bullets[ slot ].y * PLAYER_Y_STEP_PIXELS + 5
          );
    }
    set_drawing_scale( 1, 1 );

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
        draw_region_zoomed_at(
            game.player_x * 10 + 5,
            game.player_y * PLAYER_Y_STEP_PIXELS + 10
        );
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
