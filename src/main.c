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

int read_gamepad_buttons()
{
    int buttons = 0;
    if( gamepad_button_start() > 0 )
      buttons |= 1;
    if( gamepad_button_a() > 0 )
      buttons |= 2;
    if( gamepad_button_b() > 0 )
      buttons |= 4;
    if( gamepad_button_x() > 0 )
      buttons |= 8;
    if( gamepad_button_y() > 0 )
      buttons |= 16;
    if( gamepad_button_l() > 0 )
      buttons |= 32;
    if( gamepad_button_r() > 0 )
      buttons |= 64;
    return buttons;
}

void main( void )
{
    int current_state = STATE_MENU;
    int menu_input_delay_frames = frames_per_second;
    int game_over_lockout_frames = 0;
    int previous_gamepad_buttons = 0;

    while( true )
    {
        clear_screen( color_black );
        int gamepad_buttons = read_gamepad_buttons();

        if( current_state == STATE_MENU )
        {
            menu_draw();

            if( menu_input_delay_frames > 0 )
              --menu_input_delay_frames;
            else if( (gamepad_buttons & ~previous_gamepad_buttons) != 0 )
              current_state = STATE_HANGAR;
        }
        else if( current_state == STATE_HANGAR )
        {
            int pressed_buttons =
                gamepad_buttons & ~previous_gamepad_buttons;
            menu_hangar_draw();

            if( pressed_buttons != 0 )
            {
                shooter_start();
                current_state = STATE_PLAYING;
            }
        }
        else if( current_state == STATE_MAP )
        {
            int pressed_buttons =
                gamepad_buttons & ~previous_gamepad_buttons;
            menu_map_draw();

            if( (pressed_buttons & 3) != 0 )
            {
                shooter_start();
                current_state = STATE_PLAYING;
            }
            else if( (pressed_buttons & 4) != 0 )
              current_state = STATE_MENU;
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

            gameover_draw();

            if( game_over_lockout_frames == 0 && gamepad_buttons != 0 )
            {
                menu_input_delay_frames = frames_per_second;
                current_state = STATE_MENU;
            }
        }

        previous_gamepad_buttons = gamepad_buttons;
        end_frame();
    }
}
