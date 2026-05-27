#ifndef GENETIC_HPP
#define GENETIC_HPP

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

#define pb push_back

using namespace std;

struct Genome{
    vector<double> weights;
    double fitness;
    
    // Sobrecarga para ordenar de mayor a menor fitness
    bool operator<(const Genome& other) const{
        return fitness>other.fitness;
    }
};

class GeneticAlgorithm{
    private:
        int pop_size;
        int num_weights;
        double mutation_rate;
        mt19937 rng;

    public:
        vector<Genome> population;

        GeneticAlgorithm(int p_size, int n_weights): pop_size(p_size), num_weights(n_weights){
            random_device rd;
            rng = mt19937(rd());
            mutation_rate = 0.15; // 15% de probabilidad de mutar
            
            uniform_real_distribution<double> dist(-10.0, 10.0);
            
            // Inicializar poblacion aleatoria
            for(int i=0;i<pop_size;++i){
                Genome g;
                g.fitness = 0;
                for(int j=0;j<num_weights;++j){
                    g.weights.pb(dist(rng));
                }
                population.pb(g);
            }
        }

        void evolve(){
            // 1. Ordenar por fitness (los mejores al inicio)
            sort(population.begin(), population.end());
            
            vector<Genome> new_population;
            
            // 2. Elitismo: Los 3 mejores pasan directo a la siguiente generacion para no perder el progreso
            for(int i=0;i<3;++i){
                new_population.pb(population[i]);
            }
            
            // 3. Crossover: Mezclar a los 3 mejores para rellenar el resto de la poblacion
            uniform_int_distribution<int> parent_dist(0, 2); // Elegir entre el top 0, 1 y 2
            uniform_real_distribution<double> mut_dist(-2.0, 2.0); // Rango de mutacion
            uniform_real_distribution<double> prob_dist(0.0, 1.0);
            
            for(int i=3;i<pop_size;++i){
                Genome child;
                child.fitness = 0;
                
                int p1;
                p1 = parent_dist(rng);
                int p2;
                p2 = parent_dist(rng);
                
                // Cruce gen por gen (pedazos de ADN aleatorios)
                for(int j=0;j<num_weights;++j){
                    if(prob_dist(rng)>0.5){
                        child.weights.pb(population[p1].weights[j]);
                    }else{
                        child.weights.pb(population[p2].weights[j]);
                    }
                    
                    // 4. Mutacion
                    if(prob_dist(rng)<mutation_rate){
                        child.weights[j] += mut_dist(rng); // Alteracion estocastica
                    }
                }
                new_population.pb(child);
            }
            
            population = new_population;
        }
};

#endif
