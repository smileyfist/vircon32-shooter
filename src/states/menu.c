#include "video.h"
#include "menu.h"

void menu_draw()
{
    set_multiply_color( make_color_rgb( 60, 220, 255 ) );
    print_at( 220, 75, "NEON DUSK" );

    set_multiply_color( make_color_rgb( 255, 80, 100 ) );
    print_at( 195, 125, "A TOP-DOWN ARCADE SHOOTER" );

    set_multiply_color( color_white );
    print_at( 170, 190, "ARROWS  MOVE" );
    print_at( 170, 220, "Z  FIRE IN YOUR FACING DIRECTION" );

    set_multiply_color( make_color_rgb( 255, 220, 90 ) );
    print_at( 190, 285, "PRESS ENTER OR X TO START" );
}
