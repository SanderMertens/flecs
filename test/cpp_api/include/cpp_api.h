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

class POD {
public:
    POD() {
        ctor_invoked ++;
        value = 10;
    }

    POD(int v) {
        ctor_invoked ++;
        value = v;
    }    

    ~POD() {
        dtor_invoked ++;
    }

    POD& operator=(const POD& obj) {
        copy_invoked ++;
        this->value = obj.value;
        return *this;
    }

    POD& operator=(POD&& obj) {
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



#endif

