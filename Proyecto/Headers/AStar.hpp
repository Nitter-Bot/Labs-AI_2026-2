#ifndef ASTAR_HPP
#define ASTAR_HPP

#include <vector>
#include <array>
#include <queue>
#include <cmath>
#include "Global.hpp"

#define pb push_back
#define INF 1e9

using namespace std;

// Heuristica: Distancia Manhattan pura en coordenadas de grid
inline int m_dist(int x1, int y1, int x2, int y2){
    return abs(x1-x2)+abs(y1-y2);
}

Position calculate_A_star_move(
    const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& map, 
    const Position& start_pos, 
    const Position& target_pos
){
    int start_x;
    start_x = start_pos.x/CELL_SIZE;
    int start_y;
    start_y = start_pos.y/CELL_SIZE;
    
    int goal_x;
    goal_x = target_pos.x/CELL_SIZE;
    int goal_y;
    goal_y = target_pos.y/CELL_SIZE;

    // Si ya estamos encima del objetivo, no hay que moverse
    if(start_x==goal_x&&start_y==goal_y){
        return start_pos;
    }

    // priority_queue para minimo f_score. Estructura: pair<f_score, pair<x, y>>
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>> pq;
    
    // Matrices de coste y rastreo de camino (padres)
    vector<vector<int>> g_score(MAP_WIDTH, vector<int>(MAP_HEIGHT, INF));
    vector<vector<pair<int, int>>> came_from(MAP_WIDTH, vector<pair<int, int>>(MAP_HEIGHT, {-1, -1}));

    g_score[start_x][start_y] = 0;
    pq.push({m_dist(start_x, start_y, goal_x, goal_y), {start_x, start_y}});

    // Vectores de desplazamiento: N, S, E, W
    int dx[] = {0, 0, 1, -1};
    int dy[] = {-1, 1, 0, 0};

    bool found;
    found = false;

    // Bucle principal de A*
    while(!pq.empty()){
        auto curr = pq.top();
        pq.pop();

        int cx;
        cx = curr.second.first;
        int cy;
        cy = curr.second.second;

        if(cx==goal_x&&cy==goal_y){
            found = true;
            break;
        }

        // Poda: Si encontramos un camino mas corto hacia este nodo previamente, ignorar esta rama
        if(curr.first>g_score[cx][cy]+m_dist(cx, cy, goal_x, goal_y)){
            continue;
        }

        for(int i=0;i<4;++i){
            int nx;
            nx = cx+dx[i];
            int ny;
            ny = cy+dy[i];

            // Verificacion de limites
            if(nx>=0&&nx<MAP_WIDTH&&ny>=0&&ny<MAP_HEIGHT){
                if(map[nx][ny]!=Cell::Wall){
                    int tentative_g;
                    tentative_g = g_score[cx][cy]+1;
                    
                    if(tentative_g<g_score[nx][ny]){
                        g_score[nx][ny] = tentative_g;
                        int f_score;
                        f_score = tentative_g+m_dist(nx, ny, goal_x, goal_y);
                        
                        pq.push({f_score, {nx, ny}});
                        came_from[nx][ny] = {cx, cy};
                    }
                }
            }
        }
    }

    // Reconstruccion del camino
    if(found){
        int curr_x;
        curr_x = goal_x;
        int curr_y;
        curr_y = goal_y;
        
        // Un fantasma no puede teletransportarse al final, debe dar el primer paso.
        // Retrocedemos el camino desde la meta hasta que el "padre" sea el nodo de inicio.
        while(came_from[curr_x][curr_y].first!=start_x||came_from[curr_x][curr_y].second!=start_y){
            pair<int, int> parent;
            parent = came_from[curr_x][curr_y];
            curr_x = parent.first;
            curr_y = parent.second;
        }
        
        // Devolvemos la coordenada pixel real de la siguiente casilla
        return {short(curr_x*CELL_SIZE), short(curr_y*CELL_SIZE)};
    }

    // Fallback: Si no hay ruta (Pacman esta encapsulado o hubo un bug de colision),
    // devolvemos la posicion actual para que el fantasma busque la heuristica mas cercana en el motor original.
    return start_pos;
}

#endif
