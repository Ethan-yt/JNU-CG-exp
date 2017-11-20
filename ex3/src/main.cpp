#include "App.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    App::init(500, 500, "Ex3 ");
    return 0;
}

#pragma clang diagnostic pop