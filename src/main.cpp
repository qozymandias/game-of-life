#include "./frontend.hpp"
#include <ncurses.h>
#include <string>

auto main(int argc, char** argv) -> int {
    if (argc < 2) {
        std::cout << "provide window size n\n";
        return 1;
    }

    int n = std::stoi(std::string{argv[1]});
    std::cout << n << '\n';
    auto win = frontend::window(n, {0, n});
    frontend::window::init();
    win.run();

    return 0;
}
