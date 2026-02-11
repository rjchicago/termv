#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#include "board.h"
#include "piece.h"
#include "game.h"
#include "render.h"
#include "input.h"
#include "version.h"

/* Get current time in milliseconds (monotonic clock) */
static double time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/*
 * Soft-drop key tracking:
 * Since ncurses doesn't give key-release events, we use a simple heuristic:
 * if we haven't seen KEY_DOWN for a few frames, consider it released.
 */
#define SOFT_DROP_TIMEOUT_MS 150.0

int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("termv %s\n", TERMV_VERSION);
        return 0;
    }

    unsigned int seed;
    if (argc > 1) {
        seed = (unsigned int)atoi(argv[1]);
    } else {
        seed = (unsigned int)time(NULL);
    }

    /* Initialize ncurses */
    render_init();

    /* Initialize game */
    Game game;
    game_init(&game, seed);

    double last_time = time_ms();
    double soft_drop_last_seen = 0.0;
    int soft_drop_active = 0;

    /* Main game loop */
    while (game.state != STATE_QUIT) {
        double now = time_ms();
        double dt = now - last_time;
        last_time = now;

        /* Poll all available input this frame */
        InputAction action;
        int got_down = 0;
        while ((action = input_poll()) != ACTION_NONE) {
            if (action == ACTION_DOWN) {
                got_down = 1;
                soft_drop_last_seen = now;
                if (!soft_drop_active) {
                    soft_drop_active = 1;
                    game.soft_dropping = 1;
                }
            }
            input_handle(&game, action);
        }

        /* Soft drop release heuristic */
        if (soft_drop_active && !got_down) {
            if (now - soft_drop_last_seen > SOFT_DROP_TIMEOUT_MS) {
                soft_drop_active = 0;
                game.soft_dropping = 0;
            }
        }

        /* Apply gravity (paused during flash animation) */
        if (game.flash_active) {
            game_update_flash(&game, dt);
        } else if (game.state == STATE_RUNNING) {
            game_apply_gravity(&game, dt);
        }

        /* Render */
        render_draw(&game);

        /* Cap at ~60 FPS (16.6ms per frame) */
        double frame_time = time_ms() - now;
        double sleep_ms = 16.0 - frame_time;
        if (sleep_ms > 0) {
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = (long)(sleep_ms * 1000000);
            nanosleep(&req, NULL);
        }
    }

    /* Cleanup */
    render_cleanup();
    printf("Game Over! Score: %d | Lines: %d | Level: %d\n",
           game.score, game.lines, game.level);

    return 0;
}
