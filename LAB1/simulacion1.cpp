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
        GoalBasedAgent agenteRandom;
        GoalBasedAgentInsane agenteDiagonales;

        agenteRandom.setupBoardRandom();
        agenteDiagonales.setupBoardRandom();

        int tirosRandom = 0;
        int tirosDiagonales = 0;

        while(!agenteRandom.hasLost() && !agenteDiagonales.hasLost()){
            agenteRandom.fireShot(agenteDiagonales);
            tirosRandom++;
            
            if(agenteDiagonales.hasLost()){
                break;
            }

            agenteDiagonales.fireShot(agenteRandom);
            tirosDiagonales++;
        }

        if(agenteDiagonales.hasLost()){
            winsRandom++;
            shotsWinRandom += tirosRandom;
            shotsLossDiagonals += tirosDiagonales;
        }else{
            winsDiagonals++;
            shotsWinDiagonals += tirosDiagonales;
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
    file << "Diagonales," << winsDiagonals << "," << avgWinDiagonals << "," << avgLossDiagonals << "\n";
    file.close();

    std::cout << "Simulacion terminada. Resultados agregados a resultados.csv\n";
    return 0;
}
