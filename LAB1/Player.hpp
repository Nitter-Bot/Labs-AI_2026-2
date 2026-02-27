#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Board.hpp"
#include <bits/stdc++.h>

class Player{
public:
        Board ownBoard;
        Board trackingBoard;

        void setupBoardManual(){
                int ships[] = {5, 4, 3, 3, 2};
                for (int size : ships){
                        bool placed = false;
                        while(!placed){
                                ownBoard.print(false);
                                int x, y;
                                char o;
                                std::cout << "Barco de tamano " << size << ". Ingresa x, y, orientacion (H/V): ";
                                std::cin >> x >> y >> o;
                                if (x>=0 && x<10 && y>=0 && y<10 && ownBoard.canPlacePiece(x, y, size, o)){
                                        ownBoard.placePiece(x, y, size, o);
                                        placed = true;
                                }else{
                                        std::cout << "Posicion invalida o se superpone. Intenta de nuevo.\n";
                                }
                        }
                }
        }

        void setupBoardRandom(){
                ownBoard.generateRandomBoard();
        }

        virtual std::pair<int, int> getShot(){
                int x, y;
                while(true){
                        std::cout << "Ingresa coordenada de disparo (x y): ";
                        std::cin >> x >> y;
                        if (x>=0 && x<10 && y>=0 && y<10 && trackingBoard.grid[y][x]==0){
                                return {x, y};
                        }
                        std::cout << "Tiro invalido o ya disparaste ahi.\n";
                }
        }

        void fireShot(Player& enemy) {
                std::pair<int, int> shot = getShot();
                int x = shot.first;
                int y = shot.second;

                bool hit = enemy.ownBoard.processShot(x, y);
                if (hit) {
                        trackingBoard.grid[y][x] = -1;
                        std::cout << "¡Hit en (" << x << ", " << y << ")!\n";
                } else {
                        trackingBoard.grid[y][x] = 2;
                        std::cout << "Fallo en (" << x << ", " << y << ").\n";
                }
        }

        bool hasLost() {
                return ownBoard.allSunk();
        }
};

#endif
