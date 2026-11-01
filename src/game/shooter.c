#include "input.h"
#include "video.h"

#include "../utils/definitions.h"
#include "../utils/utilities.h"
#include "shooter.h"

#define MAX_BULLETS  24
#define MAX_ENEMIES  5
#define ENEMY_TYPE_COUNT  2
#define MAX_EXPLOSIONS  12
#define MAX_COINS  8
#define MAX_DEBRIS  240
#define DEBRIS_PIECE_COUNT  10
#define DEBRIS_CELL_SIZE  48
#define DEBRIS_ATLAS_COLUMNS  5
#define DEBRIS_BURST_DURATION  24
#define DEBRIS_RENDER_HALF_SIZE  5
#define MAX_HEALTH  20
#define COIN_DROP_CHANCE  4
#define COIN_SPRITE_SIZE  16
#define COIN_SPIN_FRAME_COUNT  8
#define COIN_SPIN_FRAME_DURATION  4
#define COIN_HITBOX_HALF_SIZE  8
#define BACKGROUND_SCROLL_INTERVAL  4
#define BACKGROUND_SCENE_HEIGHT  SCREEN_HEIGHT
#define BACKGROUND_SCENE_COUNT  5
#define BACKGROUND_WORLD_HEIGHT  (BACKGROUND_SCENE_COUNT * BACKGROUND_SCENE_HEIGHT)
#define JUNGLE_BACKGROUND_TEXTURE_1  5
#define JUNGLE_BACKGROUND_TEXTURE_2  13
#define JUNGLE_BACKGROUND_TEXTURE_3  14
#define JUNGLE_BACKGROUND_TEXTURE_4  15
#define JUNGLE_BACKGROUND_TEXTURE_5  16
#define PLAYER_HORIZONTAL_MOVE_INTERVAL  2
#define PLAYER_VERTICAL_MOVE_INTERVAL  1
#define ENEMY_MOVE_INTERVAL  2
#define PLAYER_BANK_FRAME_WIDTH  176
#define PLAYER_BANK_FRAME_HEIGHT  240
#define PLAYER_UP_TEXTURE  18
#define PLAYER_DOWN_TEXTURE  19
#define PLAYER_DIRECTION_UP  0
#define PLAYER_DIRECTION_DOWN  1
#define ENEMY_Y_STEP_PIXELS  10
#define PLAYER_BULLET_Y_OFFSET_PIXELS  10
#define ENEMY_DRAW_SIZE_PIXELS  38
#define ENEMY_HITBOX_HALF_SIZE  16
#define ENEMY_SPAWN_Y  -4
#define ENEMY_HIT_FLASH_DURATION  6
#define LASER_TEXTURE_LEFT  6
#define LASER_TEXTURE_TOP  1
#define LASER_SPRITE_WIDTH  21
#define LASER_SPRITE_HEIGHT  46
#define LASER_HITBOX_HALF_WIDTH  3
#define LASER_HITBOX_HALF_HEIGHT  6
#define EXPLOSION_FRAME_SIZE  64
#define EXPLOSION_FRAME_COUNT  5
#define EXPLOSION_FRAME_DURATION  3
#define PLAYER_HITBOX_HALF_WIDTH  14
#define PLAYER_HITBOX_HALF_HEIGHT  20

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
    int type;
    int hit_points;
    int move_cooldown;
    int hit_flash;
    int active;
};

struct shooter_enemy_type
{
    int texture_id;
    int sprite_width;
    int sprite_height;
    int hit_points;
    int move_interval;
};

struct shooter_coin
{
    int x;
    int y_pixels;
    int active;
};

struct shooter_debris
{
    int x_subpixels;
    int y_subpixels;
    int velocity_x;
    int velocity_y;
    int age;
    int piece;
    int grounded;
    int active;
};

struct shooter_game
{
    int player_x;
    int player_y;
    int player_bank;
    int player_direction;
    int horizontal_move_cooldown;
    int vertical_move_cooldown;
    int fire_cooldown;
    int enemy_cooldown;
    int invulnerability;
    int frame;
    int score;
    int coins_collected;
    int random_seed;
    int health;
    int wave;
    int wave_total;
    int wave_kills;
    int pending_enemies;
    int next_enemy_type;
    int game_over;
    shooter_bullet[ MAX_BULLETS ] bullets;
    shooter_enemy[ MAX_ENEMIES ] enemies;
    shooter_explosion[ MAX_EXPLOSIONS ] explosions;
    shooter_coin[ MAX_COINS ] coins;
    shooter_debris[ MAX_DEBRIS ] debris;
};

shooter_game game;
shooter_enemy_type[ ENEMY_TYPE_COUNT ] shooter_enemy_types =
{
    { 3, 113, 150, 1, 1 },
    { 17, 148, 132, 5, 2 }
};

int shooter_random( int limit )
{
    game.random_seed =
        (game.random_seed * 25173 + 13849) % 65521;
    return game.random_seed % limit;
}

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

void shooter_spawn_debris( int x, int y )
{
    int direction_offset = shooter_random( 8 );

    for( int piece = 0; piece < DEBRIS_PIECE_COUNT; ++piece )
    {
        int debris_slot = -1;

        for( int slot = 0; slot < MAX_DEBRIS; ++slot )
        {
            shooter_debris* debris = &game.debris[ slot ];
            if( !debris->active )
            {
                debris_slot = slot;
                break;
            }
        }

        if( debris_slot < 0 )
          break;

        int direction = (piece + direction_offset) % 8;
        int base_velocity_x = 0;
        int base_velocity_y = 0;
        if( direction == 0 )
        {
            base_velocity_x = -24;
            base_velocity_y = -24;
        }
        else if( direction == 1 )
          base_velocity_y = -32;
        else if( direction == 2 )
        {
            base_velocity_x = 24;
            base_velocity_y = -24;
        }
        else if( direction == 3 )
          base_velocity_x = 32;
        else if( direction == 4 )
        {
            base_velocity_x = 24;
            base_velocity_y = 24;
        }
        else if( direction == 5 )
          base_velocity_y = 32;
        else if( direction == 6 )
        {
            base_velocity_x = -24;
            base_velocity_y = 24;
        }
        else
          base_velocity_x = -32;

        int speed = 12 + shooter_random( 9 );
        int column = piece % DEBRIS_ATLAS_COLUMNS;
        int row = piece / DEBRIS_ATLAS_COLUMNS;
        shooter_debris* debris = &game.debris[ debris_slot ];
        debris->x_subpixels = x * 16 +
            (column * DEBRIS_CELL_SIZE + DEBRIS_CELL_SIZE / 2 -
             DEBRIS_ATLAS_COLUMNS * DEBRIS_CELL_SIZE / 2) * 48 / 10;
        debris->y_subpixels = y * 16 +
            (row * DEBRIS_CELL_SIZE + DEBRIS_CELL_SIZE / 2 -
             2 * DEBRIS_CELL_SIZE / 2) *
              48 / 10;
        debris->velocity_x = base_velocity_x * speed / 16;
        debris->velocity_y = base_velocity_y * speed / 16;
        debris->age = 0;
        debris->piece = piece;
        debris->grounded = false;
        debris->active = true;
    }
}

void shooter_maybe_drop_coin( int x, int y )
{
    if( shooter_random( COIN_DROP_CHANCE ) != 0 )
      return;

    for( int slot = 0; slot < MAX_COINS; ++slot )
    {
        shooter_coin* coin = &game.coins[ slot ];
        if( !coin->active )
        {
            coin->x = x;
            coin->y_pixels = y * ENEMY_Y_STEP_PIXELS + 10;
            coin->active = true;
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
    shooter_enemy* enemy = &game.enemies[ slot ];
    enemy->x = ARENA_MIN_X + 1 +
        ((slot * 11 + game.wave * 7) % (ARENA_MAX_X - ARENA_MIN_X - 1));
    enemy->y = ENEMY_SPAWN_Y;
    enemy->type = game.next_enemy_type;
    enemy->hit_points = shooter_enemy_types[ enemy->type ].hit_points;
    enemy->move_cooldown = 0;
    enemy->hit_flash = 0;
    enemy->active = true;
    ++game.next_enemy_type;
    if( game.next_enemy_type >= ENEMY_TYPE_COUNT )
      game.next_enemy_type = 0;
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
    game.player_y = PLAYER_START_Y;
    game.player_bank = 0;
    game.player_direction = PLAYER_DIRECTION_UP;
    game.horizontal_move_cooldown = 0;
    game.vertical_move_cooldown = 0;
    game.fire_cooldown = 0;
    game.enemy_cooldown = 0;
    game.invulnerability = 0;
    game.frame = 0;
    game.score = 0;
    game.coins_collected = 0;
    game.random_seed = 1;
    game.health = MAX_HEALTH;
    game.wave = 0;
    game.next_enemy_type = 0;
    game.game_over = false;

    for( int slot = 0; slot < MAX_BULLETS; ++slot )
      game.bullets[ slot ].active = false;
    for( int slot = 0; slot < MAX_EXPLOSIONS; ++slot )
      game.explosions[ slot ].active = false;
    for( int slot = 0; slot < MAX_COINS; ++slot )
      game.coins[ slot ].active = false;
    for( int slot = 0; slot < MAX_DEBRIS; ++slot )
      game.debris[ slot ].active = false;

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

    if( delta_x == 0 )
      game.player_bank = 0;
    else if( game.horizontal_move_cooldown == 0 )
    {
        if( next_x >= PLAYER_MIN_X && next_x <= PLAYER_MAX_X )
        {
            game.player_x = next_x;
            int bank_strength = 1;
            if( delta_y != 0 )
              bank_strength = 2;
            game.player_bank = delta_x * bank_strength;
        }
        else
          game.player_bank = 0;
        game.horizontal_move_cooldown = PLAYER_HORIZONTAL_MOVE_INTERVAL;
    }

    if( delta_y != 0 && game.vertical_move_cooldown == 0 )
    {
        if( delta_y < 0 )
          game.player_direction = PLAYER_DIRECTION_UP;
        else
          game.player_direction = PLAYER_DIRECTION_DOWN;

        if( next_y >= PLAYER_MIN_Y && next_y <= PLAYER_MAX_Y )
          game.player_y = next_y;
        game.vertical_move_cooldown = PLAYER_VERTICAL_MOVE_INTERVAL;
    }
}

void shooter_fire()
{
    int[ 2 ] bullet_slots;
    int available_bullets = 0;

    for( int slot = 0; slot < MAX_BULLETS; ++slot )
    {
        if( !game.bullets[ slot ].active )
        {
            bullet_slots[ available_bullets ] = slot;
            ++available_bullets;
            if( available_bullets == 2 )
              break;
        }
    }

    if( available_bullets < 2 )
      return;

    for( int shot = 0; shot < 2; ++shot )
    {
        shooter_bullet* bullet = &game.bullets[ bullet_slots[ shot ] ];
        bullet->x = game.player_x;
        if( shot == 0 )
          --bullet->x;
        else
          ++bullet->x;
        if( bullet->x < ARENA_MIN_X + 1 )
          bullet->x = ARENA_MIN_X + 1;
        if( bullet->x > ARENA_MAX_X - 1 )
          bullet->x = ARENA_MAX_X - 1;
        bullet->y = game.player_y;
        bullet->dx = 0;
        bullet->dy = -4;
        bullet->active = true;
    }

    game.fire_cooldown = 8;
}

int shooter_boxes_overlap(
    int first_x,
    int first_y,
    int first_half_width,
    int first_half_height,
    int second_x,
    int second_y,
    int second_half_width,
    int second_half_height
)
{
    return first_x - first_half_width <= second_x + second_half_width &&
        first_x + first_half_width >= second_x - second_half_width &&
        first_y - first_half_height <= second_y + second_half_height &&
        first_y + first_half_height >= second_y - second_half_height;
}

void shooter_update_coins()
{
    int player_x = game.player_x * 10 + 5;
    int player_y =
        game.player_y * PLAYER_Y_STEP_PIXELS + PLAYER_Y_OFFSET_PIXELS;

    for( int slot = 0; slot < MAX_COINS; ++slot )
    {
        shooter_coin* coin = &game.coins[ slot ];
        if( !coin->active )
          continue;

        if( (game.frame + 1) % BACKGROUND_SCROLL_INTERVAL == 0 )
          ++coin->y_pixels;

        int coin_x = coin->x * 10 + 5;
        int coin_y = coin->y_pixels;
        if( shooter_boxes_overlap(
              player_x,
              player_y,
              PLAYER_HITBOX_HALF_WIDTH,
              PLAYER_HITBOX_HALF_HEIGHT,
              coin_x,
              coin_y,
              COIN_HITBOX_HALF_SIZE,
              COIN_HITBOX_HALF_SIZE
            ) )
        {
            coin->active = false;
            ++game.coins_collected;
        }
        else if( coin_y - COIN_HITBOX_HALF_SIZE >= SCREEN_HEIGHT )
          coin->active = false;
    }
}

void shooter_update_debris()
{
    for( int slot = 0; slot < MAX_DEBRIS; ++slot )
    {
        shooter_debris* debris = &game.debris[ slot ];
        if( !debris->active )
          continue;

        if( !debris->grounded )
        {
            debris->x_subpixels += debris->velocity_x;
            debris->y_subpixels += debris->velocity_y;
            debris->velocity_y += 2;
            ++debris->age;

            if( debris->age >= DEBRIS_BURST_DURATION )
            {
                debris->grounded = true;
                debris->velocity_x = 0;
                debris->velocity_y = 0;
            }
        }
        else
        {
            ++debris->age;
            if( (game.frame + 1) % BACKGROUND_SCROLL_INTERVAL == 0 )
              debris->y_subpixels += 16;
        }

        if( debris->x_subpixels <= -DEBRIS_RENDER_HALF_SIZE * 16 ||
            debris->x_subpixels >=
              (SCREEN_WIDTH + DEBRIS_RENDER_HALF_SIZE) * 16 ||
            debris->y_subpixels <= -DEBRIS_RENDER_HALF_SIZE * 16 ||
            debris->y_subpixels >=
              (SCREEN_HEIGHT + DEBRIS_RENDER_HALF_SIZE) * 16 )
          debris->active = false;
    }
}

void shooter_update_bullets()
{
    for( int bullet_slot = 0; bullet_slot < MAX_BULLETS; ++bullet_slot )
    {
        shooter_bullet* bullet = &game.bullets[ bullet_slot ];
        if( !bullet->active )
          continue;

        int previous_y = bullet->y;
        bullet->x += bullet->dx;
        bullet->y += bullet->dy;

        if( bullet->x < ARENA_MIN_X + 1 || bullet->x > ARENA_MAX_X - 1 )
        {
            bullet->active = false;
            continue;
        }

        for( int enemy_slot = 0; enemy_slot < MAX_ENEMIES; ++enemy_slot )
        {
            shooter_enemy* enemy = &game.enemies[ enemy_slot ];
            int bullet_x = bullet->x * 10 + 5;
            int bullet_start_y =
                previous_y * PLAYER_Y_STEP_PIXELS +
                PLAYER_BULLET_Y_OFFSET_PIXELS;
            int bullet_end_y =
                bullet->y * PLAYER_Y_STEP_PIXELS +
                PLAYER_BULLET_Y_OFFSET_PIXELS;
            int bullet_min_y = bullet_start_y;
            int bullet_max_y = bullet_end_y;
            if( bullet_min_y > bullet_max_y )
            {
                bullet_min_y = bullet_end_y;
                bullet_max_y = bullet_start_y;
            }

            int enemy_x = enemy->x * 10 + 5;
            int enemy_y = enemy->y * ENEMY_Y_STEP_PIXELS + 10;
            if( enemy->active &&
                bullet_x - LASER_HITBOX_HALF_WIDTH <=
                  enemy_x + ENEMY_HITBOX_HALF_SIZE &&
                bullet_x + LASER_HITBOX_HALF_WIDTH >=
                  enemy_x - ENEMY_HITBOX_HALF_SIZE &&
                bullet_min_y - LASER_HITBOX_HALF_HEIGHT <=
                  enemy_y + ENEMY_HITBOX_HALF_SIZE &&
                bullet_max_y + LASER_HITBOX_HALF_HEIGHT >=
                  enemy_y - ENEMY_HITBOX_HALF_SIZE )
            {
                bullet->active = false;
                --enemy->hit_points;
                if( enemy->hit_points <= 0 )
                {
                    shooter_spawn_explosion( enemy_x, enemy_y );
                    shooter_spawn_debris( enemy_x, enemy_y );
                    shooter_maybe_drop_coin( enemy->x, enemy->y );
                    enemy->active = false;
                    game.score += 100;
                    ++game.wave_kills;
                    shooter_fill_enemy_slots();
                }
                else
                  enemy->hit_flash = ENEMY_HIT_FLASH_DURATION;
                break;
            }
        }

        if( bullet->y * PLAYER_Y_STEP_PIXELS +
            PLAYER_BULLET_Y_OFFSET_PIXELS +
            LASER_HITBOX_HALF_HEIGHT < 0 )
          bullet->active = false;
    }

    if( game.wave_kills >= game.wave_total )
      shooter_begin_wave();
}

void shooter_check_player_collisions()
{
    int player_x = game.player_x * 10 + 5;
    int player_y =
        game.player_y * PLAYER_Y_STEP_PIXELS + PLAYER_Y_OFFSET_PIXELS;

    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
    {
        shooter_enemy* enemy = &game.enemies[ slot ];
        if( !enemy->active )
          continue;

        int enemy_x = enemy->x * 10 + 5;
        int enemy_y = enemy->y * ENEMY_Y_STEP_PIXELS + 10;
        if( shooter_boxes_overlap(
              player_x,
              player_y,
              PLAYER_HITBOX_HALF_WIDTH,
              PLAYER_HITBOX_HALF_HEIGHT,
              enemy_x,
              enemy_y,
              ENEMY_HITBOX_HALF_SIZE,
              ENEMY_HITBOX_HALF_SIZE
            ) )
        {
            shooter_spawn_explosion( enemy_x, enemy_y );
            shooter_spawn_debris( enemy_x, enemy_y );

            if( game.invulnerability == 0 )
            {
                --game.health;
                game.invulnerability = 15;

                if( game.health <= 0 )
                  game.game_over = true;
            }

            enemy->active = false;
            ++game.wave_kills;
            shooter_fill_enemy_slots();
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

        if( enemy->move_cooldown > 0 )
        {
            --enemy->move_cooldown;
            continue;
        }

        ++enemy->y;
        enemy->move_cooldown =
            shooter_enemy_types[ enemy->type ].move_interval - 1;

        if( enemy->y * ENEMY_Y_STEP_PIXELS + 10 -
            ENEMY_DRAW_SIZE_PIXELS / 2 >=
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
    if( game.horizontal_move_cooldown > 0 )
      --game.horizontal_move_cooldown;
    if( game.vertical_move_cooldown > 0 )
      --game.vertical_move_cooldown;
    if( game.fire_cooldown > 0 )
      --game.fire_cooldown;
    if( game.enemy_cooldown > 0 )
      --game.enemy_cooldown;
    if( game.invulnerability > 0 )
      --game.invulnerability;

    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
    {
        shooter_enemy* enemy = &game.enemies[ slot ];
        if( enemy->active && enemy->hit_flash > 0 )
          --enemy->hit_flash;
    }

    shooter_update_explosions();

    if( game.horizontal_move_cooldown == 0 ||
        game.vertical_move_cooldown == 0 )
      shooter_move_player();

    if( gamepad_button_b() > 0 && game.fire_cooldown == 0 )
      shooter_fire();

    if( game.frame % 3 == 0 )
      shooter_update_bullets();

    shooter_update_coins();

    if( game.enemy_cooldown == 0 )
    {
        shooter_move_enemies();
        game.enemy_cooldown = ENEMY_MOVE_INTERVAL;
    }

    shooter_check_player_collisions();
    shooter_update_debris();

    ++game.frame;
}

void shooter_draw_background( int background_scroll )
{
    int source_y =
        (BACKGROUND_WORLD_HEIGHT - background_scroll) %
        BACKGROUND_WORLD_HEIGHT;
    int destination_y = 0;
    int remaining_height = SCREEN_HEIGHT;

    while( remaining_height > 0 )
    {
        int scene_index = source_y / BACKGROUND_SCENE_HEIGHT;
        int scene_y = source_y % BACKGROUND_SCENE_HEIGHT;
        int scene_remaining = BACKGROUND_SCENE_HEIGHT - scene_y;
        int draw_height = remaining_height;
        int texture_id = JUNGLE_BACKGROUND_TEXTURE_1;

        if( scene_index == 1 )
          texture_id = JUNGLE_BACKGROUND_TEXTURE_2;
        else if( scene_index == 2 )
          texture_id = JUNGLE_BACKGROUND_TEXTURE_3;
        else if( scene_index == 3 )
          texture_id = JUNGLE_BACKGROUND_TEXTURE_4;
        else if( scene_index == 4 )
          texture_id = JUNGLE_BACKGROUND_TEXTURE_5;

        if( draw_height > scene_remaining )
          draw_height = scene_remaining;

        select_texture( texture_id );
        select_region( 0 );
        define_region_center(
            0,
            scene_y,
            SCREEN_WIDTH,
            scene_y + draw_height
        );
        set_drawing_scale( 1, 1 );
        set_multiply_color( color_white );
        draw_region_zoomed_at(
            SCREEN_WIDTH / 2,
            destination_y + draw_height / 2
        );

        source_y += draw_height;
        if( source_y >= BACKGROUND_WORLD_HEIGHT )
          source_y = 0;
        destination_y += draw_height;
        remaining_height -= draw_height;
    }
}

void shooter_draw()
{
    int coin_frame =
        (game.frame / COIN_SPIN_FRAME_DURATION) % COIN_SPIN_FRAME_COUNT;
    int coin_frame_x = coin_frame * COIN_SPRITE_SIZE;
    int background_scroll =
        (game.frame / BACKGROUND_SCROLL_INTERVAL) %
        BACKGROUND_WORLD_HEIGHT;
    shooter_draw_background( background_scroll );

    select_texture( 12 );
    set_drawing_scale( 0.3, 0.3 );
    set_multiply_color( color_white );
    for( int slot = 0; slot < MAX_DEBRIS; ++slot )
    {
        shooter_debris* debris = &game.debris[ slot ];
        if( debris->active )
        {
            int region_x =
                (debris->piece % DEBRIS_ATLAS_COLUMNS) * DEBRIS_CELL_SIZE;
            int region_y =
                (debris->piece / DEBRIS_ATLAS_COLUMNS) * DEBRIS_CELL_SIZE;
            define_region_center(
                region_x,
                region_y,
                region_x + DEBRIS_CELL_SIZE,
                region_y + DEBRIS_CELL_SIZE
            );
            draw_region_zoomed_at(
                debris->x_subpixels / 16,
                debris->y_subpixels / 16
            );
        }
    }
    set_drawing_scale( 1, 1 );

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
    if( game.invulnerability > 0 )
    {
        set_multiply_color( color_red );
        draw_region_zoomed_at(
            SCREEN_WIDTH - 8,
            10 + (MAX_HEALTH - game.health - 1) * 17
        );
        set_multiply_color( color_white );
    }
    set_drawing_scale( 1, 1 );

    set_drawing_scale( 0.3, 0.3 );
    set_multiply_color( color_white );

    for( int slot = 0; slot < MAX_ENEMIES; ++slot )
    {
        if( game.enemies[ slot ].active )
        {
            shooter_enemy* enemy = &game.enemies[ slot ];
            shooter_enemy_type* enemy_type =
                &shooter_enemy_types[ enemy->type ];
            select_texture( enemy_type->texture_id );
            select_region( 0 );
            define_region_center(
                0,
                0,
                enemy_type->sprite_width,
                enemy_type->sprite_height
            );
            if( enemy->hit_flash > 0 )
              set_multiply_color( color_red );
            draw_region_zoomed_at(
                enemy->x * 10 + 5,
                enemy->y * ENEMY_Y_STEP_PIXELS + 10
            );
            set_multiply_color( color_white );
        }
    }
    set_drawing_scale( 1, 1 );

    select_texture( 10 );
    select_region( 0 );
    set_drawing_scale( 0.5, 0.5 );
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
    set_drawing_scale( 1, 1 );

    select_texture( 11 );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    for( int slot = 0; slot < MAX_COINS; ++slot )
    {
        if( game.coins[ slot ].active )
        {
            define_region_center(
                coin_frame_x,
                0,
                coin_frame_x + COIN_SPRITE_SIZE,
                COIN_SPRITE_SIZE
            );
          draw_region_zoomed_at(
              game.coins[ slot ].x * 10 + 5,
              game.coins[ slot ].y_pixels
          );
        }
    }

    select_texture( 8 );
    select_region( 0 );
    define_region_center(
        LASER_TEXTURE_LEFT,
        LASER_TEXTURE_TOP,
        LASER_TEXTURE_LEFT + LASER_SPRITE_WIDTH,
        LASER_TEXTURE_TOP + LASER_SPRITE_HEIGHT
    );
    set_drawing_scale( 0.25, 0.25 );
    set_multiply_color( color_white );
    for( int slot = 0; slot < MAX_BULLETS; ++slot )
    {
        if( game.bullets[ slot ].active )
          draw_region_zoomed_at(
              game.bullets[ slot ].x * 10 + 5,
              game.bullets[ slot ].y * PLAYER_Y_STEP_PIXELS +
              PLAYER_BULLET_Y_OFFSET_PIXELS
          );
    }
    set_drawing_scale( 1, 1 );

    int texture_id = PLAYER_UP_TEXTURE;
    if( game.player_direction == PLAYER_DIRECTION_DOWN )
      texture_id = PLAYER_DOWN_TEXTURE;
    int bank_column = game.player_bank + 2;
    select_texture( texture_id );
    select_region( 0 );
    define_region_center(
        bank_column * PLAYER_BANK_FRAME_WIDTH,
        0,
        (bank_column + 1) * PLAYER_BANK_FRAME_WIDTH,
        PLAYER_BANK_FRAME_HEIGHT
    );
    set_drawing_scale( 0.3, 0.3 );
    if( game.invulnerability > 0 )
      set_multiply_color( color_red );
    else
      set_multiply_color( color_white );
    draw_region_zoomed_at(
        game.player_x * 10 + 5,
        game.player_y * PLAYER_Y_STEP_PIXELS +
        PLAYER_Y_OFFSET_PIXELS
    );
    set_drawing_scale( 1, 1 );

    select_texture( 11 );
    define_region_center(
        coin_frame_x,
        0,
        coin_frame_x + COIN_SPRITE_SIZE,
        COIN_SPRITE_SIZE
    );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    draw_region_zoomed_at( 16, SCREEN_HEIGHT - 16 );
    int coin_digits = 1;
    int coin_value = game.coins_collected;
    while( coin_value >= 10 && coin_digits < 6 )
    {
        coin_value /= 10;
        ++coin_digits;
    }
    draw_number( 30, SCREEN_HEIGHT - 26, game.coins_collected, coin_digits );
    draw_number( 31, SCREEN_HEIGHT - 26, game.coins_collected, coin_digits );
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

int shooter_get_coins_collected()
{
    return game.coins_collected;
}
