#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <ncurses.h>
#include <numeric>
#include <utility>
#include <vector>
// #include <ranges>

#include <random>
#include <thread>

namespace life {

    using board_t = std::vector<std::vector<int>>;

    struct state {
        state(WINDOW* w, int l /* , std::unique_ptr<T>&& a*/, board_t&& b)
        : window{w}
        , latency{l} // , animation{std::move(a)}
        , board{b} {};

        WINDOW* window;
        int latency;
        // std::unique_ptr<T> animation;
        board_t const& board;

        auto run() const -> void {
            // TODO(oscar):
            /*
            auto t0 = std::thread([this]() -> void {
                sleep(this->latency);
            });
            */
        }
    };

    class game_of_life {
     public:
        game_of_life() = default;
        ~game_of_life() = default;

        explicit game_of_life(int size)
        : game_of_life() {
            board_ = board_t(size, std::vector<int>(size, 0));
        }

        explicit game_of_life(board_t b)
        : board_{std::move(b)} {};

        game_of_life(game_of_life& g) noexcept {
            this->board_ = board_t(g.board_);
        }

        game_of_life(game_of_life&& g) noexcept {
            this->board_ = board_t(std::move(g.board_));
            mt_ = std::mt19937{rd_()};
        }

        auto operator=(game_of_life&& g) noexcept -> game_of_life& {
            this->board_ = board_t(std::move(g.board_));
            return *this;
        }

        [[nodiscard]] auto get_board() const -> board_t {
            return board_;
        }

        auto get_board() -> board_t& {
            return board_;
        }

        auto update() -> void {
            for (std::size_t i = 0; i < board_.size(); ++i) {
                for (std::size_t j = 0; j < board_[i].size(); ++j) {
                    auto c = count_live_neighbors(board_, static_cast<int>(i), static_cast<int>(j));
                    if (c == 3 or (board_[i][j] != 0 and c == 2)) {
                        board_[i][j] |= 0b10;
                    }
                }
            }
            for (auto& row : board_) {
                for (auto& cell : row) {
                    cell >>= 1;
                }
            }
        }

        auto print(std::function<int(const unsigned)> const& printer, char const delim) -> void {
            for (auto const& rows : get_board()) {
                for (auto const& cell : rows) {
                    printer(cell == 0b1 ? '*' : ' ');
                    printer(delim);
                }
                printer('\n');
            }
        }

        auto print(std::function<int(const char*)> const& printer, char const delim) -> void {
            std::for_each(board_.begin(), board_.end(), [&printer, &delim](auto const& row) -> void {
                printer(mk_string(row, delim).c_str());
            });
        }

        auto print(WINDOW* wind, std::function<int(WINDOW*, const char*)> const& printer, char const delim)
            -> void {
            std::for_each(board_.begin(), board_.end(), [&wind, &printer, &delim](auto const& row) -> void {
                printer(wind, mk_string(row, delim).c_str());
            });
        }

     private:
        board_t board_;
        std::random_device rd_;
        std::mt19937 mt_;
        std::uniform_int_distribution<int> normal_dist_;

        static auto count_live_neighbors(board_t const& board, int x, int y) -> int {
            auto m = static_cast<int>(board.size());
            auto n = static_cast<int>(board[0].size());
            auto count = 0;
            for (auto i = std::max(x - 1, 0); i < std::min(x + 2, m); ++i) {
                for (auto j = std::max(y - 1, 0); j < std::min(y + 2, n); ++j) {
                    count += board[i][j] & 0b01;
                }
            }
            return count -= board[x][y];
        }
        static auto mk_string(std::vector<int> const& row, char const delim) -> std::string {
            return std::accumulate(row.begin(),
                                   row.end(),
                                   std::string{""},
                                   [delim](auto acc, auto const& x) -> std::string {
                                       return acc + (x == 0b1 ? '*' : ' ') + delim;
                                   })
                   + '\n';
        }

     public:
        auto populate_cell(int l, int h) -> void {
            normal_dist_ = std::uniform_int_distribution<int>(l, h / 2);
            for (std::size_t i = 0; i < get_board().size(); ++i) {
                for (std::size_t j = 0; j < board_[i].size(); ++j) {
                    auto const n = normal_dist_(mt_);
                    board_[i][j] = equation(i, j, n) ? 1 : 0;
                }
            }
        }

     private:
        static auto equation(std::size_t i, std::size_t j, int n) -> bool {
            auto a = std::sqrt(((i - n) ^ 2) + ((j - n) ^ 2));
            return a < n or (i < n and j < n);
        }
    };

    /*
    class animation {
     public:
        animation() = default;

     private:
    };
    */

    class run_api {
     public:
        explicit run_api(life::game_of_life& g)
        : gol_{std::move(g)} {};

        ~run_api() = default;

        auto get_gol() -> life::game_of_life& {
            return gol_;
        }

        auto run(int latency, WINDOW* wind, std::function<int(WINDOW*, const char*)> const& printer) -> void {
            auto t0 = std::thread(
                [latency]() -> void { std::this_thread::sleep_for(std::chrono::seconds(latency)); });
            clear();
            gol_.update();
            gol_.print(wind, printer, ' ');
            t0.join();
            refresh();
        }

        auto run(int latency, std::function<int(const char*)> const& printer) -> void {
            auto t0 = std::thread(
                [latency]() -> void { std::this_thread::sleep_for(std::chrono::seconds(latency)); });
            clear();
            gol_.update();
            gol_.print(printer, ' ');
            t0.join();
            refresh();
        }

     private:
        life::game_of_life gol_;

        /*
        std::vector<state<animation>> animations_;
        static auto loop(std::vector<state<animation> const*>& an) -> void {
            std::for_each(an.begin(), an.end(), [](auto const& x) -> void { x->run(); });
        }

        static auto init(WINDOW* curr_window, board_t& b, std::vector<state<animation>>& an) -> void {
            auto a = std::make_unique<life::animation>();
            auto start = std::make_unique<state<animation>>(curr_window, 0, std::move(a), std::move(b));
            an.emplace_back(std::move(start));
        }*/
    };

} // namespace life
