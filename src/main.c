#include "input.h"
#include "time.h"
#include "video.h"

#include "game/shooter.h"
#include "states/gameover.h"
#include "states/menu.h"
#include "utils/definitions.h"

#include "utils/utilities.c"
#include "game/shooter.c"
#include "states/menu.c"
#include "states/gameover.c"

void main( void )
{
    int current_state = STATE_MENU;

    while( true )
    {
        clear_screen( color_black );

        if( current_state == STATE_MENU )
        {
            menu_draw();

            if( gamepad_button_start() || gamepad_button_a() )
            {
                shooter_start();
                current_state = STATE_PLAYING;
            }
        }
        else if( current_state == STATE_PLAYING )
        {
            shooter_update();
            shooter_draw();

            if( shooter_is_game_over() )
              current_state = STATE_GAME_OVER;
        }
        else
        {
            gameover_draw( shooter_get_score(), shooter_get_wave() );

            if( gamepad_button_start() || gamepad_button_a() )
              current_state = STATE_MENU;
        }

        end_frame();
    }
}
