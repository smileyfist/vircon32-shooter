#include "video.h"

#include "../game/shooter.h"
#include "../utils/definitions.h"
#include "../utils/utilities.h"
#include "gameover.h"

void gameover_draw()
{
    select_texture( 9 );
    select_region( 0 );
    define_region_center( 0, 0, 640, 360 );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    draw_region_zoomed_at( 320, 180 );

    int[ 8 ] coins_label;
    coins_label[ 0 ] = 'C';
    coins_label[ 1 ] = 'O';
    coins_label[ 2 ] = 'I';
    coins_label[ 3 ] = 'N';
    coins_label[ 4 ] = 'S';
    coins_label[ 5 ] = ':';
    coins_label[ 6 ] = ' ';
    coins_label[ 7 ] = 0;

    int coins = shooter_get_coins_collected();
    int coin_digits = 1;
    int coin_value = coins;
    while( coin_value >= 10 && coin_digits < 6 )
    {
        coin_value /= 10;
        ++coin_digits;
    }

    int label_x = (SCREEN_WIDTH - (7 + coin_digits) * 10) / 2;
    set_multiply_color( color_yellow );
    print_at( label_x, 225, coins_label );
    draw_number( label_x + 70, 225, coins, coin_digits );
    set_multiply_color( color_white );
}
