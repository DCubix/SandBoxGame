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
    SB_TYPE_FIRE = 4,
    SB_TYPE_STEAM = 5
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

struct SB_PARTICLE {
    int type=1;
    float temp=20.0f;
    float max_temp=2000.0f;
    float min_temp=-100.0f;
    bool fluid=false;
    bool gas=false;
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

