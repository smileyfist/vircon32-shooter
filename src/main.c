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
    int menu_input_delay_frames = frames_per_second;
    int game_over_lockout_frames = 0;

    while( true )
    {
        clear_screen( color_black );

        if( current_state == STATE_MENU )
        {
            int menu_button_pressed =
                gamepad_button_start() ||
                gamepad_button_a() ||
                gamepad_button_b() ||
                gamepad_button_x() ||
                gamepad_button_y() ||
                gamepad_button_l() ||
                gamepad_button_r();

            menu_draw();

            if( menu_input_delay_frames > 0 )
              --menu_input_delay_frames;
            else if( menu_button_pressed )
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
            {
              current_state = STATE_GAME_OVER;
              game_over_lockout_frames = 2 * frames_per_second;
            }
        }
        else
        {
            if( game_over_lockout_frames > 0 )
              --game_over_lockout_frames;

            gameover_draw(
                shooter_get_score(),
                shooter_get_wave(),
                game_over_lockout_frames == 0
            );

            if( game_over_lockout_frames == 0 &&
                (gamepad_button_start() ||
                 gamepad_button_a() ||
                 gamepad_button_b() ||
                 gamepad_button_x() ||
                 gamepad_button_y() ||
                 gamepad_button_l() ||
                 gamepad_button_r()) )
            {
                menu_input_delay_frames = frames_per_second;
                current_state = STATE_MENU;
            }
        }

        end_frame();
    }
}
