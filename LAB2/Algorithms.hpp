#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "State.hpp"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <functional>
#include <algorithm>

struct AStarResult {
    bool success;
    std::vector<State> path;
    int nodes_opened;
    int nodes_closed;
};

struct IDAStarResult {
    bool success;
    std::vector<State> path;
    int nodes_explored;
    int max_depth;
};

struct Node {
    State state;
    int g;
    int f;
    bool operator>(const Node& other) const {
        return f > other.f;
    }
};

AStarResult aStar(const State& start, std::function<int(const State&)> heuristic) {
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    std::unordered_map<State, int> open_costs;
    std::unordered_map<State, int> closed;
    std::unordered_map<State, State> parent;

    open.push({start, 0, heuristic(start)});
    open_costs[start] = heuristic(start);

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        if (closed.find(current.state) != closed.end() && closed[current.state] <= current.g) {
            continue;
        }

        closed[current.state] = current.g;

        if (current.state.isGoal()) {
            std::vector<State> path;
            State curr = current.state;
            while (!(curr == start)) {
                path.push_back(curr);
                curr = parent[curr];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return {true, path, (int)open_costs.size(), (int)closed.size()};
        }

        for (const State& neighbor : current.state.getNeighbors()) {
            int next_g = current.g + 1;
            int next_f = next_g + heuristic(neighbor);

            if (closed.find(neighbor) != closed.end() && closed[neighbor] <= next_g) continue;

            if (open_costs.find(neighbor) == open_costs.end() || open_costs[neighbor] > next_f) {
                open_costs[neighbor] = next_f;
                parent[neighbor] = current.state;
                open.push({neighbor, next_g, next_f});
            }
        }
    }
    return {false, {}, (int)open_costs.size(), (int)closed.size()};
}

std::pair<int, bool> idaStarDFS(State current, int g, int limit, std::function<int(const State&)> heuristic, std::vector<State>& path, int& nodes, int& max_d) {
    nodes++;
    max_d = std::max(max_d, g);
    int f = g + heuristic(current);
    if (f > limit) return {f, false};
    if (current.isGoal()) return {f, true};

    int min_limit = 1e9;
    for (const State& neighbor : current.getNeighbors()) {
        if (std::find(path.begin(), path.end(), neighbor) == path.end()) {
            path.push_back(neighbor);
            auto [new_limit, found] = idaStarDFS(neighbor, g + 1, limit, heuristic, path, nodes, max_d);
            if (found) return {new_limit, true};
            min_limit = std::min(min_limit, new_limit);
            path.pop_back();
        }
    }
    return {min_limit, false};
}

IDAStarResult idaStar(const State& start, std::function<int(const State&)> heuristic) {
    int limit = heuristic(start);
    std::vector<State> path = {start};
    int total_nodes = 0;
    int max_depth = 0;

    while (true) {
        auto [new_limit, found] = idaStarDFS(start, 0, limit, heuristic, path, total_nodes, max_depth);
        if (found) {
            return {true, path, total_nodes, max_depth};
        }
        if (new_limit == 1e9) break;
        limit = new_limit;
    }
    return {false, {}, total_nodes, max_depth};
}

#endif
