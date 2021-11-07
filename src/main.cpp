#include "./frontend.hpp"
#include <ncurses.h>
#include <string>

auto main(int argc, char** argv) -> int {
    if (argc < 2) {
        std::cout << "provide window size n\n";
        return 1;
    }
    int tick = 800000;
    int const& n = std::stoi(std::string{argv[1]});
    auto const win = frontend::window(n, tick);

    if (win.init({0, n * 0.55}) == 0) {
        win.run();
    }
    return 0;
}
