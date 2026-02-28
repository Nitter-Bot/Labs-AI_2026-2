#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Agent.hpp"

const int CELL_SIZE = 40;
const int OFFSET = 100;

enum GameState {
    MENU,
    PLACING,
    PLAYING,
    GAME_OVER
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Battleship");

    GameState state = MENU;

    Player human;
    Agent* ai = nullptr;

    int selectedDifficulty = 0;
    bool manualPlacement = false;
    bool humanTurn = true;

    std::vector<int> ships = {5,4,3,3,2};
    int currentShipIndex = 0;
    char currentOrientation = 'H';

    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    //BOTONES MENU 
    sf::RectangleShape diff1(sf::Vector2f(300, 60));
    sf::RectangleShape diff2(sf::Vector2f(300, 60));
    sf::RectangleShape diff3(sf::Vector2f(300, 60));
    sf::RectangleShape placementBtn(sf::Vector2f(300, 60));
    sf::RectangleShape startBtn(sf::Vector2f(300, 60));

    diff1.setPosition(350, 150);
    diff2.setPosition(350, 230);
    diff3.setPosition(350, 310);
    placementBtn.setPosition(350, 390);
    startBtn.setPosition(350, 470);

    sf::RectangleShape restartBtn(sf::Vector2f(300,60));
    restartBtn.setPosition(350, 600);
    restartBtn.setFillColor(sf::Color(200,100,0));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // = MENU =
            if (state == MENU && event.type == sf::Event::MouseButtonPressed)
            {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                if (diff1.getGlobalBounds().contains(mx,my)) selectedDifficulty = 1;
                if (diff2.getGlobalBounds().contains(mx,my)) selectedDifficulty = 2;
                if (diff3.getGlobalBounds().contains(mx,my)) selectedDifficulty = 3;

                if (placementBtn.getGlobalBounds().contains(mx,my))
                    manualPlacement = !manualPlacement;

                if (startBtn.getGlobalBounds().contains(mx,my) && selectedDifficulty != 0)
                {
                    if (ai) delete ai;

                    if (selectedDifficulty == 1) ai = new ReflexAgent();
                    if (selectedDifficulty == 2) ai = new GoalBasedAgent();
                    if (selectedDifficulty == 3) ai = new GoalBasedAgentInsane();

                    human = Player();
                    ai->setupBoardRandom();

                    currentShipIndex = 0;
                    currentOrientation = 'H';

                    if (manualPlacement)
                        state = PLACING;
                    else {
                        human.setupBoardRandom();
                        state = PLAYING;
                    }

                    humanTurn = true;
                }
            }

          
            if (state == PLACING)
            {
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::R)
                        currentOrientation = (currentOrientation == 'H') ? 'V' : 'H';
                }

                if (event.type == sf::Event::MouseButtonPressed)
                {
                    int mx = event.mouseButton.x;
                    int my = event.mouseButton.y;

                    int gridX = (mx - 50) / CELL_SIZE;
                    int gridY = (my - OFFSET) / CELL_SIZE;

                    if (gridX >= 0 && gridX < 10 && gridY >= 0 && gridY < 10)
                    {
                        int size = ships[currentShipIndex];

                        if (human.ownBoard.canPlacePiece(gridX, gridY, size, currentOrientation))
                        {
                            human.ownBoard.placePiece(gridX, gridY, size, currentOrientation);
                            currentShipIndex++;

                            if (currentShipIndex >= ships.size())
                                state = PLAYING;
                        }
                    }
                }
            }

            // = GAME =
            if (state == PLAYING && humanTurn &&
                event.type == sf::Event::MouseButtonPressed)
            {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                int gridX = (mx - 550) / CELL_SIZE;
                int gridY = (my - OFFSET) / CELL_SIZE;

                if (gridX >= 0 && gridX < 10 && gridY >= 0 && gridY < 10)
                {
                    if (human.trackingBoard.grid[gridY][gridX] == 0)
                    {
                        bool hit = ai->ownBoard.processShot(gridX, gridY);
                        if (hit) human.trackingBoard.grid[gridY][gridX] = -1;
                        else human.trackingBoard.grid[gridY][gridX] = 2;

                        humanTurn = false;
                    }
                }
            }

            // = RESTART =
            if (state == GAME_OVER &&
                event.type == sf::Event::MouseButtonPressed)
            {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                if (restartBtn.getGlobalBounds().contains(mx,my))
                {
                    if (ai) delete ai;
                    ai = nullptr;
                    selectedDifficulty = 0;
                    state = MENU;
                }
            }
        }

        if (state == PLAYING && !humanTurn)
        {
            ai->fireShot(human);
            humanTurn = true;
        }

        if (state == PLAYING && (human.hasLost() || ai->hasLost()))
            state = GAME_OVER;

        window.clear(sf::Color(30,30,30));

        // = MENU DIBUJO =
        if (state == MENU)
        {
            diff1.setFillColor(selectedDifficulty == 1 ? sf::Color::Green : sf::Color(100,100,255));
            diff2.setFillColor(selectedDifficulty == 2 ? sf::Color::Green : sf::Color(100,100,255));
            diff3.setFillColor(selectedDifficulty == 3 ? sf::Color::Green : sf::Color(100,100,255));
            placementBtn.setFillColor(sf::Color(150,150,150));
            startBtn.setFillColor(sf::Color(0,200,100));

            window.draw(diff1);
            window.draw(diff2);
            window.draw(diff3);
            window.draw(placementBtn);
            window.draw(startBtn);

            sf::Text t1("Facil", font, 20);
            sf::Text t2("Medio", font, 20);
            sf::Text t3("Dificil", font, 20);
            sf::Text t4(manualPlacement ? "Colocacion: Manual" : "Colocacion: Aleatoria", font, 20);
            sf::Text t5("INICIAR JUEGO", font, 22);

            t1.setPosition(420,160);
            t2.setPosition(420,240);
            t3.setPosition(420,320);
            t4.setPosition(380,405);
            t5.setPosition(390,485);

            window.draw(t1);
            window.draw(t2);
            window.draw(t3);
            window.draw(t4);
            window.draw(t5);
        }

        // = PLACING DIBUJO =
        if (state == PLACING)
        {
            for(int y=0;y<10;y++)
            {
                for(int x=0;x<10;x++)
                {

                    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE-2,CELL_SIZE-2));
                    cell.setPosition(x*CELL_SIZE+50, y*CELL_SIZE+OFFSET);

                    int v = human.ownBoard.grid[y][x];
                    if(v==0) cell.setFillColor(sf::Color(100,100,100));
                    if(v==1) cell.setFillColor(sf::Color::Blue);

                    window.draw(cell);
                }
            }

            sf::Text placingText(
                "Coloca barco tamano: " + std::to_string(ships[currentShipIndex]) +
                "   Presiona R para rotar", font, 20);

            placingText.setPosition(200,50);
            window.draw(placingText);
        }

        // = GAME DIBUJO =
        if (state == PLAYING || state == GAME_OVER)
        {
            for(int y=0;y<10;y++)
            {
                for(int x=0;x<10;x++)
                {

                    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE-2,CELL_SIZE-2));
                    cell.setPosition(x*CELL_SIZE+50, y*CELL_SIZE+OFFSET);

                    int v = human.ownBoard.grid[y][x];
                    if(v==0) cell.setFillColor(sf::Color(100,100,100));
                    if(v==1) cell.setFillColor(sf::Color::Blue);
                    if(v==-1) cell.setFillColor(sf::Color::Red);
                    if(v==2) cell.setFillColor(sf::Color::White);

                    window.draw(cell);
                }
            }

            for(int y=0;y<10;y++)
            {
                for(int x=0;x<10;x++)
                {

                    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE-2,CELL_SIZE-2));
                    cell.setPosition(x*CELL_SIZE+550, y*CELL_SIZE+OFFSET);

                    int v = human.trackingBoard.grid[y][x];
                    if(v==0) cell.setFillColor(sf::Color(100,100,100));
                    if(v==-1) cell.setFillColor(sf::Color::Red);
                    if(v==2) cell.setFillColor(sf::Color::White);

                    window.draw(cell);
                }
            }

            if(state == GAME_OVER)
            {
                sf::Text result(human.hasLost() ? "PERDISTE" : "GANASTE", font, 40);
                result.setPosition(400,50);
                window.draw(result);

                window.draw(restartBtn);

                sf::Text restartText("REINICIAR", font, 22);
                restartText.setPosition(410,615);
                window.draw(restartText);
            }
        }

        window.display();
    }

    if(ai) delete ai;
    return 0;
}
