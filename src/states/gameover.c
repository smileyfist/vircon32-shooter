#include "video.h"
#include "gameover.h"

void gameover_draw()
{
    select_texture( 9 );
    select_region( 0 );
    define_region_center( 0, 0, 640, 360 );
    set_drawing_scale( 1, 1 );
    set_multiply_color( color_white );
    draw_region_zoomed_at( 320, 180 );
}
