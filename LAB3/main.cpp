#include <iostream>
#include "APIClient.hpp"
#include "GameState.hpp"

int main(){
	APIClient client("localhost", 8080);
	GameState state;
	Perception current_perception;

	if(!client.start_game(current_perception))
		return 1;


	int current_x = 0;
	int current_y = 0;

	while(!current_perception.gameOver){
		state.update_perception(current_x, current_y, current_perception);

		if(current_perception.glitter){
			client.send_action("Grab", current_perception);
			client.send_action("TurnLeft", current_perception);
			client.send_action("TurnLeft", current_perception);
			continue;
		}

		std::string next_action = "Forward";
			
		client.send_action(next_action, current_perception);
	}

	std::cout << "\n--- RESULTADO FINAL ---" << std::endl;
	std::cout << "Mensaje: " << current_perception.message << std::endl;
	std::cout << "Puntaje Final: " << current_perception.score << std::endl;

	return 0;
}
