#include "life.hpp"
#include <cstdint>
#include <exception>
#include <ncurses.h>
#include <unistd.h>
#include <utility>

namespace frontend {

    struct player {
        std::pair<int, int> pos;
        char disp_char;
    };

    class window {
     public:
        explicit window(int const size, std::pair<int, int> const& range) {
            auto gol = life::game_of_life(size);
            api_ = std::make_unique<life::run_api>(gol);
            api_->get_gol().populate_cell(range.first, range.second);
        }

        ~window() {
            endwin();
        }

        auto get_window() -> WINDOW* {
            return window_;
        }

        static auto init() -> void {
            game_init();
        }

        auto run() -> void {
            while (true) {
                api_->run(1, &addstr);
            }
        }

        static auto game_init() -> void {
            initscr();
            cbreak();
            noecho();
            clear();
            refresh();
        }

     private:
        WINDOW* window_;
        std::unique_ptr<life::run_api> api_;
    };
} // namespace frontend
