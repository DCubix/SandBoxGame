#include <stdio.h>
#include <vector>
#include <assert.h>
#include <SDL/SDL.h>
#include "data_structures.h"

#define ZOOM 4
#define WIDTH 640
#define HEIGHT 480
#define GAME_WIDTH WIDTH/ZOOM
#define GAME_HEIGHT HEIGHT/ZOOM

#undef main

using namespace std;

int particles[GAME_HEIGHT][GAME_WIDTH] = {0};

int sb_get_particle(int x, int y) {
    if (x > GAME_WIDTH) return -1;
    if (x < 0) return -1;
    if (y > GAME_HEIGHT) return -1;
    if (y < 0) return -1;

    return particles[y][x];
}

int sb_get_particle_bellow(int x, int y) {
    return sb_get_particle(x, y+1);
}

int sb_get_particle_left(int x, int y) {
    return sb_get_particle(x-1, y);
}

int sb_get_particle_right(int x, int y) {
    return sb_get_particle(x+1, y);
}

void sb_add_particle(int x, int y, int type) {
    if (x > GAME_WIDTH) return;
    if (x < 0) return;
    if (y > GAME_HEIGHT) return;
    if (y < 0) return;

    particles[y][x] = type;
}

void sb_delete_particle(int x, int y) {
    if (x > GAME_WIDTH) return;
    if (x < 0) return;
    if (y > GAME_HEIGHT) return;
    if (y < 0) return;

    particles[y][x] = 0;
}

void sb_move_down(int x, int y) {
    int down = y+1;
    if (down < GAME_HEIGHT) {
        int tmp = particles[y][x];
        particles[y][x] = particles[down][x];
        particles[down][x] = tmp;
    }
}

void sb_move_down_left(int x, int y) {
    int down = y+1;
    int left = x-1;
    if (down < GAME_HEIGHT && left > 0) {
        int tmp = particles[y][x];
        particles[y][x] = particles[down][left];
        particles[down][left] = tmp;
    }
}

void sb_move_down_right(int x, int y) {
    int down = y+1;
    int right = x+1;
    if (down < GAME_HEIGHT && right < GAME_WIDTH) {
        int tmp = particles[y][x];
        particles[y][x] = particles[down][right];
        particles[down][right] = tmp;
    }
}

void sb_move_left(int x, int y) {
    int left = x-1;
    if (left > 0) {
        int tmp = particles[y][x];
        particles[y][x] = particles[y][left];
        particles[y][left] = tmp;
    }
}

void sb_move_right(int x, int y) {
    int right = x+1;
    if (right < GAME_WIDTH) {
        int tmp = particles[y][x];
        particles[y][x] = particles[y][right];
        particles[y][right] = tmp;
    }
}

void sb_particle_update(int x, int y) {
    if (particles[y][x] == 0) return;

    int current = particles[y][x];
    int under   = sb_get_particle(x, y+1);
    int left    = sb_get_particle(x-1, y);
    int right   = sb_get_particle(x+1, y);

    switch (current) {
        case SB_TYPE_SAND: {
            if (under == 0) {
                sb_move_down(x, y);
            } else {
                if (under == SB_TYPE_WATER) {
                    sb_move_down(x, y);
                }
                if (left == 0) {
                    sb_move_down_left(x, y);
                } else if (right == 0) {
                    sb_move_down_right(x, y);
                }
            }
        } break;
        case SB_TYPE_WATER: {
            if (under == 0) {
                sb_move_down(x, y);
            } else {
                int i = random(-1, 1);
                if (i == 1) {
                    if (right == 0)
                        sb_move_right(x, y);
                } else if (i == -1) {
                    if (left == 0)
                        sb_move_left(x, y);
                }
            }
        } break;
        case SB_TYPE_METAL:
            break;
    }
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    // Main variables
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Texture* buffer;
    SDL_Event event;
    bool running = true;

    SDL_Rect screen;
    screen.w = WIDTH;
    screen.h = HEIGHT;
    screen.x = screen.y = 0;

    // Initialization
    win = SDL_CreateWindow("SandBox",
                           SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    buffer = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);

    // Game variables
    bool sand_water = false;
    bool adding = false, deleting = false;
    SB_POINT mouse_pos;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false; break;
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        sand_water = !sand_water;
                    }
                } break;
                case SDL_MOUSEMOTION: {
                    mouse_pos.x = event.motion.x / ZOOM;
                    mouse_pos.y = event.motion.y / ZOOM;
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        adding = true;
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        deleting = true;
                    }
                } break;
                case SDL_MOUSEBUTTONUP: {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        adding = false;
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        deleting = false;
                    }
                } break;
            }
        }

        // Draw to back-buffer
        SDL_SetRenderTarget(ren, buffer);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        for (int y = 0; y < GAME_HEIGHT; y++) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                int p = particles[y][x];
                switch (p) {
                    case SB_TYPE_SAND:
                        SDL_SetRenderDrawColor(ren, 230, 200, 160, 255); break;
                    case SB_TYPE_WATER:
                        SDL_SetRenderDrawColor(ren, 10, 120, 255, 128); break;
                    default:
                        continue;
                }
                SDL_RenderDrawPoint(ren, x, y);
            }
        }

        SDL_RenderPresent(ren);

        // Draw to screen
        SDL_SetRenderTarget(ren, nullptr);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_RenderCopy(ren, buffer, nullptr, &screen);

        SDL_RenderPresent(ren);

        // Update particles
        for (int y = GAME_HEIGHT-1; y >= 0; y--) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                sb_particle_update(x, y);
            }
        }

        if (adding) {
            if (sand_water)
                sb_add_particle(mouse_pos.x, mouse_pos.y, SB_TYPE_WATER);
            else
                sb_add_particle(mouse_pos.x, mouse_pos.y, SB_TYPE_SAND);
        } else if (deleting) {
            sb_delete_particle(mouse_pos.x, mouse_pos.y);
        }

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}

