#ifndef UI_H
#define UI_H

#define ID (__LINE__)

#include <SDL/SDL.h>
#include "data_structures.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>

SB_UI uistate = {{0, 0}, 0, 0, 0, 0, nullptr, 0, 0};

void sb_drawchar(SDL_Texture* font, char ch, int x, int y) {
    SDL_Rect src, dst;
    src.w = 14;
    src.h = 24;
    src.x = 0;
    src.y = (ch - 32) * 24;
    dst.w = 11;
    dst.h = 16;
    dst.x = x;
    dst.y = y;

    SDL_Renderer* ren = (SDL_Renderer*)uistate.renderer;
    SDL_RenderCopy(ren, font, &src, &dst);
}

void sb_drawtext(SDL_Texture* font, int x, int y, char* text, ...) {
    va_list args;
    int buf_size;
    char* buf, tmp;

    va_start(args, text);
    buf_size = vsnprintf(&tmp, 0, text, args);
    va_end(args);

    if(buf_size == -1) {
        buf_size = 512;
    }

    buf = (char*)alloca(buf_size + 1);
    va_start(args, text);
    vsnprintf(buf, buf_size + 1, text, args);
    va_end(args);

    while (*buf) {
        sb_drawchar(font, *buf, x, y);
        x += 11;
        buf++;
    }

}

void sb_drawrect(int x, int y, int w, int h, int r, int g, int b, bool fill=true) {
    SDL_Rect re;
    re.x = x;
    re.y = y;
    re.w = w;
    re.h = h;

    SDL_Renderer* ren = (SDL_Renderer*)uistate.renderer;
    SDL_SetRenderDrawColor(ren, r, g, b,255);
    if (fill)
        SDL_RenderFillRect(ren, &re);
    else
        SDL_RenderDrawRect(ren, &re);
}

bool sb_region_hit(int x, int y, int w, int h) {
    if (uistate.x <  x     ||
        uistate.y <  y     ||
        uistate.x >= x + w ||
        uistate.y >= y + h)
        return false;
    return true;
}

void sb_gui_prepare() {
    uistate.hotitem = 0;
}

void sb_gui_finish() {
    if (uistate.lmb == 0) {
        uistate.activeitem = 0;
    } else {
        if (uistate.activeitem == 0)
            uistate.activeitem = -1;
    }
}

bool sb_button(int id, int x, int y, char* text, SB_COLOR col, int stype=1, int mytype=1) {
    if (sb_region_hit(x, y, 64, 22)) {
        uistate.hotitem = id;
        if (uistate.activeitem == 0 && uistate.lmb)
            uistate.activeitem = id;
    }

    sb_drawrect(x, y+2, 64, 22, 0, 0, 0);
    if (uistate.hotitem == id) {
        if (uistate.activeitem == id) {
            sb_drawrect(x, y+1, 64, 22, col.r+20, col.g+20, col.b+20);
        } else {
            sb_drawrect(x, y, 64, 22, col.r+20, col.g+20, col.b+20);
        }
    } else {
        sb_drawrect(x, y, 64, 22, col.r, col.g, col.b);
    }
    int tw = strlen(text) * 11;
    int tx = 32-tw/2;
    int sely = 0;

    sb_drawtext((SDL_Texture*)uistate.font, x+tx, y+2, text);
    if (stype == mytype)
        sb_drawrect(x, y, 64, 22, 255, 0, 0, false);
    if (uistate.lmb == 0 &&
        uistate.hotitem == id &&
        uistate.activeitem == id)
        return true;
    return false;
}

#endif // UI_H

