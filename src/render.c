#include "render.h"
#include "board.h"
#include "piece.h"
#include "theme.h"
#include <ncurses.h>
#include <locale.h>
#include <string.h>

/*
 * Layout (3-column):
 *   Left panel:  score, lines, level
 *   Center:      playfield (10 cells wide × 20 rows, each cell = 2 chars)
 *   Right panel: next piece, status, controls
 */

/* Offsets for drawing (row, col in terminal coordinates) */
#define FIELD_Y      1
#define LEFT_PANEL_X 2
#define LEFT_PANEL_W 14
#define FIELD_X      (LEFT_PANEL_X + LEFT_PANEL_W)  /* board starts after left panel */
#define PANEL_X      (FIELD_X + BOARD_WIDTH * 2 + 3)  /* right of playfield + border */



void render_init(void) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);  /* Hide cursor */
    timeout(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        theme_init();
    }
}

void render_cleanup(void) {
    endwin();
}

/* Draw a single cell at terminal position (ty, tx). 2 chars wide. */
static void draw_cell(int ty, int tx, int color_id, int is_ghost) {
    if (is_ghost) {
        attron(COLOR_PAIR(COLOR_GHOST) | A_DIM);
        mvaddstr(ty, tx, "░░");
        attroff(COLOR_PAIR(COLOR_GHOST) | A_DIM);
    } else if (color_id > 0) {
        attron(COLOR_PAIR(color_id) | A_BOLD);
        mvaddstr(ty, tx, "██");
        attroff(COLOR_PAIR(color_id) | A_BOLD);
    } else {
        mvaddstr(ty, tx, "  ");
    }
}

/* Draw the playfield border and contents */
static void draw_playfield(const Game *g) {
    int fy = FIELD_Y;
    int fx = FIELD_X;

    attron(COLOR_PAIR(COLOR_BORDER));

    /* Top border */
    mvaddstr(fy - 1, fx - 1, "┌");
    for (int c = 0; c < BOARD_WIDTH * 2; c++)
        addstr("─");
    addstr("┐");

    /* Side borders and content rows */
    for (int r = 0; r < VISIBLE_HEIGHT; r++) {
        int ty = fy + r;
        mvaddstr(ty, fx - 1, "│");
        mvaddstr(ty, fx + BOARD_WIDTH * 2, "│");
    }

    /* Bottom border */
    mvaddstr(fy + VISIBLE_HEIGHT, fx - 1, "└");
    for (int c = 0; c < BOARD_WIDTH * 2; c++)
        addstr("─");
    addstr("┘");

    attroff(COLOR_PAIR(COLOR_BORDER));

    /* Build a display buffer: start with locked board cells */
    int display[VISIBLE_HEIGHT][BOARD_WIDTH];
    int ghost_mask[VISIBLE_HEIGHT][BOARD_WIDTH];
    memset(display, 0, sizeof(display));
    memset(ghost_mask, 0, sizeof(ghost_mask));

    for (int r = 0; r < VISIBLE_HEIGHT; r++) {
        int br = HIDDEN_HEIGHT + r;
        for (int c = 0; c < BOARD_WIDTH; c++) {
            display[r][c] = board[br][c];
        }
    }

    /* Ghost piece */
    if (g->state == STATE_RUNNING) {
        int ghost_row = piece_ghost_row(&g->current);
        Piece ghost = g->current;
        ghost.row = ghost_row;
        int ghost_cells[4][2];
        piece_get_cells(&ghost, ghost_cells);
        for (int i = 0; i < 4; i++) {
            int vr = ghost_cells[i][0] - HIDDEN_HEIGHT;
            int vc = ghost_cells[i][1];
            if (vr >= 0 && vr < VISIBLE_HEIGHT && vc >= 0 && vc < BOARD_WIDTH) {
                if (display[vr][vc] == 0) {
                    ghost_mask[vr][vc] = 1;
                }
            }
        }

        /* Active piece (overwrites ghost if overlapping) */
        int cells[4][2];
        piece_get_cells(&g->current, cells);
        for (int i = 0; i < 4; i++) {
            int vr = cells[i][0] - HIDDEN_HEIGHT;
            int vc = cells[i][1];
            if (vr >= 0 && vr < VISIBLE_HEIGHT && vc >= 0 && vc < BOARD_WIDTH) {
                display[vr][vc] = piece_color(g->current.type);
                ghost_mask[vr][vc] = 0;
            }
        }
    }

    /* Render cells */
    int flash_invert = g->flash_active && (g->flash_count % 2 == 1);
    for (int r = 0; r < VISIBLE_HEIGHT; r++) {
        for (int c = 0; c < BOARD_WIDTH; c++) {
            int ty = fy + r;
            int tx = fx + c * 2;
            if (flash_invert && display[r][c] > 0) {
                attron(COLOR_PAIR(display[r][c]) | A_REVERSE | A_BOLD);
                mvaddstr(ty, tx, "██");
                attroff(COLOR_PAIR(display[r][c]) | A_REVERSE | A_BOLD);
            } else {
                draw_cell(ty, tx, display[r][c], ghost_mask[r][c]);
            }
        }
    }
}

/* Draw the next piece preview */
static void draw_next_piece(const Game *g) {
    int px = PANEL_X;
    int py = FIELD_Y;

    attron(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    mvprintw(py, px, "NEXT:");
    attroff(COLOR_PAIR(COLOR_LABEL) | A_BOLD);

    /* Clear preview area (4x4) */
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            draw_cell(py + 1 + r, px + c * 2, 0, 0);

    /* Draw next piece in preview */
    Piece preview;
    piece_spawn(&preview, g->next);
    preview.row = 0;
    preview.col = 0;

    int cells[4][2];
    piece_get_cells(&preview, cells);
    int color = piece_color(g->next);
    for (int i = 0; i < 4; i++) {
        int tr = py + 1 + cells[i][0];
        int tc = px + cells[i][1] * 2;
        draw_cell(tr, tc, color, 0);
    }
}

/* Draw score / lines / level panel (left column) */
static void draw_stats(const Game *g) {
    int px = LEFT_PANEL_X;
    int py = FIELD_Y;

    attron(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    mvprintw(py, px, "SCORE:");
    attroff(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    attron(COLOR_PAIR(COLOR_LABEL));
    mvprintw(py + 1, px, "%-10d", g->score);
    attroff(COLOR_PAIR(COLOR_LABEL));

    attron(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    mvprintw(py + 3, px, "LINES:");
    attroff(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    attron(COLOR_PAIR(COLOR_LABEL));
    mvprintw(py + 4, px, "%-10d", g->lines);
    attroff(COLOR_PAIR(COLOR_LABEL));

    attron(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    mvprintw(py + 6, px, "LEVEL:");
    attroff(COLOR_PAIR(COLOR_LABEL) | A_BOLD);
    attron(COLOR_PAIR(COLOR_LABEL));
    mvprintw(py + 7, px, "%-10d", g->level);
    attroff(COLOR_PAIR(COLOR_LABEL));

    /* Show current theme name */
    attron(COLOR_PAIR(COLOR_LEGEND) | A_DIM);
    mvprintw(py + 9, px, "%-10s", theme_name());
    attroff(COLOR_PAIR(COLOR_LEGEND) | A_DIM);
}

/* Draw status line */
static void draw_status(const Game *g) {
    int px = PANEL_X;
    int py = FIELD_Y + 7;

    /* Clear status area */
    mvprintw(py, px, "            ");
    mvprintw(py + 1, px, "            ");

    switch (g->state) {
        case STATE_PAUSED:
            attron(A_BLINK | A_BOLD);
            mvprintw(py, px, "** PAUSED **");
            attroff(A_BLINK | A_BOLD);
            mvprintw(py + 1, px, "P to resume");
            break;
        case STATE_GAMEOVER:
            attron(A_BOLD);
            mvprintw(py, px, "GAME OVER!");
            attroff(A_BOLD);
            mvprintw(py + 1, px, "Q to quit");
            break;
        default:
            break;
    }
}

/* Draw controls help */
static void draw_help(void) {
    int px = PANEL_X;
    int py = FIELD_Y + 11;

    attron(COLOR_PAIR(COLOR_LEGEND) | A_DIM);
    mvprintw(py,     px, "Arrows:Move");
    mvprintw(py + 1, px, "Z:CCW X:CW");
    mvprintw(py + 2, px, "Up/Space:Drop");
    mvprintw(py + 3, px, "P:Pause Q:Quit");
    mvprintw(py + 4, px, "T:Theme");
    attroff(COLOR_PAIR(COLOR_LEGEND) | A_DIM);
}

void render_draw(const Game *g) {
    erase();
    draw_playfield(g);
    draw_next_piece(g);
    draw_stats(g);
    draw_status(g);
    draw_help();
    refresh();
}
