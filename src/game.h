#ifndef GAME_H
#define GAME_H

#include "piece.h"

/* Game states */
typedef enum {
    STATE_INIT,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_QUIT
} GameState;

/* Game context */
typedef struct {
    GameState state;
    Piece     current;
    PieceType next;
    int       score;
    int       lines;
    int       level;
    int       soft_dropping;  /* 1 while Down is held */

    /* 7-bag randomizer */
    PieceType bag[7];
    int       bag_index;

    /* Timing (ms) */
    double    gravity_interval;
    double    gravity_timer;
    double    lock_delay;
    double    lock_timer;
    int       locking;  /* 1 if piece is in lock delay */

    /* RNG seed */
    unsigned int seed;

    /* Tetris flash animation */
    int    flash_active;   /* 1 if flash animation is running */
    double flash_timer;    /* time elapsed in current flash phase */
    int    flash_count;    /* number of inversion toggles remaining */
} Game;

void game_init(Game *g, unsigned int seed);
void game_new_piece(Game *g);
void game_apply_gravity(Game *g, double dt_ms);
void game_update_flash(Game *g, double dt_ms);
void game_lock_piece(Game *g);
void game_hard_drop(Game *g);
int  game_move(Game *g, int drow, int dcol);
int  game_rotate(Game *g, int dir);
void game_toggle_pause(Game *g);
void game_quit(Game *g);

/* Get gravity interval based on current state (normal or soft drop). */
double game_get_gravity_interval(const Game *g);

#endif
