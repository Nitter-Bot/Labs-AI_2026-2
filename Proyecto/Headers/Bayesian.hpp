#ifndef BAYESIAN_HPP
#define BAYESIAN_HPP

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include "Global.hpp"

#define pb push_back

using namespace std;

// Funcion principal que el fantasma rojo llamara
Position calculate_Bayes_move(
    const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& map, 
    const Position& red_pos, 
    const Position& pacman_pos,
    unsigned char pacman_dir
){
    int px;
    px = pacman_pos.x/CELL_SIZE;
    int py;
    py = pacman_pos.y/CELL_SIZE;

    double max_prob;
    max_prob = -1.0;
    Position best_target;
    best_target = pacman_pos; // Por defecto lo persigue directo

    int R;
    R = 15; // Radio de busqueda maximo

    // Iterar en un bounding box alrededor de Pacman
    for(int x=max(0, px-R);x<=min(MAP_WIDTH-1, px+R);++x){
        for(int y=max(0, py-R);y<=min(MAP_HEIGHT-1, py+R);++y){
            int dist;
            dist = abs(px-x)+abs(py-y);
            
            // Fuera del radio o es la misma celda de pacman
            if(dist>R||dist==0){
                continue;
            }
            // Si es pared, es imposible que vaya ahi
            if(map[x][y]==Cell::Wall){
                continue;
            }

            double prob;
            prob = 1.0;

            // 1. Evidencia de Recompensa (R)
            if(map[x][y]==Cell::Energizer){
                prob *= 10.0;
            }
            else if(map[x][y]==Cell::Pellet){
                prob *= 3.0;
            }
            else{
                prob *= 0.5; // Castigo si la celda ya esta vacia
            }

            // 2. Evidencia de Inercia Direccional (D)
            // Pacman dir: 0=Derecha(x+), 1=Arriba(y-), 2=Izquierda(x-), 3=Abajo(y+)
            double dir_weight;
            dir_weight = 1.0;
            
            if(pacman_dir==0&&x>px){
                dir_weight = 2.0;
            }
            else if(pacman_dir==2&&x<px){
                dir_weight = 2.0;
            }
            else if(pacman_dir==1&&y<py){
                dir_weight = 2.0;
            }
            else if(pacman_dir==3&&y>py){
                dir_weight = 2.0;
            }
            else if(pacman_dir==0&&x<px){
                dir_weight = 0.1; // Fuerte penalizacion si esta a sus espaldas
            }
            else if(pacman_dir==2&&x>px){
                dir_weight = 0.1;
            }
            else if(pacman_dir==1&&y>py){
                dir_weight = 0.1;
            }
            else if(pacman_dir==3&&y<py){
                dir_weight = 0.1;
            }

            prob *= dir_weight;

            // 3. Decaimiento espacial (Mas cerca es meta mas inminente)
            prob /= (double)dist;

            // Maximizar la verosimilitud
            if(prob>max_prob){
                max_prob = prob;
                best_target = {short(x*CELL_SIZE), short(y*CELL_SIZE)};
            }
        }
    }

    return best_target;
}

#endif
