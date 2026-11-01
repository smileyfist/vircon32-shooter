#include "video.h"
#include "menu.h"

#define HANGAR_TEXTURE  26
#define MAP_TEXTURE  27
#define MENU_PIXEL_TEXTURE  28

void menu_draw_screen( int texture_id )
{
    select_texture( texture_id );
    select_region( 0 );
    define_region_center( 0, 0, 640, 360 );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    draw_region_zoomed_at( 320, 180 );
}

void menu_draw_button_outline( int center_x, int center_y, int width, int height )
{
    select_texture( MENU_PIXEL_TEXTURE );
    select_region( 0 );
    define_region_center( 0, 0, 2, 2 );
    set_multiply_color( color_green );

    set_drawing_scale( width / 2.0, 1 );
    draw_region_zoomed_at( center_x, center_y - height / 2 );
    draw_region_zoomed_at( center_x, center_y + height / 2 );

    set_drawing_scale( 1, height / 2.0 );
    draw_region_zoomed_at( center_x - width / 2, center_y );
    draw_region_zoomed_at( center_x + width / 2, center_y );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
}

void menu_draw()
{
    menu_draw_screen( 1 );
}

void menu_hangar_draw()
{
    int[ 8 ] play_label = { 'A', ':', ' ', 'P', 'L', 'A', 'Y', 0 };
    menu_draw_screen( HANGAR_TEXTURE );
    menu_draw_button_outline( 584, 335, 84, 32 );
    set_multiply_color( color_green );
    print_at( 549, 325, play_label );
    set_multiply_color( color_white );
}

void menu_map_draw()
{
    int[ 5 ] play_label = { 'P', 'L', 'A', 'Y', 0 };
    menu_draw_screen( MAP_TEXTURE );
    menu_draw_button_outline( 584, 335, 84, 32 );
    set_multiply_color( color_green );
    print_at( 564, 325, play_label );
    set_multiply_color( color_white );
}
