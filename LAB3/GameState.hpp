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
	std::vector<std::vector<bool>> visited;
	CNF knowledge_base;

	int encode_var(int x, int y, char type){
		int base = (x*gridSize+y)*10;
		if (type=='P')return base+1;
		if (type=='W')return base+2;
		if (type=='B')return base+3;
		if (type=='S')return base+4;
		return base;
	}

public:
	GameState(){
		visited.assign(gridSize, std::vector<bool>(gridSize, false));
	}

	void update_perception(int x, int y, const Perception& p){
		visited[x][y] = true;

		knowledge_base.push_back({p.breeze ? encode_var(x, y, 'B') : -encode_var(x, y, 'B')});
		knowledge_base.push_back({p.stench ? encode_var(x, y, 'S') : -encode_var(x, y, 'S')});
		knowledge_base.push_back({-encode_var(x, y, 'P')});
		knowledge_base.push_back({-encode_var(x, y, 'W')});
	}

	bool is_safe(int x, int y){
		CNF test_pit = knowledge_base;
		test_pit.push_back({encode_var(x, y,'P')});

		CNF test_wumpus = knowledge_base;
		test_wumpus.push_back({encode_var(x, y,'W')});

		bool pit_possible = run_dpll(test_pit);
		bool wumpus_possible = run_dpll(test_wumpus);

		return !pit_possible && !wumpus_possible;
	}
};

#endif
