#pragma once

class Pacman
{
	//This is used for the death animation.
	bool animation_over;
	//Am I dead?
	bool dead;

	unsigned char direction;
	long long score;
	//More timers!
	unsigned short animation_timer;
	unsigned short energizer_timer;

	//Current location of this creature, commonly known as Pacman.
	Position position;
	std::vector<double> dna;
	
	unsigned char calculate_ai_move(const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& map, const std::array<Position, 4>& i_ghost_positions);
public:
	Pacman();

	bool get_animation_over();
	bool get_dead();
	long long get_score();
	unsigned char get_direction();

	unsigned short get_energizer_timer();

	void draw(bool i_victory, sf::RenderWindow& i_window);
	void reset();
	void set_animation_timer(unsigned short i_animation_timer);
	void set_dead(bool i_dead);
	void set_position(short i_x, short i_y);
	void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map);
	void set_dna(std::vector<double> w);
	void update_ai(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, const std::array<Position, 4>& i_ghost_positions);
	Position get_position();
};
