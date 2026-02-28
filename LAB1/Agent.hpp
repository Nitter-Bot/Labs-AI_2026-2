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

class GoalBasedAgent:public Agent
{
public:
    std::pair<int, int> getShot() override
    {

        // Buscar si hay algún HIT en el tablero de rastreo
        for(int y = 0; y < 10; y++)
        {
            for(int x = 0; x < 10; x++)
            {

                if(trackingBoard.grid[y][x] == -1)
                {

                    // Revisar vecinos
                    int dx[] = {1, -1, 0, 0};
                    int dy[] = {0, 0, 1, -1};

                    for(int i = 0; i < 4; i++)
                    {
                        int nx = x + dx[i];
                        int ny = y + dy[i];

                        if(nx >= 0 && nx < 10 && ny >= 0 && ny < 10)
                        {
                            if(trackingBoard.grid[ny][nx] == 0)
                            {
                                return {nx, ny};
                            }
                        }
                    }
                }
            }
        }

        int x, y;
        do{
            x = rnd() % 10;
            y = rnd() % 10;
        }while(trackingBoard.grid[y][x] != 0);

        return {x, y};
    }
};

class GoalBasedAgentInsane:public Agent
{
public:
    std::pair<int, int> getShot() override
    {

        for(int y = 0; y < 10; y++){
            for(int x = 0; x < 10; x++){

                if(trackingBoard.grid[y][x] == -1)
                {

                    
                    if(x+1 < 10 && trackingBoard.grid[y][x+1] == -1)
                    {
                        int nx = x+2;
                        if(nx < 10 && trackingBoard.grid[y][nx] == 0)
                            return {nx, y};
                    }

                    // Si hay hit abajo
                    if(y+1 < 10 && trackingBoard.grid[y+1][x] == -1)
                    {
                        int ny = y+2;
                        if(ny < 10 && trackingBoard.grid[ny][x] == 0)
                            return {x, ny};
                    }
                }
            }
        }

        for(int y = 0; y < 10; y++)
        {
            for(int x = 0; x < 10; x++)
            {

                if(trackingBoard.grid[y][x] == -1)
                {

                    int dx[] = {1, -1, 0, 0};
                    int dy[] = {0, 0, 1, -1};

                    for(int i = 0; i < 4; i++)
                    {
                        int nx = x + dx[i];
                        int ny = y + dy[i];

                        if(nx >= 0 && nx < 10 && ny >= 0 && ny < 10)
                        {
                            if(trackingBoard.grid[ny][nx] == 0){
                                return {nx, ny};
                            }
                        }
                    }
                }
            }
        }
        int x, y;
        do
        {
            x = rnd() % 10;
            y = rnd() % 10;
        }while(trackingBoard.grid[y][x] != 0);

        return {x, y};
    }
};

#endif
