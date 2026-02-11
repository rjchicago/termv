#ifndef RENDER_H
#define RENDER_H

#include "game.h"

void render_init(void);
void render_cleanup(void);
void render_draw(const Game *g);

#endif
