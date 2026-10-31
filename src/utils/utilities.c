#include "video.h"
#include "utilities.h"

void draw_number( int drawing_x, int drawing_y, int value, int digits )
{
    int[ 7 ] text;
    int divisor = 1;

    if( digits < 1 )
      digits = 1;

    if( digits > 6 )
      digits = 6;

    for( int position = 1; position < digits; ++position )
      divisor *= 10;

    for( int position = 0; position < digits; ++position )
    {
        text[ position ] = '0' + (value / divisor) % 10;
        divisor /= 10;
    }

    text[ digits ] = 0;
    print_at( drawing_x, drawing_y, text );
}

void draw_arena_frame()
{
    int[ 63 ] horizontal_border;

    horizontal_border[ 0 ] = '+';
    for( int position = 1; position < 62; ++position )
      horizontal_border[ position ] = '-';
    horizontal_border[ 62 ] = '+';
    horizontal_border[ 63 ] = 0;

    set_multiply_color( make_color_rgb( 40, 90, 110 ) );
    print_at( 10, 40, horizontal_border );
    print_at( 10, 320, horizontal_border );

    for( int row = ARENA_MIN_Y; row <= ARENA_MAX_Y; ++row )
    {
        print_at( 10, row * 20, "|" );
        print_at( 620, row * 20, "|" );
    }
}
