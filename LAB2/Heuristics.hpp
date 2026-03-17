#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

#include "State.hpp"
#include <cmath>
#include <functional>
#include <vector>

int misplacedTiles(const State& s) {
    int count = 0;
    for (int i = 0; i < s.n_tiles; ++i) {
        if (s.board[i] != 0) {
            int target = s.board[i] - 1;
            if (i != target) count++;
        }
    }
    return count;
}

int manhattanDistance(const State& s) {
    int md = 0;
    for (int i = 0; i < s.n_tiles; ++i) {
        if (s.board[i] != 0) {
            int target = s.board[i] - 1;
            int current_r = i / s.size;
            int current_c = i % s.size;
            int target_r = target / s.size;
            int target_c = target % s.size;
            md += std::abs(current_r - target_r) + std::abs(current_c - target_c);
        }
    }
    return md;
}

int linearConflict(const State& s) {
    int md = manhattanDistance(s);
    int lcn = 0;

    for (int r = 0; r < s.size; ++r) {
        std::vector<int> row_conflicts(s.size, 0);
        for (int c1 = 0; c1 < s.size; ++c1) {
            int idx1 = r * s.size + c1;
            int val1 = s.board[idx1];
            if (val1 != 0 && (val1 - 1) / s.size == r) {
                for (int c2 = c1 + 1; c2 < s.size; ++c2) {
                    int idx2 = r * s.size + c2;
                    int val2 = s.board[idx2];
                    if (val2 != 0 && (val2 - 1) / s.size == r && val1 > val2) {
                        row_conflicts[c1]++;
                        row_conflicts[c2]++;
                    }
                }
            }
        }
        while (true) {
            int max_c = -1;
            for (int c = 0; c < s.size; ++c) {
                if (row_conflicts[c] > 0 && (max_c == -1 || row_conflicts[c] > row_conflicts[max_c])) {
                    max_c = c;
                }
            }
            if (max_c == -1) break;
            row_conflicts[max_c] = 0;
            lcn++;
            for (int c = 0; c < s.size; ++c) {
                if (row_conflicts[c] > 0) row_conflicts[c]--;
            }
        }
    }

    for (int c = 0; c < s.size; ++c) {
        std::vector<int> col_conflicts(s.size, 0);
        for (int r1 = 0; r1 < s.size; ++r1) {
            int idx1 = r1 * s.size + c;
            int val1 = s.board[idx1];
            if (val1 != 0 && (val1 - 1) % s.size == c) {
                for (int r2 = r1 + 1; r2 < s.size; ++r2) {
                    int idx2 = r2 * s.size + c;
                    int val2 = s.board[idx2];
                    if (val2 != 0 && (val2 - 1) % s.size == c && val1 > val2) {
                        col_conflicts[r1]++;
                        col_conflicts[r2]++;
                    }
                }
            }
        }
        while (true) {
            int max_r = -1;
            for (int r = 0; r < s.size; ++r) {
                if (col_conflicts[r] > 0 && (max_r == -1 || col_conflicts[r] > col_conflicts[max_r])) {
                    max_r = r;
                }
            }
            if (max_r == -1) break;
            col_conflicts[max_r] = 0;
            lcn++;
            for (int r = 0; r < s.size; ++r) {
                if (col_conflicts[r] > 0) col_conflicts[r]--;
            }
        }
    }

    return md + 2 * lcn;
}

#endif
