#include "game.h"
#include <math.h>

#define KEYDOWN 1
#define KEYUP 0

#define KEY_UP 0b0001
#define KEY_DOWN 0b0010
#define KEY_LEFT 0b0100
#define KEY_RIGHT 0b1000
#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100

static float x = 100;
static int y = 100;

void update_and_render(game_input_t* input, game_colour_buffer_t* colour_buf, double dt)
{
#if 1
    for (int y = 0; y < colour_buf->height; ++y) {
        for (int x = 0; x < colour_buf->width; ++x) {
            colour_buf->buf[y * colour_buf->width + x] = (x % 256) << 16 | (y % 256) << 8 | ((x + y) % 256); // RGB pattern
        }
    }
    return;
#endif

    // Clear buffer with black
    for (size_t i = 0; i < (size_t)colour_buf->height * colour_buf->width; i++) {
        colour_buf->buf[i] = 0x000000;
    }
    // for (size_t i = 0; i < (size_t)win_height * win_width; i++) {
    //     zbuf[i] = 1.0;
    // }

    // Handle input
    float velocity = 100.0f;
    if (input->state == KEYDOWN) {
        if ((input->keys & KEY_UP) == KEY_UP) {
            y -= (int)(velocity * dt);
        }

        if ((input->keys & KEY_DOWN) == KEY_DOWN) {
            y += (int)(velocity * dt);
        }

        if ((input->keys & KEY_LEFT) == KEY_LEFT) {
            x -= (int)(velocity * dt);
        }

        if ((input->keys & KEY_RIGHT) == KEY_RIGHT) {
            // float temp = x + (100.0f * dt) + 100;
            // printf("%f - %d - %f - %d\n", x, (int)x, x / step, (int)(x / step) % step);
            // x += (int)(temp / step) % step;

            x += (int)(velocity * dt);
        }
    }

    draw_grid(colour_buf, colour_buf->width, colour_buf->height);
    draw_rect(colour_buf, (int)x, y, 20, 20, 0xFF0000);
}

void draw_grid(game_colour_buffer_t* colour_buf, int width, int height)
{
    for (size_t y = 0; y < (size_t)height; y += 10) {
        for (size_t x = 0; x < (size_t)width; x += 10) {
            colour_buf->buf[(width * y) + x] = 0xFF333333;
        }
    }
}

void draw_pixel(game_colour_buffer_t* colour_buf, int x, int y, uint32_t colour)
{
    if (x < 0 || x >= colour_buf->width || y < 0 || y >= colour_buf->height) {
        return;
    }
    colour_buf->buf[(colour_buf->width * y) + x] = colour;
}

void draw_line(game_colour_buffer_t* colour_buf, int x0, int y0, int x1, int y1, uint32_t colour)
{
    // m = Δx/Δy = rise/run
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int longest_side_len = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    // Calculate the x and y increment at which to draw
    float x_inc = delta_x / (float)longest_side_len;
    float y_inc = delta_y / (float)longest_side_len;

    // The current x and y at which to draw a pixel
    float cur_x = x0;
    float cur_y = y0;

    for (size_t i = 0; i <= (size_t)longest_side_len; i++) {
        draw_pixel(colour_buf, round(cur_x), round(cur_y), colour);
        cur_x += x_inc;
        cur_y += y_inc;
    }
}

void draw_rect(game_colour_buffer_t* colour_buf, int x, int y, int w, int h, uint32_t colour)
{
    int cur_x = 0, cur_y = 0;
    for (size_t i = 0; i < (size_t)w; i++) {
        for (size_t j = 0; j < (size_t)h; j++) {
            cur_x = x + i;
            cur_y = y + j;
            draw_pixel(colour_buf, cur_x, cur_y, colour);
        }
    }
}
