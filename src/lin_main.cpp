#include "game.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

static int prev_frame_ms = 0;

bool running = false;

long lin_get_current_time_ms(void);
void lin_process_input(Display* display, XEvent* event, game_input_t* input);
XImage* lin_create_colour_buffer_image(Display* display, int width, int height);
void lin_update_colour_buffer(XImage* colour_buf_img, uint32_t* color_buf, int width, int height);
void lin_draw_colour_buffer(Display* display, Window window, GC gc, XImage* colour_buf_img, int width, int height);

int main(void)
{
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    int screen_width = DisplayWidth(display, screen);
    int screen_height = DisplayHeight(display, screen);

    Window window = XCreateSimpleWindow(
        display,
        root,
        (screen_width / 2) + (WIN_WIDTH / 2), (screen_height / 2) + (WIN_WIDTH / 2),
        WIN_WIDTH, WIN_HEIGHT,
        1,
        BlackPixel(display, screen), WhitePixel(display, screen)
    );
    XStoreName(display, window, "game");

    XClassHint class_hint;
    class_hint.res_name = (char*)"game.res";
    class_hint.res_class = (char*)"game.class";
    XSetClassHint(display, window, &class_hint);

    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);

    game_colour_buffer_t colour_buf = {};
    colour_buf.width = WIN_WIDTH;
    colour_buf.height = WIN_HEIGHT;

    colour_buf.buf = (uint32_t*)malloc(colour_buf.width * colour_buf.height * sizeof(uint32_t));
    if (!colour_buf.buf) {
        fprintf(stderr, "Unable to malloc colour buffer\n");
        exit(1);
    }

    XImage* colour_buf_img = lin_create_colour_buffer_image(display, colour_buf.width, colour_buf.height);
    if (!colour_buf_img) {
        fprintf(stderr, "Unable to creat colour buffer image\n");
        XCloseDisplay(display);
        return 1;
    }

    game_input_t input = {};
    running = true;
    XEvent event;

    while (running) {
        // Timing stuff
        long current_frame_ms = lin_get_current_time_ms();
        int time_to_wait = MILLISECS_PER_FRAME - (current_frame_ms - prev_frame_ms);

        if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) {
            struct timespec sleep_time;
            sleep_time.tv_sec = time_to_wait / 1000;
            sleep_time.tv_nsec = (time_to_wait % 1000) * 1000000;

            nanosleep(&sleep_time, NULL);
        }

        double dt = (lin_get_current_time_ms() - prev_frame_ms) / 1000.0;
        prev_frame_ms = lin_get_current_time_ms();

        // Call game update and renderer
        update_and_render(&input, &colour_buf, dt);

        // Handle platform input
        XNextEvent(display, &event);

        if (event.type == Expose) {
            lin_update_colour_buffer(colour_buf_img, colour_buf.buf, colour_buf.width, colour_buf.height);
            lin_draw_colour_buffer(display, window, gc, colour_buf_img, colour_buf.width, colour_buf.height);
        }

        lin_process_input(display, &event, &input);

        // Platform render
        lin_update_colour_buffer(colour_buf_img, colour_buf.buf, colour_buf.width, colour_buf.height);
        lin_draw_colour_buffer(display, window, gc, colour_buf_img, colour_buf.width, colour_buf.height);
    }

    // free(colour_buf.buf);
    // XFreeGC(display, gc);
    // XDestroyWindow(display, window);
    // XCloseDisplay(display);

    return 0;
}
long lin_get_current_time_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;  // Convert to milliseconds
}

void lin_process_input(Display* display, XEvent* e, game_input_t* input)
{
    switch (e->type) {
    case KeyPress: {
        if (e->xkey.keycode == XKeysymToKeycode(display, XK_Escape)) {
            running = false;
            break;
        }

        if (e->xkey.keycode == XKeysymToKeycode(display, XK_Up)) {
            input->state = 1;
            input->keys |= (1 << 0);
            break;
        } else if (e->xkey.keycode == XKeysymToKeycode(display, XK_Down)) {
            input->state = 1;
            input->keys |= (1 << 1);
            break;
        } else if (e->xkey.keycode == XKeysymToKeycode(display, XK_Left)) {
            input->state = 1;
            input->keys |= (1 << 2);
            break;
        } else if (e->xkey.keycode == XKeysymToKeycode(display, XK_Right)) {
            input->state = 1;
            input->keys |= (1 << 3);
            break;
        }

        // KeySym keysym = XLookupKeysym(&event->xkey, 0);
        // char* keystr = XKeysymToString(keysym);
        // printf("key pressed: %s\n", keystr);
    } break;

    case KeyRelease: {
        if (e->xkey.keycode == XKeysymToKeycode(display, XK_Up)) {
            input->state = 0;
            input->keys ^= (1 << 0);
            break;
        } else if (e->xkey.keycode == XKeysymToKeycode(display, XK_Down)) {
            input->state = 0;
            input->keys ^= (1 << 1);
            break;
        } else if (e->xkey.keycode == XKeysymToKeycode(display, XK_Left)) {
            input->state = 0;
            input->keys ^= (1 << 2);
            break;
        } else if (e->xkey.keycode == XKeysymToKeycode(display, XK_Right)) {
            input->state = 0;
            input->keys ^= (1 << 3);
            break;
        }

        // KeySym keysym = XLookupKeysym(&event->xkey, 0);
        // char* keystr = XKeysymToString(keysym);
        // printf("key released: %s\n", keystr);
    } break;

    // case MotionNotify: {
    // } break;

    // case ButtonPress: {
    // } break;
    }
}

XImage* lin_create_colour_buffer_image(Display* display, int width, int height)
{
    int depth = DefaultDepth(display, DefaultScreen(display));
    XImage* colour_buf_img = XCreateImage(
        display,
        DefaultVisual(display, DefaultScreen(display)),
        depth,
        ZPixmap,
        0,
        (char*)malloc(width * height * sizeof(uint32_t)),
        width,
        height,
        32,
        width * sizeof(uint32_t)
    );
    if (!colour_buf_img) {
        fprintf(stderr, "Failed to create color buffer image.\n");
        return NULL;
    }
    return colour_buf_img;
}

void lin_update_colour_buffer(XImage* colour_buf_img, uint32_t* color_buf, int width, int height)
{
    memcpy(colour_buf_img->data, color_buf, width * height * sizeof(uint32_t));
}

void lin_draw_colour_buffer(Display* display, Window window, GC gc, XImage* colour_buf_img, int width, int height)
{
    XPutImage(display, window, gc, colour_buf_img, 0, 0, 0, 0, width, height);
}
