#include <iostream>
#include <vector>
#include <set>
#include "APIClient.hpp"
#include "GameState.hpp"

struct Position { int x, y; };
bool operator<(const Position& a, const Position& b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

std::string get_turn(int current_dir, int target_dir) {
    if (current_dir == target_dir) return "Forward";
    if ((current_dir + 1) % 4 == target_dir) return "TurnLeft";
    return "TurnRight";
}

int main() {
    APIClient client("localhost", 8080);
    GameState state;
    Perception p;

    if (!client.start_game(p)) return 1;

    int x = 0, y = 0, dir = 0; 
    bool has_gold = false;
    std::set<Position> visited;

    while (!p.gameOver) {
        state.update_perception(x, y, p);
        visited.insert({x, y});

        if (p.glitter && !has_gold) {
            client.send_action("Grab", p);
            has_gold = true;
            continue;
        }

        std::string action;

        if (has_gold) {
            if (x == 0 && y == 0) action = "Climb";
            else if (x > 0) action = get_turn(dir, 2);
            else if (y > 0) action = get_turn(dir, 3);
        } else {
            Position next_move = {-1, -1};
            Position neighbors[4] = {{x+1, y}, {x, y+1}, {x-1, y}, {x, y-1}};
            
            for (auto n : neighbors) {
                if (n.x >= 0 && n.x < 4 && n.y >= 0 && n.y < 4) {
                    if (visited.find(n) == visited.end() && state.is_safe(n.x, n.y)) {
                        next_move = n;
                        break;
                    }
                }
            }

            if (next_move.x != -1) {
                int target_dir = dir;
                if (next_move.x > x) target_dir = 0;
                else if (next_move.y > y) target_dir = 1;
                else if (next_move.x < x) target_dir = 2;
                else if (next_move.y < y) target_dir = 3;

                action = get_turn(dir, target_dir);
            } else {
                if (x > 0) action = get_turn(dir, 2);
                else if (y > 0) action = get_turn(dir, 3);
                else action = "Climb";
            }
        }

        client.send_action(action, p);

        if (action == "TurnLeft") dir = (dir + 1) % 4;
        else if (action == "TurnRight") dir = (dir + 3) % 4;
        else if (action == "Forward" && !p.bump) {
            if (dir == 0) x++;
            else if (dir == 1) y++;
            else if (dir == 2) x--;
            else if (dir == 3) y--;
        }
    }

    std::cout << p.message << "\nPuntaje: " << p.score << "\n";
    return 0;
}
