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

#include <cmath>
#include <random>
#include <thread>

namespace life {

    using board_t = std::vector<std::vector<int>>;

    class game_of_life {
     private:
        board_t board_;
        std::random_device rd_;
        std::mt19937 mt_;
        std::uniform_int_distribution<int> normal_dist_;

     public:
        game_of_life() = default;
        ~game_of_life() = default;

        explicit game_of_life(int size) {
            board_ = board_t(size, std::vector<int>(size, 0));
        }

        explicit game_of_life(board_t b)
        : board_{std::move(b)} {};

        game_of_life(game_of_life& g) noexcept
        : board_{g.board_}
        , mt_{g.mt_}
        , normal_dist_{g.normal_dist_} {}

        game_of_life(game_of_life&& g) noexcept
        : board_{std::move(g.board_)}
        , mt_{g.mt_}
        , normal_dist_{g.normal_dist_} {}

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

        auto populate_cell(int l, int h) -> void {
            normal_dist_ = std::uniform_int_distribution<int>(l, h);
            for (std::size_t i = 0; i < get_board().size(); ++i) {
                for (std::size_t j = 0; j < board_[i].size(); ++j) {
                    auto const n = normal_dist_(mt_);
                    board_[i][j] = equation(i, j, n) ? 1 : 0;
                }
            }
        }

        auto print(std::function<int(const char*)> const& printer, char const delim) -> void {
            for (auto const& row : board_) {
                printer(mk_string(row, delim).c_str());
            }
        }

        auto print(WINDOW* wind, std::function<int(WINDOW*, const char*)> const& printer, char const delim)
            -> void {
            for (auto const& row : board_) {
                printer(wind, mk_string(row, delim).c_str());
            }
        }

     private:
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

        static auto equation(std::size_t i, std::size_t j, int n) -> bool {
            auto a = std::sqrt(((i - n) ^ 2) + ((j - n) ^ 2));
            // NOLINTNEXTLINE
            return (j == n * std::sin(n) and j < n * std::cos(n))
                   or (a <= n or (i >= n / 2 and i <= n and j >= n / 2 and j <= n));
        }
    };

    class interface {
     public:
        explicit interface(int const size)
        : gol_{life::game_of_life(size)} {};
        ~interface() = default;

        auto get_gol() -> life::game_of_life& {
            return gol_;
        }

        auto run(int latency, WINDOW* wind, std::function<int(WINDOW*, const char*)> const& printer) -> void {
            auto timer_thread = std::thread(
                [latency]() -> void { std::this_thread::sleep_for(std::chrono::microseconds(latency)); });
            auto update_print_thread = std::thread([&wind, &printer, this]() -> void {
                wclear(wind);
                gol_.update();
                gol_.print(wind, printer, ' ');
            });

            timer_thread.join();
            update_print_thread.join();
            wrefresh(wind);
        }

     private:
        life::game_of_life gol_;
    };

} // namespace life
