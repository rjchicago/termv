#include "piece.h"
#include "board.h"

/*
 * Tetromino definitions: 4 rotation states each, stored as 4 (row, col)
 * offsets relative to the piece's top-left origin in a 4x4 bounding box.
 *
 * Rotation 0 = spawn (North), 1 = East (CW), 2 = South, 3 = West (CCW).
 */

/* I-piece */
static const int I_CELLS[4][4][2] = {
    /* 0: ....  */  {{1,0},{1,1},{1,2},{1,3}},
    /*    ####  */
    /* 1         */ {{0,2},{1,2},{2,2},{3,2}},
    /* 2         */ {{2,0},{2,1},{2,2},{2,3}},
    /* 3         */ {{0,1},{1,1},{2,1},{3,1}},
};

/* O-piece */
static const int O_CELLS[4][4][2] = {
    {{0,1},{0,2},{1,1},{1,2}},
    {{0,1},{0,2},{1,1},{1,2}},
    {{0,1},{0,2},{1,1},{1,2}},
    {{0,1},{0,2},{1,1},{1,2}},
};

/* T-piece */
static const int T_CELLS[4][4][2] = {
    {{0,0},{0,1},{0,2},{1,1}},   /* North: ###  / .#. */
    {{0,1},{1,1},{2,1},{1,2}},   /* East:  .# / .## / .# */
    {{1,0},{1,1},{1,2},{0,1}},   /* South: .#. / ### */
    {{0,1},{1,0},{1,1},{2,1}},   /* West:  .# / ## / .# */
};

/* S-piece — SRS true rotation about center (1,1) in 3x3 */
static const int S_CELLS[4][4][2] = {
    {{0,1},{0,2},{1,0},{1,1}},   /* 0 North: .## / ##. / ... */
    {{0,1},{1,1},{1,2},{2,2}},   /* 1 East:  .#. / .## / ..# */
    {{1,1},{1,2},{2,0},{2,1}},   /* 2 South: ... / .## / ##. */
    {{0,0},{1,0},{1,1},{2,1}},   /* 3 West:  #.. / ##. / .#. */
};

/* Z-piece */
static const int Z_CELLS[4][4][2] = {
    {{0,0},{0,1},{1,1},{1,2}},
    {{0,2},{1,1},{1,2},{2,1}},
    {{1,0},{1,1},{2,1},{2,2}},
    {{0,1},{1,0},{1,1},{2,0}},
};

/* J-piece */
static const int J_CELLS[4][4][2] = {
    {{0,0},{0,1},{0,2},{1,2}},   /* 0:   ### / ..# */
    {{0,0},{0,1},{1,0},{2,0}},   /* CCW: ## / #. / #. */
    {{0,0},{1,0},{1,1},{1,2}},   /* 180: #.. / ### */
    {{0,1},{1,1},{2,1},{2,0}},   /* CW:  .# / .# / ## */
};

/* L-piece */
static const int L_CELLS[4][4][2] = {
    {{0,0},{0,1},{0,2},{1,0}},   /* 0:   ### / #.. */
    {{0,0},{1,0},{2,0},{2,1}},   /* CCW: #. / #. / ## */
    {{1,0},{1,1},{1,2},{0,2}},   /* 180: ..# / ### */
    {{0,0},{0,1},{1,1},{2,1}},   /* CW:  ## / .# / .# */
};

/* Table of all piece cell data, indexed by PieceType */
static const int (*PIECE_TABLE[PIECE_COUNT])[4][2] = {
    I_CELLS, O_CELLS, T_CELLS, S_CELLS, Z_CELLS, J_CELLS, L_CELLS
};

/* Wall kick offsets for basic rotation (non-SRS) */
static const int KICK_OFFSETS[][2] = {
    { 0,  0},
    { 0,  1},  /* right */
    { 0, -1},  /* left */
    {-1,  0},  /* up */
    { 0,  2},  /* right 2 */
    { 0, -2},  /* left 2 */
};
#define NUM_KICKS 6

void piece_get_cells(const Piece *p, int out[4][2]) {
    const int (*cells)[4][2] = PIECE_TABLE[p->type];
    int rot = p->rotation & 3;
    for (int i = 0; i < 4; i++) {
        out[i][0] = p->row + cells[rot][i][0];
        out[i][1] = p->col + cells[rot][i][1];
    }
}

int piece_valid(const Piece *p) {
    int cells[4][2];
    piece_get_cells(p, cells);
    for (int i = 0; i < 4; i++) {
        int r = cells[i][0];
        int c = cells[i][1];
        if (!board_in_bounds(r, c))
            return 0;
        if (!board_is_empty(r, c))
            return 0;
    }
    return 1;
}

int piece_try_rotate(Piece *p, int dir) {
    Piece test = *p;
    test.rotation = (test.rotation + dir + 4) & 3;

    for (int k = 0; k < NUM_KICKS; k++) {
        Piece kicked = test;
        kicked.row += KICK_OFFSETS[k][0];
        kicked.col += KICK_OFFSETS[k][1];
        if (piece_valid(&kicked)) {
            *p = kicked;
            return 1;
        }
    }
    return 0;
}

int piece_color(PieceType type) {
    return (int)type + 1;  /* 1-7 */
}

void piece_spawn(Piece *p, PieceType type) {
    p->type = type;
    p->rotation = 0;
    /* Spawn centered in the hidden buffer area, just above visible region */
    p->row = HIDDEN_HEIGHT - 2;  /* row 18 (one above visible top) */
    p->col = 3;                  /* centered for 4-wide bounding box in 10-wide board */
}

int piece_ghost_row(const Piece *p) {
    Piece ghost = *p;
    while (1) {
        Piece next = ghost;
        next.row++;
        if (!piece_valid(&next))
            break;
        ghost = next;
    }
    return ghost.row;
}
