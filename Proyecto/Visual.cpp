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
#define VISUAL_FPS 50

using namespace std;

int main(){
    int level;
    level = 0;
    
    // 0 = Input Generacion | 1 = Simulacion | 2 = Game Over
    int app_state;
    app_state = 0; 
    
    string input_gen;
    input_gen = "";

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
    GhostManager ghost_manager;

    sf::RenderWindow window(sf::VideoMode(CELL_SIZE*MAP_WIDTH*SCREEN_RESIZE, (FONT_HEIGHT+CELL_SIZE*MAP_HEIGHT)*SCREEN_RESIZE), "Pac-Man AI Visualizer", sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE*MAP_WIDTH, FONT_HEIGHT+CELL_SIZE*MAP_HEIGHT)));
    
    window.setFramerateLimit(VISUAL_FPS);

    sf::Event event;

    while(window.isOpen()){
        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed){
                window.close();
            }
            
            if(app_state==0){
                // Capturamos el texto del teclado
                if(event.type==sf::Event::TextEntered){
                    // Aceptamos numeros del 0 al 9 y el signo menos (-)
                    if(event.text.unicode==45||(event.text.unicode>=48&&event.text.unicode<=57)){
                        input_gen += static_cast<char>(event.text.unicode);
                    }
                    // Borramos con Backspace
                    else if(event.text.unicode==8&&input_gen.length()>0){
                        input_gen.pop_back();
                    }
                    // Iniciamos simulacion con Enter
                    else if(event.text.unicode==13){
                        int target_gen;
                        if(input_gen.length()==0){
                            target_gen = -1;
                        }
                        else{
                            target_gen = stoi(input_gen);
                        }
                        
                        vector<double> best_weights;
                        ifstream csv_file("best_genomes.csv");
                        
                        if(csv_file.is_open()){
                            string line;
                            string selected_line;
                            while(getline(csv_file, line)){
                                if(line.length()>0&&line[0]!='G'){
                                    if(target_gen==-1){
                                        selected_line = line;
                                    }
                                    else{
                                        stringstream temp_ss(line);
                                        string gen_token;
                                        getline(temp_ss, gen_token, ',');
                                        if(stoi(gen_token)==target_gen){
                                            selected_line = line;
                                            break;
                                        }
                                    }
                                }
                            }
                            csv_file.close();

                            if(selected_line.length()>0){
                                stringstream ss(selected_line);
                                string token;
                                getline(ss, token, ',');
                                getline(ss, token, ',');
                                while(getline(ss, token, ',')){
                                    best_weights.pb(stod(token));
                                }
                            }
                        }

                        // Reseteamos y preparamos el entorno de juego
                        pacman.set_dead(0); 
                        level = 0;
                        map = convert_sketch(map_sketch, ghost_positions, pacman);
                        
                        if(best_weights.size()>0){
                            pacman.set_dna(best_weights);
                        }
                        
                        ghost_manager.reset(0, ghost_positions);
                        
                        // Pasamos al estado de simulacion
                        app_state = 1;
                    }
                }
            }
            else if(app_state==2){
                // Si ya murio y presionamos una tecla (que no sea enter accidental del input anterior), reseteamos
                if(event.type==sf::Event::KeyPressed){
                    app_state = 0;
                    input_gen = "";
                }
            }
        }

        window.clear(sf::Color(0, 0, 0));
        
        if(app_state==0){
            // Pantalla de Input Grafica usando la libreria del juego
            draw_text(0, CELL_SIZE, CELL_SIZE*8, "GENERACION (-1 = ULTIMA):", window);
            draw_text(0, CELL_SIZE, CELL_SIZE*10, input_gen, window);
            draw_text(0, CELL_SIZE, CELL_SIZE*15, "PRESIONA ENTER PARA INICIAR", window);
        }
        else if(app_state==1||app_state==2){
            if(app_state==1){
                if(!pacman.get_dead()){
                    bool game_won;
                    game_won = true;
                    for(const auto& column:map){
                        for(const auto& cell:column){
                            if(Cell::Pellet==cell){
                                game_won = false;
                                break;
                            }
                        }
                        if(!game_won){
                            break;
                        }
                    }

                    if(game_won){
                        pacman.set_animation_timer(0);
                        level++;
                        map = convert_sketch(map_sketch, ghost_positions, pacman);
                        ghost_manager.reset(level, ghost_positions);
                    }
                    else{
                        pacman.update_ai(level, map, ghost_manager.get_ghost_positions());
                        ghost_manager.update(level, map, pacman);
                    }
                }
                else{
                    // Si pacman murio, pasamos al estado de Game Over
                    app_state = 2;
                }
            }

            draw_map(map, window);
            ghost_manager.draw(0, window);
            pacman.draw(0, window);
            
            if(app_state==2){
                // Pantalla superpuesta cuando muere
                draw_text(0, CELL_SIZE*4, CELL_SIZE*10, "SIMULACION TERMINADA", window);
                draw_text(0, CELL_SIZE*2, CELL_SIZE*12, "PRESIONA UNA TECLA PARA SALIR", window);
            }
        }
        
        window.display();
    }

    return 0;
}
