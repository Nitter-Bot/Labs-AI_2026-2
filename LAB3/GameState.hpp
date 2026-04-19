#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <string>
#include <vector>
#include "DPLL.hpp"

struct Perception{
    bool stench;
    bool breeze;
    bool glitter;
    bool bump;
    bool scream;
    int score;
    bool gameOver;
    std::string message;
};

class GameState{
private:
    int gridSize = 4;
    CNF knowledge_base;

    int encode_var(int x, int y, char type){
        int base = (x * gridSize + y) * 10;
        if (type == 'P') return base + 1; // Pozo
        if (type == 'W') return base + 2; // Wumpus
        if (type == 'B') return base + 3; // Brisa
        if (type == 'S') return base + 4; // Hedor
        return base;
    }

public:
    GameState() {
        knowledge_base.push_back({-encode_var(0, 0, 'P')});
        knowledge_base.push_back({-encode_var(0, 0, 'W')});

        for (int x = 0; x < gridSize; x++){
            for (int y = 0; y < gridSize; y++){
                std::vector<int> adj_pits, adj_wumpus;
                
                if (x > 0) { adj_pits.push_back(encode_var(x-1, y, 'P')); adj_wumpus.push_back(encode_var(x-1, y, 'W')); }
                if (x < 3) { adj_pits.push_back(encode_var(x+1, y, 'P')); adj_wumpus.push_back(encode_var(x+1, y, 'W')); }
                if (y > 0) { adj_pits.push_back(encode_var(x, y-1, 'P')); adj_wumpus.push_back(encode_var(x, y-1, 'W')); }
                if (y < 3) { adj_pits.push_back(encode_var(x, y+1, 'P')); adj_wumpus.push_back(encode_var(x, y+1, 'W')); }

                // B_{x,y} => P_adj1 v P_adj2 v
                Clause b_implies_p = {-encode_var(x, y, 'B')};
                for (int p : adj_pits) b_implies_p.push_back(p);
                knowledge_base.push_back(b_implies_p);

                // P_adj => B_{x,y} (En CNF: -P_adj v B_{x,y})
                for (int p : adj_pits) knowledge_base.push_back({-p, encode_var(x, y, 'B')});

                // S_{x,y} => W_adj1 v W_adj2 v ... (En CNF: -S v W_adj1 v W_adj2)
                Clause s_implies_w = {-encode_var(x, y, 'S')};
                for (int w : adj_wumpus) s_implies_w.push_back(w);
                knowledge_base.push_back(s_implies_w);

                // W_adj => S_{x,y} (En CNF: -W_adj v S_{x,y})
                for (int w : adj_wumpus) knowledge_base.push_back({-w, encode_var(x, y, 'S')});
            }
        }
    }

    void update_perception(int x, int y, const Perception& p){
        knowledge_base.push_back({p.breeze ? encode_var(x, y, 'B') : -encode_var(x, y, 'B')});
        knowledge_base.push_back({p.stench ? encode_var(x, y, 'S') : -encode_var(x, y, 'S')});
        knowledge_base.push_back({-encode_var(x, y, 'P')});
        knowledge_base.push_back({-encode_var(x, y, 'W')});
    }

    bool is_safe(int x, int y){
        CNF test_pit = knowledge_base;
        test_pit.push_back({encode_var(x, y, 'P')});
        bool pit_possible = run_dpll(test_pit);

        CNF test_wumpus = knowledge_base;
        test_wumpus.push_back({encode_var(x, y, 'W')});
        bool wumpus_possible = run_dpll(test_wumpus);

        return !pit_possible && !wumpus_possible;
    }
};

#endif
