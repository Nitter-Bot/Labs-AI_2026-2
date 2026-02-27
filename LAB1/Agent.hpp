#ifndef AGENT_HPP
#define AGENT_HPP

#include "Player.hpp"
using namespace std;


class Agent:public Player{
public:
    virtual std::pair<int, int> getShot() override = 0;
};

class ReflexAgent:public Agent{
public:
    std::pair<int, int> getShot() override{
        int x, y;
        do{
            x = rnd() % 10;
            y = rnd() % 10;
        }while (trackingBoard.grid[y][x] != 0);
        return {x, y}; 
    }
};

class GoalBasedAgent:public Agent{
public:
    std::pair<int, int> getShot() override{
        int x, y;
        do{
            x = rnd() % 10;
            y = rnd() % 10;
        }while(trackingBoard.grid[y][x] != 0);
        return {x, y};
    }
};

#endif
