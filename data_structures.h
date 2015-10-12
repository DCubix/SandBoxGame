#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES

#include <stdlib.h>

int random(int _min, int _max) {
    return _min + (rand() % (int)(_max - _min + 1));
}

enum SB_TYPE {
    SB_TYPE_SAND = 1,
    SB_TYPE_WATER = 2,
    SB_TYPE_METAL = 3
};

struct SB_POINT {
    int x, y;
};

#endif // DATA_STRUCTURES

