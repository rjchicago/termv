#ifndef PIECE_H
#define PIECE_H

/* Piece types */
typedef enum {
    PIECE_I = 0,
    PIECE_O,
    PIECE_T,
    PIECE_S,
    PIECE_Z,
    PIECE_J,
    PIECE_L,
    PIECE_COUNT  /* = 7 */
} PieceType;

/* Active piece state */
typedef struct {
    PieceType type;
    int rotation;  /* 0-3 */
    int row;       /* top-left reference row in board coords */
    int col;       /* top-left reference col in board coords */
} Piece;

/* Get the 4 mino coordinates for a piece. out[4][2] = { {row, col}, ... } */
void piece_get_cells(const Piece *p, int out[4][2]);

/* Attempt rotation. dir: +1 = CW, -1 = CCW. Returns 1 if successful. */
int  piece_try_rotate(Piece *p, int dir);

/* Check if piece position is valid (in bounds, no collision). */
int  piece_valid(const Piece *p);

/* Get the color ID (1-7) for a piece type. */
int  piece_color(PieceType type);

/* Spawn coordinates for a given piece type. Sets p->row, p->col, p->rotation. */
void piece_spawn(Piece *p, PieceType type);

/* Get ghost (hard drop) row for a piece. */
int  piece_ghost_row(const Piece *p);

#endif
