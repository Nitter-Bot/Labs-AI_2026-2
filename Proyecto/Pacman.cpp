#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/MapCollision.hpp"

Pacman::Pacman() :
	animation_over(0),
	dead(0),
	direction(0),
	energizer_timer(0),
	position({0, 0})
{
	//I just realized that I already explained everything in the Ghost class.
	//And I don't like to repeat myself.
	score = 0;
}

bool Pacman::get_animation_over()
{
	return animation_over;
}

bool Pacman::get_dead()
{
	return dead;
}

unsigned char Pacman::get_direction()
{
	return direction;
}

unsigned short Pacman::get_energizer_timer()
{
	return energizer_timer;
}

long long Pacman::get_score()
{
    return score;
}

void Pacman::draw(bool i_victory, sf::RenderWindow& i_window)
{
	unsigned char frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(PACMAN_ANIMATION_SPEED)));

	sf::Sprite sprite;

	sf::Texture texture;

	sprite.setPosition(position.x, position.y);

	if (1 == dead || 1 == i_victory)
	{
		if (animation_timer < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED)
		{
			animation_timer++;

			texture.loadFromFile("Resources/Images/PacmanDeath" + std::to_string(CELL_SIZE) + ".png");

			sprite.setTexture(texture);
			sprite.setTextureRect(sf::IntRect(CELL_SIZE * frame, 0, CELL_SIZE, CELL_SIZE));

			i_window.draw(sprite);
		}
		else
		{
			//You can only die once.
			animation_over = 1;
		}
	}
	else
	{
		texture.loadFromFile("Resources/Images/Pacman" + std::to_string(CELL_SIZE) + ".png");

		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(CELL_SIZE * frame, CELL_SIZE * direction, CELL_SIZE, CELL_SIZE));

		i_window.draw(sprite);

		animation_timer = (1 + animation_timer) % (PACMAN_ANIMATION_FRAMES * PACMAN_ANIMATION_SPEED);
	}
}

void Pacman::reset()
{
	animation_over = 0;
	dead = 0;

	direction = 0;

	animation_timer = 0;
	energizer_timer = 0;
}

void Pacman::set_animation_timer(unsigned short i_animation_timer)
{
	animation_timer = i_animation_timer;
}

void Pacman::set_dead(bool i_dead)
{
	dead = i_dead;

	if (1 == dead)
	{
		//Making sure that the animation starts from the beginning.
		animation_timer = 0;
	}
}

void Pacman::set_position(short i_x, short i_y)
{
	position = {i_x, i_y};
}

void Pacman::set_dna(std::vector<double> w)
{
    dna = w;
}

void Pacman::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
	unsigned short pellets_antes = 0;
	unsigned short energizers_antes = 0;
	    for (int i = 0; i < MAP_WIDTH; ++i) {
		for (int j = 0; j < MAP_HEIGHT; ++j) {
		    if (i_map[i][j] == Cell::Pellet) pellets_antes++;
		    else if (i_map[i][j] == Cell::Energizer) energizers_antes++;
		}
	    }

	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, PACMAN_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - PACMAN_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - PACMAN_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, PACMAN_SPEED + position.y, i_map);

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (0 == walls[0]) //You can't turn in this direction if there's a wall there.
		{
			direction = 0;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (0 == walls[1])
		{
			direction = 1;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (0 == walls[2])
		{
			direction = 2;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (0 == walls[3])
		{
			direction = 3;
		}
	}

	if (0 == walls[direction])
	{
		switch (direction)
		{
			case 0:
			{
				position.x += PACMAN_SPEED;

				break;
			}
			case 1:
			{
				position.y -= PACMAN_SPEED;

				break;
			}
			case 2:
			{
				position.x -= PACMAN_SPEED;

				break;
			}
			case 3:
			{
				position.y += PACMAN_SPEED;
			}
		}
	}

	if (-CELL_SIZE >= position.x)
	{
		position.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
	}
	else if (CELL_SIZE * MAP_WIDTH <= position.x)
	{
		position.x = PACMAN_SPEED - CELL_SIZE;
	}

	if (1 == map_collision(1, 0, position.x, position.y, i_map)) //When Pacman eats an energizer...
	{
		//He becomes energized!
		energizer_timer = static_cast<unsigned short>(ENERGIZER_DURATION / pow(2, i_level));
	}
	else
	{
		energizer_timer = std::max(0, energizer_timer - 1);
	}

	unsigned short pellets_despues = 0;
	    unsigned short energizers_despues = 0;
	    for (int i = 0; i < MAP_WIDTH; ++i) {
		for (int j = 0; j < MAP_HEIGHT; ++j) {
		    if (i_map[i][j] == Cell::Pellet) pellets_despues++;
		    else if (i_map[i][j] == Cell::Energizer) energizers_despues++;
		}
	    }

	    // 3. Calculamos la diferencia matemática e imprimimos el puntaje
	    if (pellets_despues < pellets_antes) {
		score += 10ll * (pellets_antes - pellets_despues);
		//std::cout << "Score: " << score << '\n';
	    }
	    
	    if (energizers_despues < energizers_antes) {
		score += 50ll * (energizers_antes - energizers_despues);
		//std::cout << "Score: " << score << " (Energizer!)\n";
	    }
	
}

Position Pacman::get_position()
{
	return position;
}

void Pacman::update_ai(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, const std::array<Position, 4>& i_ghost_positions)
{
{
	unsigned short pellets_antes = 0;
	unsigned short energizers_antes = 0;
	    for (int i = 0; i < MAP_WIDTH; ++i) {
		for (int j = 0; j < MAP_HEIGHT; ++j) {
		    if (i_map[i][j] == Cell::Pellet) pellets_antes++;
		    else if (i_map[i][j] == Cell::Energizer) energizers_antes++;
		}
	    }
    std::array<bool, 4> walls{};
    walls[0] = map_collision(0, 0, PACMAN_SPEED+position.x, position.y, i_map);
    walls[1] = map_collision(0, 0, position.x, position.y-PACMAN_SPEED, i_map);
    walls[2] = map_collision(0, 0, position.x-PACMAN_SPEED, position.y, i_map);
    walls[3] = map_collision(0, 0, position.x, PACMAN_SPEED+position.y, i_map);

    if(position.x%CELL_SIZE==0&&position.y%CELL_SIZE==0){
        unsigned char ai_dir;
        ai_dir = calculate_ai_move(i_map, i_ghost_positions);
        if(0==walls[ai_dir]){
            direction = ai_dir;
        }
    }

    if(0==walls[direction]){
        switch(direction)
        {
            case 0: position.x += PACMAN_SPEED; break;
            case 1: position.y -= PACMAN_SPEED; break;
            case 2: position.x -= PACMAN_SPEED; break;
            case 3: position.y += PACMAN_SPEED; break;
        }
    }

    if(-CELL_SIZE>=position.x){
        position.x = CELL_SIZE*MAP_WIDTH-PACMAN_SPEED;
    }
    else if(position.x>=CELL_SIZE*MAP_WIDTH){
        position.x = PACMAN_SPEED-CELL_SIZE;
    }
	
	if (1 == map_collision(1, 0, position.x, position.y, i_map)) //When Pacman eats an energizer...
	{
		//He becomes energized!
		energizer_timer = static_cast<unsigned short>(ENERGIZER_DURATION / pow(2, i_level));
	}
	else
	{
		energizer_timer = std::max(0, energizer_timer - 1);
	}
	
	unsigned short pellets_despues = 0;
	    unsigned short energizers_despues = 0;
	    for (int i = 0; i < MAP_WIDTH; ++i) {
		for (int j = 0; j < MAP_HEIGHT; ++j) {
		    if (i_map[i][j] == Cell::Pellet) pellets_despues++;
		    else if (i_map[i][j] == Cell::Energizer) energizers_despues++;
		}
	    }

	    // 3. Calculamos la diferencia matemática e imprimimos el puntaje
	    if (pellets_despues < pellets_antes) {
		score += 10ll * (pellets_antes - pellets_despues);
		//std::cout << "Score: " << score << '\n';
	    }else if (pellets_despues == pellets_antes)score = std::max(0ll,score-3);
	    
	    if (energizers_despues < energizers_antes){
		score += 50ll * (energizers_antes - energizers_despues);
		//std::cout << "Score: " << score << " (Energizer!)\n";
	    }
	}

}

unsigned char Pacman::calculate_ai_move(const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& map, const std::array<Position, 4>& i_ghost_positions)
{
    if(dna.empty()){
        return direction;
    }

    int px;
    px = position.x/CELL_SIZE;
    int py;
    py = position.y/CELL_SIZE;
    
    double best_score;
    best_score = -1e9;
    unsigned char best_dir;
    best_dir = direction;
    
    int dx[] = {1, 0, -1, 0}; 
    int dy[] = {0, -1, 0, 1};

    for(int i=0;i<4;++i){
        int nx;
        nx = px+dx[i];
        int ny;
        ny = py+dy[i];

        if(nx>=0&&nx<MAP_WIDTH&&ny>=0&&ny<MAP_HEIGHT){
            if(map[nx][ny]!=Cell::Wall){
                double score_move;
                score_move = 0.0;
                
                if(map[nx][ny]==Cell::Pellet){
                    score_move += dna[0];
                }
                if(map[nx][ny]==Cell::Energizer){
                    score_move += dna[1];
                }

                int min_p;
                min_p = 1000;
                int min_e;
                min_e = 1000;

                for(int cx=0;cx<MAP_WIDTH;++cx){
                    for(int cy=0;cy<MAP_HEIGHT;++cy){
                        if(map[cx][cy]==Cell::Pellet){
                            int dist;
                            dist = abs(nx-cx)+abs(ny-cy);
                            if(dist<min_p){
                                min_p = dist;
                            }
                        }
                        if(map[cx][cy]==Cell::Energizer){
                            int dist;
                            dist = abs(nx-cx)+abs(ny-cy);
                            if(dist<min_e){
                                min_e = dist;
                            }
                        }
                    }
                }

                // ==========================================================
                // GEN 5: EVALUACION DE FANTASMAS
                // ==========================================================
                int min_g;
                min_g = 1000;
                for(int k=0;k<4;++k){
                    int gx;
                    gx = i_ghost_positions[k].x/CELL_SIZE;
                    int gy;
                    gy = i_ghost_positions[k].y/CELL_SIZE;
                    int dist_g;
                    dist_g = abs(nx-gx)+abs(ny-gy);
                    if(dist_g<min_g){
                        min_g = dist_g;
                    }
                }

                if(min_p>0&&min_p<1000){
                    score_move += dna[2]*(1.0/min_p);
                }
                if(min_e>0&&min_e<1000){
                    score_move += dna[3]*(1.0/min_e);
                }
                
                if(min_g>0&&min_g<1000){
			if(energizer_timer == 0 && min_g < 3)
				score_move -= 50.0; 
                    // 2. LÓGICA DE ADN (Pesos para optimización)
			if(energizer_timer > 0){
				// MODO CAZA: DNA positivo es bueno
				score_move += dna[4] * (2.0 / (min_g + 1));
			}else{
				// MODO HUÍDA: DNA positivo penaliza acercamiento
				score_move -= dna[4] * (2.0 / (min_g + 1));
			}
                }

                if(i==(direction+2)%4){
                    score_move -= 20.0; 
                }

                if(score_move>best_score){
                    best_score = score_move;
                    best_dir = i;
                }
            }
        }
    }
    
    return best_dir;
}
