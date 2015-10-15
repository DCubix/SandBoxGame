#include <stdio.h>
#include <cmath>
#include "ui.h"

#define ZOOM 6
#define WIDTH 640
#define HEIGHT 480
#define MENU 60
#define GAME_WIDTH WIDTH/ZOOM
#define GAME_HEIGHT (HEIGHT/ZOOM)-(MENU/ZOOM)

#define SHIFT 0x1073742049 | 1073742053

#undef main

using namespace std;

int    particles[GAME_HEIGHT][GAME_WIDTH] = {0};
float  temps[GAME_HEIGHT][GAME_WIDTH]     = {0};

int sb_get_particle(int x, int y) {
    if (x < 0) return 0;
    if (x > GAME_WIDTH) return 0;
    if (y < 0) return 0;
    if (y > GAME_HEIGHT) return 0;

    return particles[y][x];
}

void sb_add_particle(int x, int y, int type) {
    if (x < 0) return;
    if (x > GAME_WIDTH) return;
    if (y < 0) return;
    if (y > GAME_HEIGHT) return;
    if (particles[y][x] != 0) return;

    particles[y][x] = type;
    switch (type) {
        case SB_TYPE_WATER:
            temps[y][x] = 10.0f;
            break;
        case SB_TYPE_FIRE:
            temps[y][x] = 500.0f;
            break;
        default:
            temps[y][x] = 20.0f;
            break;
    }
}

void sb_paint_particles(int px, int py, int type, int brushsize=1, bool eraser=false) {
    if (brushsize > 0) {
        for(int y=-brushsize; y <= brushsize; y++) {
            for(int x=-brushsize; x <= brushsize; x++) {
                if(x * x + y * y <= brushsize * brushsize) {
                    if (!eraser)
                        sb_add_particle(px+x, py+y, type);
                    else
                        particles[py+y][px+x] = 0;
                }
            }
        }
    } else {
        if (!eraser)
            sb_add_particle(px, py, type);
        else
            particles[py][px] = 0;
    }
}

// Flood-Fill algorithm
void sb_fill_area(int ax, int ay, int type) {
    if (ax < 0 || ax > GAME_WIDTH) return;
    if (ay < 0 || ay > GAME_HEIGHT) return;

    int node = sb_get_particle(ax, ay);
    if (node != 0) return;

    sb_add_particle(ax, ay, type);

    sb_fill_area(ax+1, ay, type);
    sb_fill_area(ax-1, ay, type);
    sb_fill_area(ax, ay+1, type);
    sb_fill_area(ax, ay-1, type);

    return;
}

float sb_probe_temp(int x, int y) {
    if (x < 0) return 0;
    if (x > GAME_WIDTH) return 0;
    if (y < 0) return 0;
    if (y > GAME_HEIGHT) return 0;

    return temps[y][x];
}

void sb_particle_move(int x, int y, int direction) {
    if (x < 0) return;
    if (x > GAME_WIDTH) return;
    if (y < 0) return;
    if (y > GAME_HEIGHT) return;

    int tmp = particles[y][x];

    switch (direction) {
        case SB_DIR_DOWN: {
            int d = y+1;
            if (d < GAME_HEIGHT) {
                particles[y][x] = particles[d][x];
                particles[d][x] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_DOWN_LEFT: {
            int d = y+1, l = x-1;
            if (d < GAME_HEIGHT && l > 0) {
                particles[y][x] = particles[d][l];
                particles[d][l] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_DOWN_RIGHT: {
            int d = y+1, r = x+1;
            if (d < GAME_HEIGHT && r < GAME_WIDTH) {
                particles[y][x] = particles[d][r];
                particles[d][r] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_LEFT: {
            int l = x-1;
            if (l > 0) {
                particles[y][x] = particles[y][l];
                particles[y][l] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_RIGHT: {
            int r = x+1;
            if (r < GAME_WIDTH) {
                particles[y][x] = particles[y][r];
                particles[y][r] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_UP: {
            int u = y-1;
            if (u > 0) {
                particles[y][x] = particles[u][x];
                particles[u][x] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_UP_LEFT: {
            int u = y-1, l = x-1;
            if (u > 0 && l > 0) {
                particles[y][x] = particles[u][l];
                particles[u][l] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
        case SB_DIR_UP_RIGHT: {
            int u = y-1, r = x+1;
            if (u > 0 && r < GAME_WIDTH) {
                particles[y][x] = particles[u][r];
                particles[u][r] = tmp;
            } else {
                particles[y][x] = 0;
            }
        } break;
    }
}

void sb_particle_heat_transfer(int x, int y) {
    float heat = 0.0f;

    if (y-1 > 0) {
        heat += sb_probe_temp(x, y-1) - sb_probe_temp(x, y);
    }
    if (y+1 < GAME_HEIGHT) {
        heat += sb_probe_temp(x, y+1) - sb_probe_temp(x, y);
    }
    if (x-1 > 0) {
        heat += sb_probe_temp(x-1, y) - sb_probe_temp(x, y);
    }
    if (x+1 < GAME_WIDTH) {
        heat += sb_probe_temp(x+1, y) - sb_probe_temp(x, y);
    }

    temps[y][x] += heat / 4.0f;
    temps[y][x] *= 0.995f;
}

void sb_particle_update(int x, int y) {
    int current = sb_get_particle(x, y);
    if (current == 0) return;

    int top     = sb_get_particle(x, y-1);
    int under   = sb_get_particle(x, y+1);
    int left    = sb_get_particle(x-1, y);
    int right   = sb_get_particle(x+1, y);

    switch (current) {
        case SB_TYPE_METAL:
        case SB_TYPE_WALL:
            break;
        case SB_TYPE_SAND: {
            if (under == 0) {
                sb_particle_move(x, y, SB_DIR_DOWN);
            } else {
                if (under == SB_TYPE_SAND) {
                    int dl = sb_get_particle(x-1, y+1);
                    int dr = sb_get_particle(x+1, y+1);
                    if (dl == 0 || dl == SB_TYPE_WATER) {
                        sb_particle_move(x, y, SB_DIR_DOWN_LEFT);
                    } else if (dr == 0 || dr == SB_TYPE_WATER) {
                        sb_particle_move(x, y, SB_DIR_DOWN_RIGHT);
                    } else if (dr == 0 && dl == 0) {
                        int i = random(-1, 1);
                        if (i == 1) {
                            sb_particle_move(x, y, SB_DIR_DOWN_RIGHT);
                        } else if (i == -1) {
                            sb_particle_move(x, y, SB_DIR_DOWN_LEFT);
                        }
                    }
                } else if (under == SB_TYPE_WATER) {
                    sb_particle_move(x, y, SB_DIR_DOWN);
                }
            }
        } break;
        case SB_TYPE_WATER: {
            if (under == 0) {
                sb_particle_move(x, y, SB_DIR_DOWN);
            } else {
                int i = random(-1, 1);
                if (i == 1) {
                    if (right == 0)
                        sb_particle_move(x, y, SB_DIR_RIGHT);
                } else if (i == -1) {
                    if (left == 0)
                        sb_particle_move(x, y, SB_DIR_LEFT);
                }
            }
        } break;
        case SB_TYPE_FIRE: {
            int tl = sb_get_particle(x-1, y-1);
            int tr = sb_get_particle(x+1, y-1);
            if (top != 0) {
                particles[y][x] = 0;
            } else if (tl != 0) {
                particles[y][x] = 0;
            } else if (tr != 0) {
                particles[y][x] = 0;
            } else {
                int i = random(-1, 1);
                if (i == 1) {
                    sb_particle_move(x, y, SB_DIR_UP_RIGHT);
                } else if (i == -1) {
                    sb_particle_move(x, y, SB_DIR_UP_LEFT);
                } else {
                    sb_particle_move(x, y, SB_DIR_UP);
                }
            }
        } break;
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    // Main variables
    SDL_Window*   win;
    SDL_Renderer* ren;
    SDL_Texture*  buffer;
    SDL_Event     event;
    SDL_Texture*  font;
    bool running = true;

    SDL_Rect screen;
    screen.w = WIDTH;
    screen.h = HEIGHT-MENU;
    screen.x = screen.y = 0;

    // Initialization
    win = SDL_CreateWindow("SandBox",
                           SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    buffer = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);

    // Load font
    SDL_Surface* _temp = SDL_LoadBMP("font.bmp");
    SDL_Surface* sfont = SDL_ConvertSurface(_temp, SDL_GetWindowSurface(win)->format, SDL_SWSURFACE);
    SDL_SetColorKey(sfont, 0x00001000, 0xff00ff);

    font = SDL_CreateTextureFromSurface(ren, sfont);

    SDL_FreeSurface(_temp);
    SDL_FreeSurface(sfont);

    // Initialize matrix
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            particles[y][x] = 0;
            temps[y][x] = 20.0f;
        }
    }

    // Game variables
    bool simulate = true, filling = false, drawline = false;
    int type = SB_TYPE_SAND, brush_size = 0;
    int life = -1;

    SB_COLOR sand  = {230, 200, 160, 255};
    SB_COLOR water = {10, 120, 255, 128};
    SB_COLOR metal = {70, 70, 100, 255};
    SB_COLOR fire  = {255, 100, 20, 255};
    SB_COLOR clne  = {10, 220, 20, 100};
    SB_COLOR ewall = {70, 70, 70, 250};
    SB_COLOR c     = {255, 255, 255, 255};

    int then, now, delta;
    now = SDL_GetTicks();

    uistate.renderer = ren;
    uistate.font = font;

    while (running) {
        then = SDL_GetTicks();
        delta = now - then;
        now = then;

        while (SDL_PollEvent(&event)) {
//            printf("%d\n", event.key.keysym.sym);
            switch (event.type) {
                case SDL_QUIT:
                    running = false; break;
                case SDL_MOUSEMOTION: {
                    uistate.mouse_pos.x = event.motion.x / ZOOM;
                    uistate.mouse_pos.y = event.motion.y / ZOOM;
                    uistate.x = event.motion.x;
                    uistate.y = event.motion.y;
                } break;
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_SPACE)
                        simulate = !simulate;
                    if (event.key.keysym.sym == SHIFT)
                        filling = true;
                    if (event.key.keysym.sym == SDLK_MINUS)
                        brush_size -= brush_size > 0 ? 1 : 0;
                    else if (event.key.keysym.sym == 61)
                        brush_size += brush_size < 4 ? 1 : 0;
                } break;
                case SDL_KEYUP: {
                    if (event.key.keysym.sym == SHIFT)
                        filling = false;
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        uistate.lmb = true;
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        uistate.rmb = true;
                    }
                } break;
                case SDL_MOUSEBUTTONUP: {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        uistate.lmb = false;
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        uistate.rmb = false;
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
                int p = sb_get_particle(x, y);
                if (p == 0) continue;
                switch (p) {
                    case SB_TYPE_SAND:
                        c = sand; break;
                    case SB_TYPE_WALL:
                        c = ewall; break;
                    case SB_TYPE_METAL:
                        c = metal; break;
                    case SB_TYPE_WATER:
                        c = water; break;
                    case SB_TYPE_FIRE:
                        c = fire; break;
                }

                SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
                SDL_RenderDrawPoint(ren, x, y);
            }
        }

        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);
        for (int y = 0; y < GAME_HEIGHT; y++) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                float temp = sb_probe_temp(x, y);
                if (temp > 20.0f) {
                    int alpha = int(255.0f * (temp / 500.0f));
                    SDL_SetRenderDrawColor(ren, 255, 50, 12, alpha);
                    SDL_RenderDrawPoint(ren, x, y);
                } else { continue; }
            }
        }

        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(ren, 50, 255, 50, 90);
        if (brush_size > 0) {
            for(int y=-brush_size; y <= brush_size; y++) {
                for(int x=-brush_size; x <= brush_size; x++) {
                    if(x * x + y * y <= brush_size * brush_size) {
                        SDL_RenderDrawPoint(ren, uistate.mouse_pos.x+x, uistate.mouse_pos.y+y);
                    }
                }
            }
        } else {
            SDL_RenderDrawPoint(ren, uistate.mouse_pos.x, uistate.mouse_pos.y);
        }

        SDL_RenderPresent(ren);

        // Draw to screen
        SDL_SetRenderTarget(ren, nullptr);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        SDL_RenderCopy(ren, buffer, nullptr, &screen);
        SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);
        SDL_RenderDrawLine(ren, 0, screen.h, screen.w, screen.h);

        sb_gui_prepare();

        if (sb_button(ID, 2, screen.h+2, "SAND", sand, type, SB_TYPE_SAND)) {
            type = SB_TYPE_SAND;
        }
        if (sb_button(ID, 54, screen.h+2, "WALL", ewall, type, SB_TYPE_WALL)) {
            type = SB_TYPE_WALL;
        }
        if (sb_button(ID, 106, screen.h+2, "WATER", water, type, SB_TYPE_WATER)) {
            type = SB_TYPE_WATER;
        }
        if (sb_button(ID, 158, screen.h+2, "FIRE", fire, type, SB_TYPE_FIRE)) {
            type = SB_TYPE_FIRE;
        }

        sb_gui_finish();

        if (!simulate) {
            sb_drawtext(font, 10, 10, "PAUSED");
        }

//        auto p = sb_get_particle(uistate.mouse_pos.x, uistate.mouse_pos.y);
//        if (p != 0) {
//            sb_drawtext(font, 10, screen.h-20, "temp: %f Cº", roundf(p->temp));
//        }

        SDL_RenderPresent(ren);

        if (simulate) {
            // Update particles (NORMAL)
            for (int y = GAME_HEIGHT-1; y >= 0; y--) {
                for (int x = 0; x < GAME_WIDTH; x++) {
                    if (sb_get_particle(x, y) == SB_TYPE_FIRE) continue;
                    sb_particle_update(x, y);
                }
            }
            // Update particles (GASES)
            for (int y = 0; y < GAME_HEIGHT; y++) {
                for (int x = 0; x < GAME_WIDTH; x++) {
                    if (sb_get_particle(x, y) != SB_TYPE_FIRE) continue;
                    sb_particle_update(x, y);
                }
            }
        }

        if (uistate.lmb) {
            if (!filling) {
                sb_paint_particles(uistate.mouse_pos.x, uistate.mouse_pos.y, type, brush_size);
            } else {
                sb_fill_area(uistate.mouse_pos.x, uistate.mouse_pos.y, type);
            }
        } else if (uistate.rmb) {
            sb_paint_particles(uistate.mouse_pos.x, uistate.mouse_pos.y, -1, brush_size, true);
        }

        SDL_Delay(5);

        // Update heat
        for (int y = 0; y < GAME_HEIGHT; y++) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                sb_particle_heat_transfer(x, y);
            }
        }

    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}

