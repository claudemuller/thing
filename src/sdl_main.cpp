#include "game.h"
#include <SDL2/SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

static int prev_frame_ms = 0;

bool running = false;

void sdl_process_input(game_input_t* input);
bool sdl_init(SDL_Window* window, SDL_Renderer* renderer);

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error init'ing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return 1;
    }

    game_colour_buffer_t colour_buf = {};
    colour_buf.width = WIN_WIDTH;
    colour_buf.height = WIN_HEIGHT;

    colour_buf.buf = (uint32_t*)malloc(sizeof(uint32_t) * WIN_WIDTH * WIN_HEIGHT);
    if (!colour_buf.buf) {
        fprintf(stderr, "Error allocating colour buffer\n");
        return 1;
    }

    // float* zbuf = (float*)malloc(sizeof(float) * WIN_WIDTH * WIN_HEIGHT);
    // if (!zbuf) {
    //     fprintf(stderr, "error allocating z buffer\n");
    //     return 1;
    // }

    SDL_Texture* colour_buf_tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WIN_WIDTH,
        WIN_HEIGHT
    );
    if (!colour_buf_tex) {
        fprintf(stderr, "Error creating colour buffer texture: %s\n", SDL_GetError());
        return 1;
    }

    game_input_t input = {};
    running = true;

    while (running) {
        // Timing stuff
        int time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - prev_frame_ms);
        if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) {
            SDL_Delay(time_to_wait);
        }

        double dt = (SDL_GetTicks() - prev_frame_ms) / 1000.0;
        prev_frame_ms = SDL_GetTicks();

        // Start to render
        SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
        SDL_RenderClear(renderer);

        // Handle platform input
        sdl_process_input(&input);

        // Call game update and renderer
        update_and_render(&input, &colour_buf, dt);

        // Finish rendering
        SDL_UpdateTexture(
            colour_buf_tex,
            NULL,
            colour_buf.buf,
            WIN_WIDTH * sizeof(uint32_t)
        );
        SDL_RenderCopy(renderer, colour_buf_tex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    return 0;
}

bool sdl_init(SDL_Window* window, SDL_Renderer* renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error init'ing SDL: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void sdl_process_input(game_input_t* input)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_ESCAPE: {
                running = false;
            } break;

            case SDLK_UP:
                input->state = e.key.state;
                input->keys |= (1 << 0);
                break;
            case SDLK_DOWN:
                input->state = e.key.state;
                input->keys |= (1 << 1);
                break;
            case SDLK_LEFT:
                input->state = e.key.state;
                input->keys |= (1 << 2);
                break;
            case SDLK_RIGHT:
                input->state = e.key.state;
                input->keys |= (1 << 3);
                break;
            case SDLK_w:
            case SDLK_a:
            case SDLK_s:
            case SDLK_d: {
                input->type = e.key.type;
                input->keycode = e.key.keysym.sym;
                input->mod = e.key.keysym.mod;
                input->state = e.key.state;
            } break;
            }
        }
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
            case SDLK_UP:
                input->state = e.key.state;
                input->keys ^= (1 << 0);
                break;
            case SDLK_DOWN:
                input->state = e.key.state;
                input->keys ^= (1 << 1);
                break;
            case SDLK_LEFT:
                input->state = e.key.state;
                input->keys ^= (1 << 2);
                break;
            case SDLK_RIGHT:
                input->state = e.key.state;
                input->keys ^= (1 << 3);
                break;
            case SDLK_w:
            case SDLK_a:
            case SDLK_s:
            case SDLK_d: {
                // input->test = 0;
                input->type = e.key.type;
                input->keycode = e.key.keysym.sym;
                input->mod = e.key.keysym.mod;
                input->state = e.key.state;
            } break;
            }
        }
    }
}
