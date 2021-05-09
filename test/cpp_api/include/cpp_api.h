#ifndef CPP_API_H
#define CPP_API_H

/* This generated file contains includes for project dependencies */
#include <cpp_api/bake_config.h>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Mass {
    float value;
};

struct Rotation {
    float value;
};

struct Tag { };

class Pod {
public:
    Pod() {
        ctor_invoked ++;
        value = 10;
    }

    Pod(int v) {
        ctor_invoked ++;
        value = v;
    }    

    ~Pod() {
        dtor_invoked ++;
    }

    Pod& operator=(const Pod& obj) {
        copy_invoked ++;
        this->value = obj.value;
        return *this;
    }

    Pod& operator=(Pod&& obj) {
        move_invoked ++;
        this->value = obj.value;
        return *this;
    }   

    int value;

    static int ctor_invoked;
    static int dtor_invoked;
    static int copy_invoked;
    static int move_invoked;
};

template <typename T>
struct Template {
    T x;
    T y;
};

#endif

