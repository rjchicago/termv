#ifndef BOARD_H
#define BOARD_H

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 40
#define VISIBLE_HEIGHT 20
#define HIDDEN_HEIGHT  20

/* Each cell: 0 = empty, 1-7 = piece color ID */
extern int board[BOARD_HEIGHT][BOARD_WIDTH];

void board_init(void);
int  board_cell(int row, int col);
void board_set(int row, int col, int val);
int  board_is_empty(int row, int col);
int  board_in_bounds(int row, int col);

/* Lock the active piece minos into the board. coords is [4][2] (row, col). */
void board_lock(int coords[4][2], int color_id);

/* Check and clear full lines. Returns number of lines cleared. */
int  board_clear_lines(void);

#endif
