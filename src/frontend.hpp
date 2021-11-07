#include "life.hpp"
#include <cstdint>
#include <exception>
#include <ncurses.h>
#include <unistd.h>
#include <utility>

namespace frontend {

    class window {
     private:
        int tick_;
        WINDOW* window_;
        std::unique_ptr<life::interface> api_;

     public:
        // NOLINTNEXTLINE
        explicit window(int const size, int const tick)
        : tick_{tick}
        , window_{initscr()}
        , api_{std::make_unique<life::interface>(size)} {}

        ~window() {
            endwin();
        }

        auto run() const -> void {
            while (true) {
                api_->run(tick_, window_, &waddstr);
            }
        }

        [[nodiscard]] auto init(std::pair<int, int> const& range) const -> int {
            api_->get_gol().populate_cell(range.first, range.second);

            cbreak();
            noecho();
            clear();
            refresh();

            keypad(window_, true);
            nodelay(window_, true);
            curs_set(0);

            if (!has_colors()) {
                throw std::runtime_error("ERROR: Terminal does not support color.\n");
            }
            start_color();
            attron(A_BOLD);
            box(window_, 0, 0);
            attroff(A_BOLD);
            return 0;
        }
    };
} // namespace frontend
