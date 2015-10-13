#include <stdio.h>
#include <cmath>
#include "ui.h"

#define ZOOM 4
#define WIDTH 640
#define HEIGHT 480
#define MENU 60
#define GAME_WIDTH WIDTH/ZOOM
#define GAME_HEIGHT (HEIGHT/ZOOM)-(MENU/ZOOM)

#undef main

using namespace std;

SB_PARTICLE* particles[GAME_HEIGHT][GAME_WIDTH] = {nullptr};
int time = 0;

SB_PARTICLE* sb_get_particle(int x, int y) {
    if (x > GAME_WIDTH) return nullptr;
    if (x < 0) return nullptr;
    if (y > GAME_HEIGHT) return nullptr;
    if (y < 0) return nullptr;

    return particles[y][x];
}

SB_PARTICLE* sb_get_particle_bellow(int x, int y) {
    return sb_get_particle(x, y+1);
}

SB_PARTICLE* sb_get_particle_left(int x, int y) {
    return sb_get_particle(x-1, y);
}

SB_PARTICLE* sb_get_particle_right(int x, int y) {
    return sb_get_particle(x+1, y);
}

void sb_add_particle(int x, int y, int type, float temp=20.0f, float maxtemp=100.0f, float mintemp=0.0f) {
    if (x > GAME_WIDTH) return;
    if (x < 0) return;
    if (y > GAME_HEIGHT) return;
    if (y < 0) return;

    auto p = new SB_PARTICLE();
    p->max_temp = maxtemp;
    p->min_temp = mintemp;
    p->temp = temp;
    p->type = type;

    switch (type) {
        case SB_TYPE_WATER:
            p->gas = false;
            p->fluid = true;
            break;
        case SB_TYPE_STEAM:
            p->gas = true;
            p->fluid = false;
            break;
        case SB_TYPE_FIRE:
            p->gas = true;
            p->fluid = false;
            break;
        default:
            p->gas = p->fluid = false;
            break;
    }

    particles[y][x] = p;
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
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[down][x];
        particles[down][x] = tmp;
    } else {
        particles[y][x] = 0;
    }
}

void sb_move_up(int x, int y) {
    int tup = y-1;
    if (tup > 0) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[tup][x];
        particles[tup][x] = tmp;
    } else {
        particles[y][x] = 0;
    }
}

void sb_move_up_left(int x, int y) {
    int up = y-1;
    int left = x-1;
    if (up > 0 && left > 0) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[up][left];
        particles[up][left] = tmp;
    } else {
        particles[y][x] = 0;
    }
}

void sb_move_up_right(int x, int y) {
    int up = y-1;
    int right = x+1;
    if (up > 0 && right < GAME_WIDTH) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[up][right];
        particles[up][right] = tmp;
    } else {
        particles[y][x] = 0;
    }
}

void sb_move_down_left(int x, int y) {
    int down = y+1;
    int left = x-1;
    if (down < GAME_HEIGHT && left > 0) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[down][left];
        particles[down][left] = tmp;
    }
}

void sb_move_down_right(int x, int y) {
    int down = y+1;
    int right = x+1;
    if (down < GAME_HEIGHT && right < GAME_WIDTH) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[down][right];
        particles[down][right] = tmp;
    }
}

void sb_move_left(int x, int y) {
    int left = x-1;
    if (left > 0) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[y][left];
        particles[y][left] = tmp;
    }
}

void sb_move_right(int x, int y) {
    int right = x+1;
    if (right < GAME_WIDTH) {
        SB_PARTICLE* tmp = particles[y][x];
        particles[y][x] = particles[y][right];
        particles[y][right] = tmp;
    }
}

void sb_transfer_heat(int x, int y) {
    SB_PARTICLE* current = sb_get_particle(x, y);
    if (current == nullptr) return;

    SB_PARTICLE* top     = sb_get_particle(x, y-1);
    SB_PARTICLE* under   = sb_get_particle(x, y+1);
    SB_PARTICLE* left    = sb_get_particle(x-1, y);
    SB_PARTICLE* right   = sb_get_particle(x+1, y);

    float heat = 0.0f;

    if (top != nullptr) {
        heat += top->temp - current->temp;
    }
    if (under != nullptr) {
        heat += under->temp - current->temp;
    }
    if (left != nullptr) {
        heat += left->temp - current->temp;
    }
    if (right != nullptr) {
        heat += right->temp - current->temp;
    }

    particles[y][x]->temp += heat / 2.0f;
}

void sb_particle_update(int x, int y, int delta) {
    if (particles[y][x] == 0) return;

    SB_PARTICLE* current = sb_get_particle(x, y);
    SB_PARTICLE* top     = sb_get_particle(x, y-1);
    SB_PARTICLE* under   = sb_get_particle(x, y+1);
    SB_PARTICLE* left    = sb_get_particle(x-1, y);
    SB_PARTICLE* right   = sb_get_particle(x+1, y);

    sb_transfer_heat(x, y);

    switch (current->type) {
        case SB_TYPE_SAND: {
            SB_PARTICLE* dl = sb_get_particle(x-1, y+1);
            SB_PARTICLE* dr = sb_get_particle(x+1, y+1);
            if (under == nullptr) {
                sb_move_down(x, y);
            } else {
                if (under->type == SB_TYPE_WATER) {
                    sb_move_down(x, y);
                } else if (under->type == SB_TYPE_SAND) {
                    if (dl == nullptr)
                        sb_move_down_left(x, y);
                    else if (dr == nullptr)
                        sb_move_down_right(x, y);
                    else if (dr == nullptr && dl == nullptr) {
                        int i = random(-1, 1);
                        if (i == 1) {
                            sb_move_down_right(x, y);
                        } else if (i == -1) {
                            sb_move_down_left(x, y);
                        }
                    }
                }
            }
        } break;
        case SB_TYPE_WATER: {
            if (current->temp > current->max_temp) {
                particles[y][x]->type = SB_TYPE_STEAM;
            }
            if (under == nullptr) {
                sb_move_down(x, y);
            } else {
                int i = random(-1, 1);
                if (i == 1) {
                    if (right == nullptr)
                        sb_move_right(x, y);
                } else if (i == -1) {
                    if (left == nullptr)
                        sb_move_left(x, y);
                }
            }
        } break;
        case SB_TYPE_METAL:
            break;
        case SB_TYPE_STEAM: {
            SB_PARTICLE* tl = sb_get_particle(x-1, y-1);
            SB_PARTICLE* tr = sb_get_particle(x+1, y-1);

            int i = random(-1, 1);
            if (i == 1) {
                if (tr == nullptr)
                    sb_move_up_right(x, y);
                else {
                    if (right == nullptr)
                        sb_move_right(x, y);
                }
            } else if (i == -1) {
                if (tl == nullptr)
                    sb_move_up_left(x, y);
                else {
                    if (left == nullptr)
                        sb_move_left(x, y);
                }
            } else {
                if (top == nullptr)
                    sb_move_up(x, y);
            }

        } break;
        case SB_TYPE_FIRE: {
            SB_PARTICLE* tl = sb_get_particle(x-1, y-1);
            SB_PARTICLE* tr = sb_get_particle(x+1, y-1);
            if (top != nullptr) {
                particles[y][x] = nullptr;
            } else if (tl != nullptr) {
                particles[y][x] = nullptr;
            } else if (tr != nullptr) {
                particles[y][x] = nullptr;
            } else {
                int i = random(-1, 1);
                if (i == 1) {
                    if (tr == nullptr)
                        sb_move_up_right(x, y);
                } else if (i == -1) {
                    if (tl == nullptr)
                        sb_move_up_left(x, y);
                } else {
                    sb_move_up(x, y);
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
    SDL_Surface* _temp = SDL_LoadBMP("debug/font.bmp");
    SDL_Surface* sfont = SDL_ConvertSurface(_temp, SDL_GetWindowSurface(win)->format, SDL_SWSURFACE);
    SDL_SetColorKey(sfont, 0x00001000, 0xff00ff);

    font = SDL_CreateTextureFromSurface(ren, sfont);

    SDL_FreeSurface(_temp);
    SDL_FreeSurface(sfont);

    // Initialize matrix
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            particles[y][x] = nullptr;
        }
    }

    // Game variables
    int type = 1;
    float temp = 20.0f, maxtemp = 100.0f, mintemp = 0.0f;

    SB_COLOR sand  = {230, 200, 160, 255};
    SB_COLOR water = {10, 120, 255, 128};
    SB_COLOR metal = {70, 70, 80, 255};
    SB_COLOR fire  = {255, 100, 20, 255};
    SB_COLOR steam = {255, 255, 255, 70};
    SB_COLOR c = {255, 255, 255, 255};

    int then, now, delta;
    now = SDL_GetTicks();

    uistate.renderer = ren;
    uistate.font = font;

    while (running) {
        then = SDL_GetTicks();
        delta = now - then;
        now = then;

        time += delta;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false; break;
                case SDL_MOUSEMOTION: {
                    uistate.mouse_pos.x = event.motion.x / ZOOM;
                    uistate.mouse_pos.y = event.motion.y / ZOOM;
                    uistate.x = event.motion.x;
                    uistate.y = event.motion.y;
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
                SB_PARTICLE* p = particles[y][x];
                if (p == nullptr) continue;
                if (p->type == 0) continue;

                switch (p->type) {
                    case SB_TYPE_SAND:
                        c = sand; break;
                    case SB_TYPE_WATER:
                        c = water; break;
                    case SB_TYPE_METAL:
                        c = metal; break;
                    case SB_TYPE_FIRE:
                        c = fire; break;
                    case SB_TYPE_STEAM:
                        c = steam; break;
                }

                auto top = sb_get_particle(x, y-1);
                if (top == nullptr) {
                    c = c.brightness(1.2f);
                } else if (top != nullptr) {
                    if (sb_get_particle(x, y-2) == nullptr) {
                        c = c.brightness(0.9f);
                    }
                }
                //printf("%d, %d, %d\n", color.r, color.g, color.b);
                SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
                SDL_RenderDrawPoint(ren, x, y);
                if (!p->fluid && !p->gas) {
                    if (p->temp > 20.0f) {
                        int q = clamp(int(p->temp), 0, 255);
                        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);
                        SDL_SetRenderDrawColor(ren, 255, 80, 0, q);
                        SDL_RenderDrawPoint(ren, x, y);
                    }
                }
            }
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

        if (sb_button(ID, 5, screen.h+10, "SAND", sand, type, SB_TYPE_SAND)) {
            type = SB_TYPE_SAND;
            temp = 20.0f;
            maxtemp = 2000.0f;
            mintemp = -200.0f;
        }
        if (sb_button(ID, 74, screen.h+10, "WATER", water, type, SB_TYPE_WATER)) {
            type = SB_TYPE_WATER;
            temp = 20.0f;
            maxtemp = 100.0f;
            mintemp = -0.1f;
        }
        if (sb_button(ID, 143, screen.h+10, "METAL", metal, type, SB_TYPE_METAL)) {
            type = SB_TYPE_METAL;
            temp = 20.0f;
            maxtemp = 500.0f;
            mintemp = -0.1f;
        }
        if (sb_button(ID, 212, screen.h+10, "FIRE", fire, type, SB_TYPE_FIRE)) {
            type = SB_TYPE_FIRE;
            temp = 180.0f;
            maxtemp = 400.0f;
            mintemp = 100.0f;
        }
        if (sb_button(ID, 281, screen.h+10, "STEAM", steam, type, SB_TYPE_STEAM)) {
            type = SB_TYPE_STEAM;
            temp = 101.0f;
            maxtemp = 200.0f;
            mintemp = 100.0f;
        }

//        int mx = uistate.mouse_pos.y, my = uistate.mouse_pos.x;
//        auto p = particles[my][mx];
//        if (p != nullptr) {
//            sb_drawtext(font, 10, 10, "Temp: %f C", roundf(p->temp));
//        }

        sb_gui_finish();

        SDL_RenderPresent(ren);

        // Update particles (NORMAL)
        for (int y = GAME_HEIGHT-1; y >= 0; y--) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                SB_PARTICLE* p = sb_get_particle(x, y);
                if (p == nullptr) continue;
                if (p->gas) continue;

                sb_particle_update(x, y, delta);
            }
        }
        // Update particles (FIRE)
        for (int y = 0; y < GAME_HEIGHT; y++) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                SB_PARTICLE* p = sb_get_particle(x, y);
                if (p == nullptr) continue;
                if (!p->gas) continue;

                sb_particle_update(x, y, delta);
            }
        }
        if (time > 1000) time = 0;

        if (uistate.lmb) {
            sb_add_particle(uistate.mouse_pos.x, uistate.mouse_pos.y, type, temp, maxtemp, mintemp);
        } else if (uistate.rmb) {
            sb_delete_particle(uistate.mouse_pos.x, uistate.mouse_pos.y);
        }

        SDL_Delay(22);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}

