#include "State.hpp"
#include "Heuristics.hpp"
#include "Algorithms.hpp"
#include <iostream>
#include <string>

int main() {
    int choice_board, choice_input, choice_algo, choice_heur;
    int size = 3;

    std::cout << "Selecciona el tablero:\n1. 3x3\n2. 4x4\n3. 5x5\nOpcion: ";
    std::cin >> choice_board;
    if (choice_board == 2) size = 4;
    if (choice_board == 3) size = 5;

    std::cout << "Generacion:\n1. Aleatorio\n2. Ingresar secuencia\nOpcion: ";
    std::cin >> choice_input;

    State start_state(size);

    if (choice_input == 2) {
        std::vector<int> b(size * size);
        std::cout << "Ingresa los " << size * size << " numeros separados por espacio:\n";
        for (int i = 0; i < size * size; ++i) std::cin >> b[i];
        start_state = State(size, b);
        if (!start_state.isSolvable()) {
            std::cout << "El estado ingresado no tiene solucion.\n";
            return 0;
        }
    }

    std::cout << "Estado inicial:\n";
    start_state.print();

    std::cout << "Algoritmo:\n1. A*\n2. IDA*\nOpcion: ";
    std::cin >> choice_algo;

    std::cout << "Heuristica:\n1. Misplaced Tiles\n2. Manhattan Distance\n3. Linear Conflict\nOpcion: ";
    std::cin >> choice_heur;

    std::function<int(const State&)> heur;
    std::string h_name;
    if (choice_heur == 1) { heur = misplacedTiles; h_name = "Misplaced Tiles"; }
    else if (choice_heur == 2) { heur = manhattanDistance; h_name = "Manhattan Distance"; }
    else { heur = linearConflict; h_name = "Linear Conflict"; }

    std::cout << "Resolviendo con " << h_name << "...\n";

    if (choice_algo == 1) {
        AStarResult res = aStar(start_state, heur);
        if (res.success) {
            std::cout << "Camino encontrado. Longitud: " << res.path.size() - 1 << "\n";
            std::cout << "Nodos generados (OPEN + CLOSED aprox): " << res.nodes_opened << "\n";
            std::cout << "Nodos expandidos (CLOSED): " << res.nodes_closed << "\n";
            for (const auto& s : res.path) {
                s.print();
            }
        }
    } else {
        IDAStarResult res = idaStar(start_state, heur);
        if (res.success) {
            std::cout << "Camino encontrado. Longitud: " << res.path.size() - 1 << "\n";
            std::cout << "Nodos explorados en total: " << res.nodes_explored << "\n";
            std::cout << "Profundidad maxima alcanzada: " << res.max_depth << "\n";
            for (const auto& s : res.path) {
                s.print();
            }
        }
    }

    return 0;
}
