#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "Algorithms.hpp"
#include "Heuristics.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>
#include <functional>

const int WINDOW_SIZE = 600;

enum AppState {
    MENU_BOARD,
    MENU_HEURISTIC,
    SOLVING
};

struct Button {
    sf::RectangleShape rect;
    sf::Text text;

    Button(sf::Vector2f pos, sf::Vector2f size, std::string str, const sf::Font& font) 
        : text(font) 
    {
        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(sf::Color(200, 200, 200));
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2.f);

        text.setString(str);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(pos.x + 10.f, pos.y + 10.f));
    }

    bool isClicked(sf::Vector2i mousePos) {
        return rect.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect);
        window.draw(text);
    }
};

void drawBoard(sf::RenderWindow& window, const State& state, const sf::Font& font) {
    int tileSize = WINDOW_SIZE / state.size;

    for (int i = 0; i < state.n_tiles; ++i) {
        int val = state.board[i];

        sf::RectangleShape tile(sf::Vector2f(static_cast<float>(tileSize - 5), static_cast<float>(tileSize - 5)));
        tile.setPosition(sf::Vector2f(static_cast<float>((i % state.size) * tileSize), static_cast<float>((i / state.size) * tileSize)));

        if (val == 0) {
            tile.setFillColor(sf::Color::Black);
        } else {
            tile.setFillColor(sf::Color(100, 200, 250));
        }

        window.draw(tile);

        if (val != 0) {
            sf::Text text(font);
            text.setString(std::to_string(val));
            text.setCharacterSize(30);
            text.setFillColor(sf::Color::Black);

            text.setPosition(sf::Vector2f(
                static_cast<float>((i % state.size) * tileSize + tileSize / 3),
                static_cast<float>((i / state.size) * tileSize + tileSize / 4)
            ));

            window.draw(text);
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_SIZE, WINDOW_SIZE)), "Puzzle Solver");

    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf")) {
        return -1;
    }

    AppState currentState = MENU_BOARD;
    int size = 3;
    int algo_type = 1;
    std::function<int(const State&)> heuristic;

    Button btn3x3(sf::Vector2f(200.f, 150.f), sf::Vector2f(200.f, 50.f), "Tablero 3x3", font);
    Button btn4x4(sf::Vector2f(200.f, 250.f), sf::Vector2f(200.f, 50.f), "Tablero 4x4", font);
    Button btn5x5(sf::Vector2f(200.f, 350.f), sf::Vector2f(200.f, 50.f), "Tablero 5x5", font);

    Button btnMisplaced(sf::Vector2f(150.f, 200.f), sf::Vector2f(300.f, 50.f), "Misplaced Tiles", font);
    Button btnManhattan(sf::Vector2f(150.f, 300.f), sf::Vector2f(300.f, 50.f), "Manhattan Distance", font);

    State startState;
    std::vector<State> solution;
    std::atomic<bool> solved(false);
    std::unique_ptr<std::thread> solverThread;

    auto startSolving = [&]() {
        startState = State(size);
        solverThread = std::make_unique<std::thread>([&]() {
            if (algo_type == 1) {
                AStarResult res = aStar(startState, heuristic);
                if (res.success) {
                    solution = res.path;
                    solved = true;
                }
            } else if (algo_type == 2) {
                IDAStarResult res = idaStar(startState, heuristic);
                if (res.success) {
                    solution = res.path;
                    solved = true;
                }
            } else if (algo_type == 3) {
                IDAStarResult res = wIdaStar(startState, heuristic, 2.0);
                if (res.success) {
                    solution = res.path;
                    solved = true;
                }
            }
        });
    };

    int step = 0;
    sf::Clock clock;

    while (window.isOpen()) {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->getIf<sf::Event::Closed>()) {
                window.close();
            }

            if (event->getIf<sf::Event::KeyPressed>()) {
                if (currentState == SOLVING && solved && step == (int)solution.size() - 1) {
                    if (solverThread && solverThread->joinable()) {
                        solverThread->join();
                    }
                    currentState = MENU_BOARD;
                    solved = false;
                    step = 0;
                    solution.clear();
                }
            }

            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = mouseButton->position;

                    if (currentState == MENU_BOARD) {
                        if (btn3x3.isClicked(mousePos)) {
                            size = 3;
                            currentState = MENU_HEURISTIC;
                        } else if (btn4x4.isClicked(mousePos)) {
                            size = 4;
                            algo_type = 2;
                            heuristic = linearConflict;
                            currentState = SOLVING;
                            startSolving();
                        } else if (btn5x5.isClicked(mousePos)) {
                            size = 5;
                            algo_type = 3; 
                            heuristic = linearConflict;
                            currentState = SOLVING;
                            startSolving();
                        }
                    } else if (currentState == MENU_HEURISTIC) {
                        if (btnMisplaced.isClicked(mousePos)) {
                            heuristic = misplacedTiles;
                            algo_type = 1;
                            currentState = SOLVING;
                            startSolving();
                        } else if (btnManhattan.isClicked(mousePos)) {
                            heuristic = manhattanDistance;
                            algo_type = 1;
                            currentState = SOLVING;
                            startSolving();
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(240, 240, 240));

        if (currentState == MENU_BOARD) {
            btn3x3.draw(window);
            btn4x4.draw(window);
            btn5x5.draw(window);
        } else if (currentState == MENU_HEURISTIC) {
            btnMisplaced.draw(window);
            btnManhattan.draw(window);
        } else if (currentState == SOLVING) {
            if (solved) {
                if (clock.getElapsedTime().asMilliseconds() > 500 && step < (int)solution.size() - 1) {
                    step++;
                    clock.restart();
                }
                drawBoard(window, solution[step], font);

                if (step == (int)solution.size() - 1) {
                    sf::Text returnText(font);
                    returnText.setString("Resuelto! Presiona cualquier tecla para volver");
                    returnText.setCharacterSize(20);
                    returnText.setFillColor(sf::Color::Red);
                    returnText.setPosition(sf::Vector2f(10.f, WINDOW_SIZE - 30.f));
                    window.draw(returnText);
                }
            } else {
                drawBoard(window, startState, font);
                
                sf::Text calcText(font);
                calcText.setString("Calculando solucion...");
                calcText.setCharacterSize(20);
                calcText.setFillColor(sf::Color::Red);
                calcText.setPosition(sf::Vector2f(10.f, WINDOW_SIZE - 30.f));
                window.draw(calcText);
            }
        }

        window.display();
    }

    if (solverThread && solverThread->joinable()) {
        solverThread->join();
    }

    return 0;
}
