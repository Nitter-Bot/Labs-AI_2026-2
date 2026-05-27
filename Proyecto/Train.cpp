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

using namespace std;

int main(){
    int level = 0;
    int pop_size;
    pop_size = 50;
    int num_genes;
    num_genes = 5;
    int generaciones;
    generaciones = 100;

    GeneticAlgorithm ga(pop_size, num_genes);

    // Intentar leer la ultima generacion del CSV
    vector<double> best_weights;
    int start_gen;
    start_gen = 0;
    
    ifstream csv_read("best_genomes.csv");
    if(csv_read.is_open()){
        string line;
        string last_line;
        while(getline(csv_read, line)){
            if(line.length()>0){
                last_line = line;
            }
        }
        csv_read.close();

        if(last_line.length()>0&&last_line[0]!='G'){
            stringstream ss(last_line);
            string token;
            
            getline(ss, token, ',');
            start_gen = stoi(token)+1; 
            
            getline(ss, token, ','); // Saltar fitness
            
            while(getline(ss, token, ',')){
                best_weights.pb(stod(token));
            }
            
            cout<<"=====================================\n";
            cout<<"[INFO] Reanudando entrenamiento desde la Generacion "<<start_gen<<'\n';
            cout<<"=====================================\n";

            // Inyectar el ADN a los individuos base
            if(best_weights.size()==num_genes){
                for(int i=0;i<pop_size;++i){
                    ga.population[i].weights = best_weights;
                }
            }
        }
    }
    else{
        // Si no existe el archivo, lo creamos
        ofstream csv_init;
        csv_init.open("best_genomes.csv", ios::out | ios::trunc);
        csv_init<<"Generacion,Fitness,W1,W2,W3,W4\n";
        csv_init.close();
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

    for(int gen=start_gen;gen<start_gen+generaciones;++gen){
        for(int i=0;i<pop_size;++i){
            long long total_fitness;
            total_fitness = 0.0;

            for(int sim=0;sim<10;++sim){
                std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map;
                std::array<Position, 4> ghost_positions;
                Pacman pacman;

                map = convert_sketch(map_sketch, ghost_positions, pacman);
                pacman.set_dna(ga.population[i].weights);

                GhostManager ghost_manager;
                ghost_manager.reset(level, ghost_positions);

                int frames_vividos;
                frames_vividos = 0;
                bool game_over;
                game_over = false;

                while(!game_over&&frames_vividos<10000){
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
			    // Subir nivel y resetear el mapa
			    level++;
			    map = convert_sketch(map_sketch, ghost_positions, pacman);
			    ghost_manager.reset(level, ghost_positions);
			}

			// 2. Ejecutar IA y Físicas
			pacman.update_ai(level, map, ghost_manager.get_ghost_positions());
			ghost_manager.update(level, map, pacman);

			// 3. Checar muerte
			if (pacman.get_dead() == 1) {
			    game_over = true;
			    level = 0; // Reiniciar nivel al morir
			}
			frames_vividos++;
		}

                total_fitness += (pacman.get_score()*1000)+frames_vividos;
            }

            ga.population[i].fitness = total_fitness/50;
            cout<<"  -> Gen "<<gen<<" | Individuo "<<i<<" | Fitness: "<<ga.population[i].fitness<<'\n';
        }

        sort(ga.population.begin(), ga.population.end());

        ofstream csv_file;
        csv_file.open("best_genomes.csv", ios::app);
        csv_file<<gen<<","<<ga.population[0].fitness;
        for(int w=0;w<num_genes;++w){
            csv_file<<","<<ga.population[0].weights[w];
        }
        csv_file<<"\n";
        csv_file.close();

        cout<<">>> Generacion "<<gen<<" FINALIZADA | Mejor Fitness: "<<ga.population[0].fitness<<"\n\n";

        ga.evolve();
    }

    return 0;
}
