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
    long long fitness;
    
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
        Genome best_ever; 
        bool has_best_ever = false;

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
            new_population.pb(population[0]);
           uniform_int_distribution<int> tournament_dist(0, pop_size - 1);
            normal_distribution<double> gauss_dist(0.0, 1.0); // Mutación Gaussiana
            uniform_real_distribution<double> prob_dist(0.0, 1.0);
            
            // Rellenar el resto de la poblacion mediante torneos
            while(new_population.size() < pop_size){
                // Torneo: Elegimos 3 al azar, el mejor de los 3 gana el derecho a procrear
                vector<int> candidates = {tournament_dist(rng), tournament_dist(rng), tournament_dist(rng)};
                int winner = candidates[0];
                for(int idx : candidates){
                    if(population[idx].fitness > population[winner].fitness) winner = idx;
                }
                
                // Hacemos lo mismo para el segundo padre
                int winner2 = candidates[0];
                for(int idx : candidates){
                    if(population[idx].fitness > population[winner2].fitness && idx != winner) winner2 = idx;
                }
                
                // Crossover
                Genome child;
                for(int j=0; j<num_weights; ++j){
                    // Cruce promediado o aleatorio
                    double val = (prob_dist(rng) > 0.5) ? population[winner].weights[j] : population[winner2].weights[j];
                    
                    // Mutacion Gaussiana (Ajuste fino en lugar de saltos bruscos)
                    if(prob_dist(rng) < mutation_rate){
                        val += gauss_dist(rng) * 0.5; // Ajuste fino
                    }
                    child.weights.pb(val);
                }
                child.fitness = 0;
                new_population.pb(child);
            }
            population = new_population;
        }
};

#endif
