#include "video.h"
#include "menu.h"

void menu_draw()
{
    set_multiply_color( make_color_rgb( 60, 220, 255 ) );
    print_at( 240, 110, "SHOOTER MCSHOOTS" );

    set_multiply_color( make_color_rgb( 255, 220, 90 ) );
    print_at( 280, 220, "[ PLAY ]" );

    set_multiply_color( color_white );
    print_at( 210, 265, "PRESS ANY BUTTON" );
}
