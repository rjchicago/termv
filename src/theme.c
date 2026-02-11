#include "theme.h"
#include <ncurses.h>

typedef struct {
    const char *name;
    short piece[7];  /* color for pieces 1-7 (I,O,T,S,Z,J,L) */
    short ghost;
    short border;
    short label;
    short legend;
} Theme;

static const Theme themes[] = {
    /* PASTEL (current default) */
    {
        "PASTEL",
        { COLOR_CYAN, COLOR_YELLOW, COLOR_MAGENTA,
          COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_WHITE },
        COLOR_WHITE,   /* ghost */
        COLOR_WHITE,   /* border */
        COLOR_WHITE,   /* label */
        COLOR_WHITE    /* legend */
    },
    /* RETRO — warm arcade CRT feel */
    {
        "RETRO",
        { COLOR_CYAN, COLOR_YELLOW, COLOR_MAGENTA,
          COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_WHITE },
        COLOR_YELLOW,  /* ghost */
        COLOR_YELLOW,  /* border */
        COLOR_YELLOW,  /* label */
        COLOR_YELLOW   /* legend */
    },
    /* MATRIX — all green, subdued UI */
    {
        "MATRIX",
        { COLOR_GREEN, COLOR_GREEN, COLOR_GREEN,
          COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN },
        COLOR_GREEN,   /* ghost */
        COLOR_GREEN,   /* border */
        COLOR_GREEN,   /* label */
        COLOR_GREEN    /* legend */
    },
};

#define THEME_COUNT (int)(sizeof(themes) / sizeof(themes[0]))

static int current_theme = 0;

static void apply_theme(const Theme *t) {
    for (int i = 0; i < 7; i++)
        init_pair(i + 1, t->piece[i], -1);
    init_pair(COLOR_GHOST,  t->ghost,  -1);
    init_pair(COLOR_BORDER, t->border, -1);
    init_pair(COLOR_LABEL,  t->label,  -1);
    init_pair(COLOR_LEGEND, t->legend, -1);
}

void theme_init(void) {
    current_theme = 0;
    apply_theme(&themes[current_theme]);
}

void theme_cycle(void) {
    current_theme = (current_theme + 1) % THEME_COUNT;
    apply_theme(&themes[current_theme]);
}

const char *theme_name(void) {
    return themes[current_theme].name;
}
