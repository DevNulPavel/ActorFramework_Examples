#include <caf/all.hpp>

#include "HelloWorldExample.h"


int main(int argc, char** argv) {
    return caf::exec_main(hello_world_caf_main, argc, argv);
}
