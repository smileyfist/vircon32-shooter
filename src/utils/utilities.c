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
