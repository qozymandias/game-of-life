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
        , latency{l}
        // , animation{std::move(a)}
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
        explicit game_of_life(int size) {
            board_ = board_t(size, std::vector<int>(size, 0));
            mt_ = std::mt19937{rd_()};
        }

        explicit game_of_life(board_t b)
        : board_{std::move(b)} {};

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
            for (auto const& row : board_) {
                auto s = std::accumulate(row.begin(),
                                         row.end(),
                                         std::string{""},
                                         [delim](auto acc, auto const& x) -> std::string {
                                             return acc + (x == 0b1 ? '*' : ' ') + delim;
                                         })
                         + "\n";
                printer(s.c_str());
            }
        }

        auto print(WINDOW* wind, std::function<int(WINDOW*, const char*)> const& printer, char const delim) -> void {
            for (auto const& row : board_) {
                auto s = std::accumulate(row.begin(),
                                         row.end(),
                                         std::string{""},
                                         [delim](auto acc, auto const& x) -> std::string {
                                             return acc + (x == 0b1 ? '*' : ' ') + delim;
                                         })
                         + "\n";
                printer(wind,s.c_str());
            }
        }

     private:
        board_t board_;
        std::random_device rd_;
        std::mt19937 mt_;

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

     public:
        auto populate_cell() -> void {
            auto normal_dist = std::uniform_int_distribution<int>(0, 3);
            for (auto& rows : get_board()) {
                for (auto& cell : rows) {
                    auto const n = normal_dist(mt_);
                    if (n == 0) {
                        cell = 1;
                    }
                }
            }
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
        explicit run_api(std::unique_ptr<game_of_life>& g) : gol_{std::move(g)} {};

        ~run_api() = default;

        auto run(int latency, WINDOW* wind, std::function<int(WINDOW*, const char*)> const& printer) -> void {
            (void)latency;
            /*
            auto t0 = std::thread(
                [latency]() -> void { std::this_thread::sleep_for(std::chrono::seconds(latency)); });
                */

            gol_->update();
            gol_->print(wind, printer, ' ');
            // sleep(latency);
            // t0.join();
        }

        
        auto get_gol() -> life::game_of_life* {
            return gol_.get();
        }

     private:
        std::unique_ptr<game_of_life> gol_;

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
