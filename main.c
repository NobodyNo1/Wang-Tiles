#include "wang.h"

#ifdef TEST
#include "test.h"
int test(){
    generate_single_tile();
}
#endif

int app(){

    setup();
    generate();
    bake_image();
}

int main() {
    app();
    return 0;
}
