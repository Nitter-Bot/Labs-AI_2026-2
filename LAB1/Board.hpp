#ifndef BOARD_HPP
#define BOARD_HPP

#include <bits/stdc++.h>

class Board{
public:
	int grid[10][10];

	Board(){
		memset(grid,0,sizeof(grid));
	}

	bool canPlacePiece(int x, int y, int size, char orientation){
        	if(orientation == 'H' || orientation == 'h'){
        		if(y+size > 10)return 0;
			for(int i = 0; i < size; ++i){
                		if(grid[x][y + i] != 0)return 0;
			}
		}else{
        		if(x+size>10)return 0;
			for(int i=0;i<size;i++){
				if (grid[x+i][y]!= 0)return 0;
			}
        	}
        	return 1;
	}

	void placePiece(int x, int y, int size, char orientation){
        	if (orientation == 'H' || orientation == 'h'){
        		for(int i=0;i<size;i++)grid[x][y+i] = 1;
        	}else{
        		for(int i=0;i<size;i++)grid[x+i][y] = 1;
		}
	}

	void generateRandomBoard(){
        	vector<int> ships = {5, 4, 3, 3, 2};

		for (int size:ships){
        		bool placed = false;
        		while(!placed){
        			int x = rand() % 10;
        			int y = rand() % 10;
        			char o = (rand() % 2 == 0) ? 'H' : 'V';
        			if(canPlacePiece(x, y, size, o)) {
                			placePiece(x, y, size, o);
                			placed = 1;
                		}
			}
        	}
	}

	bool processShot(int x, int y){
		if (grid[y][x] == 1){
			grid[y][x] = -1;
        		return 1;
        	}else if (grid[y][x] == 0){
        		grid[y][x] = 2;
        		return false;
        	}
        	return false;
	}

	void print(bool hideShips){
        	std::cout << "  0 1 2 3 4 5 6 7 8 9\n";
        	for(int i=0;i<10;i++){
        		std::cout << i << " ";
        		for(int j=0;j<10;j++){
                		if (grid[i][j] == 0) std::cout << ". ";
                		else if (grid[i][j] == 1) std::cout << (hideShips ? ". " : "S ");
                		else if (grid[i][j] == -1) std::cout << "X ";
                		else if (grid[i][j] == 2) std::cout << "O ";
        		}
        		std::cout << "\n";
        	}
	}

	bool allSunk() {
        	for(int i = 0; i < 10; ++i)
        		for(int j = 0; j < 10; ++j)
                		if(grid[i][j] == 1) return false;
        	return 1;
	}
};

#endif
