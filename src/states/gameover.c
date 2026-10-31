#include "video.h"
#include "../utils/utilities.h"
#include "gameover.h"

void gameover_draw( int score, int wave )
{
    set_multiply_color( make_color_rgb( 255, 75, 85 ) );
    print_at( 245, 100, "GAME OVER" );

    set_multiply_color( color_white );
    print_at( 220, 160, "FINAL SCORE" );
    draw_number( 340, 160, score, 5 );
    print_at( 220, 190, "WAVE REACHED" );
    draw_number( 350, 190, wave, 2 );

    set_multiply_color( make_color_rgb( 255, 220, 90 ) );
    print_at( 190, 270, "PRESS ENTER OR X FOR TITLE" );
}
