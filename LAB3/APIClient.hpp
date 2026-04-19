#ifndef API_CLIENT_HPP
#define API_CLIENT_HPP

#include <string>
#include <iostream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "GameState.hpp"

using json = nlohmann::json;

class APIClient{
private:
	httplib::Client cli;
	std::string gameId;

public:
	APIClient(const std::string& host,int port):cli(host, port){}

	bool start_game(Perception& initial_perception){
		auto res = cli.Post("/game/new");
		if (res && res->status == 201) {
			auto data = json::parse(res->body);
			gameId = data["gameId"];
			initial_perception = {
				data["perception"]["stench"],
				data["perception"]["breeze"],
				data["perception"]["glitter"],
				data["perception"]["bump"],
				data["perception"]["scream"],
				data["perception"]["score"],
				data["perception"]["gameOver"],
				data["perception"]["message"]
			};
			return true;
		}
		return false;
	}

	bool send_action(const std::string& action,Perception& new_perception){
		std::string path = "/game/" + gameId + "/action";
		json req_body = {{"action", action}};

		auto res = cli.Post(path,req_body.dump(),"application/json");
		if(res&&res->status==200){
			auto data = json::parse(res->body);
			new_perception={
				data["stench"], data["breeze"], data["glitter"],
				data["bump"], data["scream"], data["score"],
				data["gameOver"], data["message"]
			};
			return true;
		}
		return false;
	}
};

#endif
