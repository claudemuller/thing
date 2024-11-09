#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

typedef struct {
    int width;
    int height;
    uint32_t* buf;
} game_colour_buffer_t;

typedef struct {
    uint32_t type;
    int32_t keycode;
    uint16_t mod;
    uint8_t state;
    unsigned char keys = 0b0000;
} game_input_t;

void update_and_render(game_input_t* input, game_colour_buffer_t* colour_buf, double dt);
void draw_grid(game_colour_buffer_t* colour_buf, int width, int height);
void draw_pixel(game_colour_buffer_t* colour_buf, int x, int y, uint32_t colour);
void draw_line(game_colour_buffer_t* colour_buf, int x0, int y0, int x1, int y1, uint32_t colour);
void draw_rect(game_colour_buffer_t* colour_buf, int x, int y, int w, int h, uint32_t colour);

#endif // !GAME_H_
