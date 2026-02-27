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
    std::cout << "Opcion: ";
    std::cin >> opc;

    if (opc == 1) {
        ai = new ReflexAgent();
    } else {
        ai = new GoalBasedAgent();
    }

    std::cout << "\nComo quieres colocar tus barcos?\n";
    std::cout << "1. Manual\n";
    std::cout << "2. Aleatorio\n";
    std::cout << "Opcion: ";
    std::cin >> opc;

    if (opc == 1) human.setupBoardManual();
    else human.setupBoardRandom();

    // El agente siempre genera el suyo aleatoriamente
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
