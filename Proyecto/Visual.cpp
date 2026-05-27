#include <array>
#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Headers/Global.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/Ghost.hpp"
#include "Headers/GhostManager.hpp"
#include "Headers/ConvertSketch.hpp"
#include "Headers/DrawMap.hpp"
#include "Headers/MapCollision.hpp"
#include "Headers/Genetic.hpp"

#define pb push_back

// Constante para controlar la velocidad (lag) de la visualizacion
// 60 = Velocidad normal | 30 = Mitad de velocidad | 120 = Camara rapida
#define VISUAL_FPS 60

using namespace std;

int main(){
	int level = 0;
    vector<double> best_weights;
    ifstream csv_file("best_genomes.csv");
    
    if(csv_file.is_open()){
        string line;
        string last_line;
        while(getline(csv_file, line)){
            if(line.length()>0){
                last_line = line;
            }
        }
        csv_file.close();

        if(last_line.length()>0&&last_line[0]!='G'){
            stringstream ss(last_line);
            string token;
            getline(ss, token, ',');
            getline(ss, token, ',');
            while(getline(ss, token, ',')){
                best_weights.pb(stod(token));
            }
        }
    }

    std::array<std::string, MAP_HEIGHT> map_sketch = {
        " ################### ",
        " #........#........# ",
        " #o##.###.#.###.##o# ",
        " #.................# ",
        " #.##.#.#####.#.##.# ",
        " #....#...#...#....# ",
        " ####.### # ###.#### ",
        "    #.#   0   #.#    ",
        "#####.# ##=## #.#####",
        "     .  #123#  .     ",
        "#####.# ##### #.#####",
        "    #.#       #.#    ",
        " ####.# ##### #.#### ",
        " #........#........# ",
        " #.##.###.#.###.##.# ",
        " #o.#.....P.....#.o# ",
        " ##.#.#.#####.#.#.## ",
        " #....#...#...#....# ",
        " #.######.#.######.# ",
        " #.................# ",
        " ################### "
    };

    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map;
    std::array<Position, 4> ghost_positions;
    Pacman pacman;

    map = convert_sketch(map_sketch, ghost_positions, pacman);
    
    if(best_weights.size()>0){
        pacman.set_dna(best_weights);
        cout<<"ADN cargado exitosamente. Visualizando mejor agente...\n";
    }else{
        cout<<"No se encontro el archivo CSV o esta vacio. Se usaran pesos por defecto.\n";
    }

    GhostManager ghost_manager;
    ghost_manager.reset(0, ghost_positions);

    sf::RenderWindow window(sf::VideoMode(CELL_SIZE*MAP_WIDTH*SCREEN_RESIZE, (FONT_HEIGHT+CELL_SIZE*MAP_HEIGHT)*SCREEN_RESIZE), "Pac-Man AI Visualizer", sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE*MAP_WIDTH, FONT_HEIGHT+CELL_SIZE*MAP_HEIGHT)));
    
    // Aplicamos el "lag" inyectando el limite de FPS directamente a la tarjeta grafica
    window.setFramerateLimit(VISUAL_FPS);

    sf::Event event;

    while(window.isOpen()){
        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed){
                window.close();
            }
            if(event.type==sf::Event::KeyPressed&&pacman.get_dead()){
                window.close();
            }
        }

if (!pacman.get_dead()) {
    // Verificar si se ganó el nivel
    bool game_won = true;
    for (const auto& column : map) {
        for (const auto& cell : column) {
            if (Cell::Pellet == cell) {
                game_won = false;
                break;
            }
        }
        if (!game_won) break;
    }

    if (game_won) {
        // Lógica de victoria visual
        pacman.set_animation_timer(0);
        level++;
        map = convert_sketch(map_sketch, ghost_positions, pacman);
        ghost_manager.reset(level, ghost_positions);
    } else {
        // Juego normal
        pacman.update_ai(level, map, ghost_manager.get_ghost_positions());
        ghost_manager.update(level, map, pacman);
    }
} else {
    // Si muere, resetear nivel a 0
    level = 0;
}

        window.clear(sf::Color(0, 0, 0));
        
        draw_map(map, window);
        ghost_manager.draw(0, window);
        pacman.draw(0, window);
        
        window.display();
    }

    return 0;
}


