#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include <chrono>

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
mt19937 randomGen(seed);
sf::Font font;

class GameLogic {
    int curPlayer;

    public:
    GameLogic() {
        curPlayer = 0;
    }

    int getCurPlayer() {
        return curPlayer;
    }

    void nextPlayer() {
        curPlayer++;
        if (curPlayer == 4) {
            curPlayer = 0;
        }
    }
};

class Square {
    int xPos, yPos, number;
    int leadsTo;
    sf::RectangleShape square;
    sf::Text numberText;

    public:
    Square() {}
    Square(int x, int y, int n): xPos(x), yPos(y), number(n) {
        leadsTo = -1;

        square = sf::RectangleShape(sf::Vector2f(63, 63));
        square.setPosition(x, y);
        square.setFillColor(sf::Color(255,255,255));
        square.setOutlineThickness(1);
        square.setOutlineColor(sf::Color(0,0,0));

        numberText.setFont(font);
        numberText.setString(to_string(n));
        numberText.setCharacterSize(10);
        numberText.setColor(sf::Color(0,0,0));
        numberText.setPosition(x+2, y+2);
    }

    sf::RectangleShape getShape() {
        return square;
    }

    void drawSquare(sf::RenderWindow& window) {
        window.draw(square);
        window.draw(numberText);
    }

    void setLeadsTo(int d) {
        leadsTo = d;
    }

    int getLeadsTo() {
        return leadsTo;
    }

    int getPosX() {
        return xPos;
    }

    int getPosY() {
        return yPos;
    }
};

class Board {
    int size;
    vector<Square> square;

    public:
    Board(int s): size(s) {
        square = vector<Square>(size);
    }

    void initBoard() {
        int initX = 0;
        int initY = 0;
        int Xmult = 1;

        for (int i = size; i > 0; i--) {
            square[i-1] = Square(initX, initY, i);
            initX += 64 * Xmult;
            if (initX == 640 || initX == -64) {
                Xmult *= -1;
                initX += 64 * Xmult;
                initY += 64;
            }
        }

        this->initStairs();
        this->initSnakes();
    }

    void initStairs() {
        square[1].setLeadsTo(37);
        square[3].setLeadsTo(13);
        square[8].setLeadsTo(30);
        square[20].setLeadsTo(41);
        square[27].setLeadsTo(83);
        square[35].setLeadsTo(43);
        square[50].setLeadsTo(66);
        square[70].setLeadsTo(90);
        square[79].setLeadsTo(98);
    }

    void initSnakes() {
        square[15].setLeadsTo(5);
        square[46].setLeadsTo(25);
        square[48].setLeadsTo(10);
        square[55].setLeadsTo(52);
        square[61].setLeadsTo(18);
        square[63].setLeadsTo(59);
        square[86].setLeadsTo(23);
        square[92].setLeadsTo(72);
        square[94].setLeadsTo(74);
        square[97].setLeadsTo(77);
    }

    Square getSquare(int i) {
        return square[i];
    }

    void drawStairsAndSnakes(sf::RenderWindow& window) {
        for (int i = 0; i < 100; i++) {
            if (square[i].getLeadsTo() != -1) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(square[i].getPosX() + 32, square[i].getPosY() + 32)),
                    sf::Vertex(sf::Vector2f(square[square[i].getLeadsTo()].getPosX() + 32, square[square[i].getLeadsTo()].getPosY() + 32))
                };

                if (square[i].getLeadsTo() > i) {
                    line[0].color = sf::Color::Blue;
                    line[1].color = sf::Color::Blue;
                }
                else {
                    line[0].color = sf::Color::Red;
                    line[1].color = sf::Color::Red;
                }

                window.draw(line, 2, sf::Lines);
            }
        }
    }
};

class Player {
    int xOffset, yOffset;
    int squarePos;
    sf::CircleShape token;

    public:
    Player() {};
    Player(Board b, int p) {
        squarePos = 0;

        token = sf::CircleShape(16);

        switch (p) {
            case 1:
                xOffset = 0;
                yOffset = 0;
                token.setFillColor(sf::Color::Red);
                break;
            case 2:
                xOffset = 32;
                yOffset = 0;
                token.setFillColor(sf::Color::Green);
                break;
            case 3:
                xOffset = 0;
                yOffset = 32;
                token.setFillColor(sf::Color::Blue);
                break;
            case 4:
                xOffset = 32;
                yOffset = 32;
                token.setFillColor(sf::Color(255,0,255));
        }

        token.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
    }

    sf::CircleShape getShape() {
        return token;
    }

    void move(int value, Board b) {
        int mult = 1;

        while (value > 0) {
            squarePos += mult;
            value--;

            if (squarePos == 99) {
                mult = -1;
            }
        }

        if (b.getSquare(squarePos).getLeadsTo() != -1) {
            squarePos = b.getSquare(squarePos).getLeadsTo();
        }

        token.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);

        /*if (squarePos == 99) {
            sf::Text youWin;
            sf::Font winFont;

            winFont.loadFromFile("Verdana.ttf");
            youWin.setFont(winFont);
            youWin.setString("You Win!");
            youWin.setCharacterSize(50);
            youWin.setColor(token.getFillColor());
        }*/
    }
};

class Dice {
    int value;
    sf::Text valueText, rollText;
    sf::RectangleShape button;
    std::uniform_int_distribution<int> distribution;

    public:
    Dice() {
        value = 6;
        valueText.setFont(font);
        valueText.setString(to_string(value));
        valueText.setCharacterSize(50);
        valueText.setColor(sf::Color(255,255,255));
        valueText.setPosition(640 + 80 - 20, 50);

        rollText.setFont(font);
        rollText.setString("Roll Dice");
        rollText.setCharacterSize(24);
        rollText.setColor(sf::Color(0,0,0));
        rollText.setPosition(640 + 25, 120);

        button = sf::RectangleShape(sf::Vector2f(125,39));
        button.setPosition(640 + 15, 115);
        button.setFillColor(sf::Color(255,255,255));

        distribution = uniform_int_distribution<int>(1,6);
    }

    void drawDice(sf::RenderWindow& window) {
        window.draw(valueText);
        window.draw(button);
        window.draw(rollText);
    }

    void rollDice() {
        value = distribution(randomGen);
        valueText.setString(to_string(value));
    }

    void checkDiceRoll(sf::RenderWindow& window, Player& player, Board& board, GameLogic& game) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        if (mousePosition.x >= button.getPosition().x && mousePosition.x <= button.getPosition().x + button.getSize().x
        && mousePosition.y >= button.getPosition().y && mousePosition.y <= button.getPosition().y + button.getSize().y) {
            this->rollDice();
        }

        player.move(value, board);
        game.nextPlayer();
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800,640), "Snakes and Ladders!");
    font.loadFromFile("Verdana.ttf");
    
    GameLogic game;
    Board board(100);
    vector<Player> players(4);
    Dice dice;

    board.initBoard();
    for (int i = 0; i < 4; i++) {
        players[i] = Player(board, i+1);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    dice.checkDiceRoll(window, players[game.getCurPlayer()], board, game);
                }
            }
        }

        window.clear();

        for (int i = 0; i < 100; i++) {
            board.getSquare(i).drawSquare(window);
        }
        board.drawStairsAndSnakes(window);
        for (int i = 0; i < 4; i++) {
            window.draw(players[i].getShape());
        }
        dice.drawDice(window);

        window.display();
    }

    return 0;
}