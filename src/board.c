#include "board.h"
#include <string.h>

int board[BOARD_HEIGHT][BOARD_WIDTH];

void board_init(void) {
    memset(board, 0, sizeof(board));
}

int board_cell(int row, int col) {
    if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH)
        return -1;
    return board[row][col];
}

void board_set(int row, int col, int val) {
    if (row >= 0 && row < BOARD_HEIGHT && col >= 0 && col < BOARD_WIDTH)
        board[row][col] = val;
}

int board_is_empty(int row, int col) {
    return board_cell(row, col) == 0;
}

int board_in_bounds(int row, int col) {
    return row >= 0 && row < BOARD_HEIGHT && col >= 0 && col < BOARD_WIDTH;
}

void board_lock(int coords[4][2], int color_id) {
    for (int i = 0; i < 4; i++) {
        int r = coords[i][0];
        int c = coords[i][1];
        board_set(r, c, color_id);
    }
}

int board_clear_lines(void) {
    int cleared = 0;

    for (int r = BOARD_HEIGHT - 1; r >= 0; r--) {
        int full = 1;
        for (int c = 0; c < BOARD_WIDTH; c++) {
            if (board[r][c] == 0) {
                full = 0;
                break;
            }
        }
        if (full) {
            cleared++;
            /* Shift all rows above down by 1 */
            for (int rr = r; rr > 0; rr--) {
                memcpy(board[rr], board[rr - 1], sizeof(board[rr]));
            }
            /* Clear top row */
            memset(board[0], 0, sizeof(board[0]));
            /* Re-check this row since it now has new content */
            r++;
        }
    }

    return cleared;
}
