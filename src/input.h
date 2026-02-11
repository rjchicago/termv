#ifndef INPUT_H
#define INPUT_H

#include "game.h"

/* Key action results */
typedef enum {
    ACTION_NONE,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_DOWN,
    ACTION_DOWN_RELEASE,
    ACTION_ROTATE_CW,
    ACTION_ROTATE_CCW,
    ACTION_HARD_DROP,
    ACTION_PAUSE,
    ACTION_THEME,
    ACTION_QUIT
} InputAction;

/* Read a key from ncurses (non-blocking). Returns ACTION_*. */
InputAction input_poll(void);

/* Process an action on the game. */
void input_handle(Game *g, InputAction action);

#endif
