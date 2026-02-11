#include "game.h"
#include "board.h"
#include "piece.h"
#include <stdlib.h>
#include <math.h>

/* ── 7-bag randomizer ────────────────────────────────────────────── */

static void bag_shuffle(Game *g) {
    for (int i = 6; i > 0; i--) {
        int j = rand() % (i + 1);
        PieceType tmp = g->bag[i];
        g->bag[i] = g->bag[j];
        g->bag[j] = tmp;
    }
    g->bag_index = 0;
}

static void bag_fill(Game *g) {
    for (int i = 0; i < 7; i++)
        g->bag[i] = (PieceType)i;
    bag_shuffle(g);
}

static PieceType bag_next(Game *g) {
    if (g->bag_index >= 7)
        bag_fill(g);
    return g->bag[g->bag_index++];
}

/* ── Gravity interval calculation ────────────────────────────────── */

static double calc_gravity_interval(int level) {
    /* 500ms at level 0, decreasing by 20% each level, minimum 50ms */
    double interval = 500.0 * pow(0.8, level);
    if (interval < 50.0)
        interval = 50.0;
    return interval;
}

/* ── Scoring ─────────────────────────────────────────────────────── */

static const int LINE_SCORES[5] = {0, 100, 300, 500, 800};

static void apply_score_lines(Game *g, int lines_cleared) {
    if (lines_cleared < 1 || lines_cleared > 4)
        return;
    g->score += LINE_SCORES[lines_cleared] * (g->level + 1);
    g->lines += lines_cleared;

    /* Tetris! Trigger flash celebration */
    if (lines_cleared == 4) {
        g->flash_active = 1;
        g->flash_timer = 0.0;
        g->flash_count = 4;  /* 4 toggles = 2 full flash cycles */
    }

    /* Level up every 10 lines */
    int new_level = g->lines / 10;
    if (new_level > g->level) {
        g->level = new_level;
        g->gravity_interval = calc_gravity_interval(g->level);
    }
}

/* ── Public API ───────────────────────────────────────────────────── */

void game_init(Game *g, unsigned int seed) {
    g->state = STATE_INIT;
    g->score = 0;
    g->lines = 0;
    g->level = 0;
    g->soft_dropping = 0;
    g->gravity_interval = calc_gravity_interval(0);
    g->gravity_timer = 0.0;
    g->lock_delay = 500.0;  /* 500ms lock delay */
    g->lock_timer = 0.0;
    g->locking = 0;
    g->bag_index = 7;  /* Force refill on first call */
    g->seed = seed;
    g->flash_active = 0;
    g->flash_timer = 0.0;
    g->flash_count = 0;

    srand(seed);
    board_init();

    /* Pre-load next piece and spawn first piece */
    g->next = bag_next(g);
    game_new_piece(g);
    g->state = STATE_RUNNING;
}

void game_new_piece(Game *g) {
    PieceType type = g->next;
    g->next = bag_next(g);

    piece_spawn(&g->current, type);
    g->locking = 0;
    g->lock_timer = 0.0;
    g->gravity_timer = 0.0;

    /* Check if spawn position is valid */
    if (!piece_valid(&g->current)) {
        g->state = STATE_GAMEOVER;
    }
}

void game_lock_piece(Game *g) {
    int cells[4][2];
    piece_get_cells(&g->current, cells);
    board_lock(cells, piece_color(g->current.type));

    int cleared = board_clear_lines();
    if (cleared > 0) {
        apply_score_lines(g, cleared);
    }

    game_new_piece(g);
}

void game_apply_gravity(Game *g, double dt_ms) {
    if (g->state != STATE_RUNNING)
        return;

    double interval = game_get_gravity_interval(g);

    if (g->locking) {
        /* In lock delay: count down */
        g->lock_timer += dt_ms;
        if (g->lock_timer >= g->lock_delay) {
            game_lock_piece(g);
        }
        return;
    }

    g->gravity_timer += dt_ms;
    while (g->gravity_timer >= interval) {
        g->gravity_timer -= interval;

        /* Try to move down */
        Piece next = g->current;
        next.row++;
        if (piece_valid(&next)) {
            g->current = next;
            /* Award soft drop point */
            if (g->soft_dropping)
                g->score += 1;
        } else {
            /* Can't move down — start lock delay */
            g->locking = 1;
            g->lock_timer = 0.0;
            break;
        }
    }
}

void game_update_flash(Game *g, double dt_ms) {
    if (!g->flash_active)
        return;
    g->flash_timer += dt_ms;
    if (g->flash_timer >= 100.0) {
        g->flash_timer -= 100.0;
        g->flash_count--;
        if (g->flash_count <= 0) {
            g->flash_active = 0;
            g->flash_count = 0;
        }
    }
}

void game_hard_drop(Game *g) {
    if (g->state != STATE_RUNNING)
        return;

    int rows_dropped = 0;
    while (1) {
        Piece next = g->current;
        next.row++;
        if (!piece_valid(&next))
            break;
        g->current = next;
        rows_dropped++;
    }
    g->score += rows_dropped * 2;
    game_lock_piece(g);
}

int game_move(Game *g, int drow, int dcol) {
    if (g->state != STATE_RUNNING)
        return 0;

    Piece next = g->current;
    next.row += drow;
    next.col += dcol;
    if (piece_valid(&next)) {
        g->current = next;
        /* If we moved while in lock delay, reset it */
        if (g->locking) {
            g->lock_timer = 0.0;
            /* Check if we're no longer on the ground */
            Piece below = g->current;
            below.row++;
            if (piece_valid(&below)) {
                g->locking = 0;
            }
        }
        return 1;
    }
    return 0;
}

int game_rotate(Game *g, int dir) {
    if (g->state != STATE_RUNNING)
        return 0;

    if (piece_try_rotate(&g->current, dir)) {
        /* Reset lock delay if rotating while locking */
        if (g->locking) {
            g->lock_timer = 0.0;
            Piece below = g->current;
            below.row++;
            if (piece_valid(&below)) {
                g->locking = 0;
            }
        }
        return 1;
    }
    return 0;
}

void game_toggle_pause(Game *g) {
    if (g->state == STATE_RUNNING)
        g->state = STATE_PAUSED;
    else if (g->state == STATE_PAUSED)
        g->state = STATE_RUNNING;
}

void game_quit(Game *g) {
    g->state = STATE_QUIT;
}

double game_get_gravity_interval(const Game *g) {
    if (g->soft_dropping)
        return 50.0;  /* Fast drop: 50ms */
    return g->gravity_interval;
}
