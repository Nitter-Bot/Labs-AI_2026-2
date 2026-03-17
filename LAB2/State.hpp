#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <iostream>
#include <numeric>
#include <random>
#include <chrono>
#include <algorithm>

class State {
public:
    std::vector<int> board;
    int size;
    int n_tiles;

    State() : size(0), n_tiles(0) {}

    State(int s) : size(s), n_tiles(s * s) {
        board.resize(n_tiles);
        std::iota(board.begin(), board.end(), 0);
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        do {
            std::shuffle(board.begin(), board.end(), rng);
        } while (!isSolvable() || isGoal());
    }

    State(int s, const std::vector<int>& b) : size(s), n_tiles(s * s), board(b) {}

    bool isSolvable() const {
        int inv_count = 0;
        int blank_row = 0;
        for (int i = 0; i < n_tiles; ++i) {
            if (board[i] == 0) {
                blank_row = i / size;
                continue;
            }
            for (int j = i + 1; j < n_tiles; ++j) {
                if (board[j] && board[i] > board[j]) {
                    inv_count++;
                }
            }
        }

        if (size % 2 != 0) {
            return inv_count % 2 == 0;
        } else {
            return (inv_count + blank_row) % 2 != 0;
        }
    }

    bool isGoal() const {
        for (int i = 0; i < n_tiles - 1; ++i) {
            if (board[i] != i + 1) return false;
        }
        return board.back() == 0;
    }

    void print() const {
        for (int i = 0; i < n_tiles; ++i) {
            std::cout << board[i] << "\t";
            if ((i + 1) % size == 0) std::cout << "\n";
        }
        std::cout << "\n";
    }

    std::vector<State> getNeighbors() const {
        std::vector<State> neighbors;
        int blank_idx = 0;
        for (int i = 0; i < n_tiles; ++i) {
            if (board[i] == 0) {
                blank_idx = i;
                break;
            }
        }

        int r = blank_idx / size;
        int c = blank_idx % size;
        int moves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for (auto& m : moves) {
            int nr = r + m[0];
            int nc = c + m[1];
            if (nr >= 0 && nr < size && nc >= 0 && nc < size) {
                State next_state = *this;
                std::swap(next_state.board[blank_idx], next_state.board[nr * size + nc]);
                neighbors.push_back(next_state);
            }
        }
        return neighbors;
    }

    bool operator==(const State& other) const {
        return board == other.board;
    }
};

namespace std {
    template <> struct hash<State> {
        size_t operator()(const State& s) const {
            size_t h = 0;
            for (int x : s.board) {
                h ^= hash<int>()(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}

#endif
