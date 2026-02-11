#include "input.h"
#include "theme.h"
#include <ncurses.h>

InputAction input_poll(void) {
    int ch = getch();
    if (ch == ERR)
        return ACTION_NONE;

    switch (ch) {
        case KEY_LEFT:
            return ACTION_LEFT;
        case KEY_RIGHT:
            return ACTION_RIGHT;
        case KEY_DOWN:
            return ACTION_DOWN;
        case KEY_UP:
        case 'z':
        case 'Z':
            return ACTION_ROTATE_CCW;
        case KEY_SR:       /* Shift+Up in some terminals */
        case 'x':
        case 'X':
            return ACTION_ROTATE_CW;
        case ' ':
            return ACTION_HARD_DROP;
        case 'p':
        case 'P':
            return ACTION_PAUSE;
        case 't':
        case 'T':
            return ACTION_THEME;
        case 'q':
        case 'Q':
        case 27:  /* Escape */
            return ACTION_QUIT;
        default:
            return ACTION_NONE;
    }
}

void input_handle(Game *g, InputAction action) {
    switch (action) {
        case ACTION_LEFT:
            game_move(g, 0, -1);
            break;
        case ACTION_RIGHT:
            game_move(g, 0, 1);
            break;
        case ACTION_DOWN:
            g->soft_dropping = 1;
            game_move(g, 1, 0);
            break;
        case ACTION_DOWN_RELEASE:
            g->soft_dropping = 0;
            break;
        case ACTION_ROTATE_CW:
            game_rotate(g, -1);
            break;
        case ACTION_ROTATE_CCW:
            game_rotate(g, 1);
            break;
        case ACTION_HARD_DROP:
            game_hard_drop(g);
            break;
        case ACTION_PAUSE:
            game_toggle_pause(g);
            break;
        case ACTION_THEME:
            theme_cycle();
            break;
        case ACTION_QUIT:
            game_quit(g);
            break;
        case ACTION_NONE:
            break;
    }
}
