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
        window() {
            window_ = initscr();
            auto gol = std::make_unique<life::game_of_life>(20);
            api_ = std::make_unique<life::run_api>(gol);
        }

        ~window() {
            endwin();
        }

        auto get_window() -> WINDOW* {
            return window_;
        }

        auto init() -> void {
            game_init(window_);
            api_->get_gol()->populate_cell();
        }

        auto run() -> void {
            do {
                api_->run(1, get_window(), &waddstr);
                wrefresh(get_window());
            }
            while(true);
        }

     private:
        WINDOW* window_;
        std::unique_ptr<life::run_api> api_;

        static auto game_init(WINDOW* wnd) -> void {
            cbreak();
            noecho();
            clear();
            refresh();

            // enable function keys
            keypad(wnd, true);

            // disable input blocking
            nodelay(wnd, true);

            // hide cursor
            curs_set(0);

            if (!has_colors()) {
                endwin();
                printf("ERROR: Terminal does not support color.\n");
                throw std::runtime_error("boom!");
            }

            // enable color modification
            start_color();

            // draw box around screen
            attron(A_BOLD);
            box(wnd, 0, 0);
            attroff(A_BOLD);
        }
    };
} // namespace frontend
