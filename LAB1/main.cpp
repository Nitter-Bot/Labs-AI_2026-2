#include<bits/stdc++.h>
#include "Player.hpp"
#include "Agent.hpp"

using namespace std;

int main() {
    Player human;
    Agent* ai = nullptr;

    int opc;
    std::cout << "Elige contra que agente jugaras:\n";
    std::cout << "1. Agente de Reflejos\n";
    std::cout << "2. Agente Goal-Based\n";
    std::cout << "3. Agente Insano\n";
    std::cout << "Opcion: ";
    std::cin >> opc;

    if (opc == 1) 
    {
        ai = new ReflexAgent();
    }
    else if (opc == 2) 
    {
        ai = new GoalBasedAgent();
    }
    else if (opc == 3) 
    {
        ai = new GoalBasedAgentInsane();
    }
    else 
    {
        std::cout << "Opcion invalida, se usara Agente de Reflejos.\n";
        ai = new ReflexAgent();
    }

    std::cout << "\nComo quieres colocar tus barcos?\n";
    std::cout << "1. Manual\n";
    std::cout << "2. Aleatorio\n";
    std::cout << "Opcion: ";
    std::cin >> opc;

    if (opc == 1) human.setupBoardManual();
    else human.setupBoardRandom();

    ai->setupBoardRandom();

    bool humanTurn = true;

    while (!human.hasLost() && !ai->hasLost()) {
        if (humanTurn) {
            std::cout << "\n==============================\n";
            std::cout << "        TU TURNO\n";
            std::cout << "==============================\n";
            std::cout << "\n--- TU TABLERO ---\n";
            human.ownBoard.print(false);
            
            std::cout << "\n--- TABLERO DE RASTREO (ENEMIGO) ---\n";
            human.trackingBoard.print(true);
            
            human.fireShot(*ai);
        } else {
            std::cout << "\n==============================\n";
            std::cout << "      TURNO DEL AGENTE\n";
            std::cout << "==============================\n";
            ai->fireShot(human);
        }
        humanTurn = !humanTurn;
    }

    if (human.hasLost()) {
        std::cout << "\nEl Agente ha ganado. ¡Mas suerte para la proxima!\n";
    } else {
        std::cout << "\n¡Ganaste! Destruiste toda la flota enemiga.\n";
    }

    delete ai;
    return 0;
}
