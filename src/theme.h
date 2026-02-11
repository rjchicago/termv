#ifndef THEME_H
#define THEME_H

/* Color pair IDs: 1-7 = piece colors, then: */
#define COLOR_GHOST  8
#define COLOR_BORDER 9
#define COLOR_LABEL  10
#define COLOR_LEGEND 11

/* Initialize the theme system and apply the default theme. */
void theme_init(void);

/* Cycle to the next theme and re-apply color pairs. */
void theme_cycle(void);

/* Return the name of the current theme. */
const char *theme_name(void);

#endif
