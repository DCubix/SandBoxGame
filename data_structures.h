#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES

#include <stdlib.h>

int random(int _min, int _max) {
    return _min + (rand() % (int)(_max - _min + 1));
}

int clamp(int v, int _min, int _max) {
    if (v > _max)
        return _max;
    else if (v < _min)
        return _min;
    else
        return v;
}

enum SB_TYPE {
    SB_TYPE_SAND = 1,
    SB_TYPE_WATER = 2,
    SB_TYPE_METAL = 3,
    SB_TYPE_WALL = 4,
    SB_TYPE_FIRE = 5
};

enum SB_DIRECTION {
    SB_DIR_UP = 0x001a,
    SB_DIR_DOWN = 0x002b,
    SB_DIR_LEFT = 0x003c,
    SB_DIR_RIGHT = 0x004d,
    SB_DIR_UP_RIGHT = (SB_DIR_UP | SB_DIR_RIGHT),
    SB_DIR_UP_LEFT = (SB_DIR_UP | SB_DIR_LEFT),
    SB_DIR_DOWN_RIGHT = (SB_DIR_DOWN | SB_DIR_RIGHT),
    SB_DIR_DOWN_LEFT = (SB_DIR_DOWN | SB_DIR_LEFT)
};

struct SB_COLOR {
    int r, g, b, a;

    SB_COLOR brightness(float p=0.5f) {
        SB_COLOR col;
        col.r = clamp(int(float(r) * p), 0, 255);
        col.g = clamp(int(float(g) * p), 0, 255);
        col.b = clamp(int(float(b) * p), 0, 255);
        col.a = a;
        return col;
    }
};

struct SB_POINT {
    int x, y;
};

struct SB_UI {
    SB_POINT mouse_pos;
    bool lmb, rmb;

    int hotitem;
    int activeitem;
    void* renderer;
    void* font;

    int x, y;
};

#endif // DATA_STRUCTURES

