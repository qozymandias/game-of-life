#include "./frontend.hpp"
#include <string>

auto main(int argc, char** argv) -> int {
    (void)argc;
    (void)argv;

    auto b = life::board_t{
        {1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 1, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 1, 0, 0, 0},
    };
    auto win = frontend::window();
    win.init();

    win.run();


    return 0;
}
