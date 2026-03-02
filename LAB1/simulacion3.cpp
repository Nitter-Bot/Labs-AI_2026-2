#include <iostream>
#include <chrono>
#include <random>
#include "Player.hpp"
#include "Agent.hpp"

int main(){
    long long winsRandom = 0;
    long long winsDiagonals = 0;

    long long shotsWinRandom = 0;
    long long shotsLossRandom = 0;
    
    long long shotsWinDiagonals = 0;
    long long shotsLossDiagonals = 0;

    int totalGames = 1e5;
    int games = totalGames;
    
    while(games--){
        ReflexAgent agenteRandom;
        GoalBasedAgentInsane agenteGoalBased;

        agenteRandom.setupBoardRandom();
        agenteGoalBased.setupBoardRandom();

        int tirosRandom = 0;
        int tirosGoalBased = 0;

        while(!agenteRandom.hasLost() && !agenteGoalBased.hasLost()){
            agenteRandom.fireShot(agenteGoalBased);
            tirosRandom++;
            
            if(agenteGoalBased.hasLost()){
                break;
            }

            agenteGoalBased.fireShot(agenteRandom);
            tirosGoalBased++;
        }

        if(agenteGoalBased.hasLost()){
            winsRandom++;
            shotsWinRandom += tirosRandom;
            shotsLossDiagonals += tirosGoalBased;
        }else{
            winsDiagonals++;
            shotsWinDiagonals += tirosGoalBased;
            shotsLossRandom += tirosRandom;
        }
    }

    double avgWinRandom = winsRandom > 0 ? (double)shotsWinRandom / winsRandom : 0;
    double avgLossRandom = (totalGames - winsRandom) > 0 ? (double)shotsLossRandom / (totalGames - winsRandom) : 0;

    double avgWinDiagonals = winsDiagonals > 0 ? (double)shotsWinDiagonals / winsDiagonals : 0;
    double avgLossDiagonals = (totalGames - winsDiagonals) > 0 ? (double)shotsLossDiagonals / (totalGames - winsDiagonals) : 0;

    std::ofstream file("resultados.csv", std::ios::app);
    if(file.tellp() == 0){
        file << "Agente,Ganadas,PromedioTirosVictoria,PromedioTirosDerrota\n";
    }
    
    file << "Aleatorio," << winsRandom << "," << avgWinRandom << "," << avgLossRandom << "\n";
    file << "GoalBasedInsane," << winsDiagonals << "," << avgWinDiagonals << "," << avgLossDiagonals << "\n";
    file.close();

    std::cout << "Simulacion terminada. Resultados agregados a resultados.csv\n";
    return 0;
}
