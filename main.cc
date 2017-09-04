#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include <chrono>

#define N_PLAYERS 4

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
mt19937 randomGen(seed);
sf::Font font;
sf::Font scoreFont;

class GameLogic {
    int curPlayer;
    int state;

    public:
    GameLogic() {
        curPlayer = 0;
        state = 0;
    }

    int getCurPlayer() {
        return curPlayer;
    }

    void nextPlayer() {
        curPlayer++;
        if (curPlayer == N_PLAYERS) {
            curPlayer = 0;
        }
    }

    int getState() {
        return state;
    }

    void setState(int value) {
        state = value;
    }
};

class TextureManager {
    map<string, sf::Texture> textures;
    
    public:
    TextureManager(): textures(){}

    const sf::Texture& getTexture(const std::string& filename) {
        for(map<string, sf::Texture>::const_iterator it = textures.begin(); it != textures.end(); ++it){
            if( filename == it->first ){
                return it->second;
            }
        }

        sf::Texture image;
        if(image.loadFromFile( filename )){
            textures[filename] = image;
            return textures[filename];
        }

        textures[filename] = image;
        return textures[filename];
    }
};

class Square {
    int xPos, yPos, number;
    int leadsTo, gold;
    sf::RectangleShape square;
    sf::Text numberText, goldText;

    public:
    Square() {}
    Square(int x, int y, int n): xPos(x), yPos(y), number(n) {
        gold = 5;
        leadsTo = -1;

        square = sf::RectangleShape(sf::Vector2f(63, 63));
        square.setPosition(x, y);
        square.setFillColor(sf::Color(139,69,19));
        square.setOutlineThickness(1);
        square.setOutlineColor(sf::Color(0,0,0));

        numberText.setFont(font);
        numberText.setString(to_string(n));
        numberText.setCharacterSize(10);
        numberText.setColor(sf::Color(0,0,0));
        numberText.setPosition(x+2, y+2);

        goldText.setFont(font);
        goldText.setString(to_string(gold));
        goldText.setCharacterSize(18);
        goldText.setColor(sf::Color::Yellow);
        goldText.setPosition(x+25,y+19);
    }

    sf::RectangleShape getShape() {
        return square;
    }

    void drawSquare(sf::RenderWindow& window) {
        window.draw(square);
        window.draw(numberText);
        goldText.setString(to_string(gold));
        window.draw(goldText);
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

    int getGold() {
        return gold;
    }

    void setGold(int value) {
        gold = value;
    }
};

class Board {
    int size;
    vector<Square> square;
    sf::Sprite ladder;
    TextureManager textureManager;

    public:
    Board(int s, TextureManager tm): size(s), textureManager(tm) {
        square = vector<Square>(size);
        ladder.setTexture(textureManager.getTexture("sprites/ladder.png"));
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

    Square& getSquare(int i) {
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
                    ladder.setPosition(sf::Vector2f(square[i].getPosX() + 32, square[i].getPosY() + 32));
                    //ladder.setRotation(angle);
                    window.draw(ladder);
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
    int squarePos, gold, diamonds;
    sf::CircleShape token;
    sf::Sprite minecart;
    TextureManager textureManager;

    public:
    Player() {};
    Player(Board b, int p, TextureManager tm): textureManager(tm) {
        squarePos = 0;
        gold = 0;
        diamonds = 0;

        token = sf::CircleShape(16);
        minecart.setTexture(textureManager.getTexture("sprites/minecart"+to_string(p)+".png"));
        minecart.setScale(sf::Vector2f(.12f, .12f));

        switch (p) {
            case 1:
                xOffset = 0;
                yOffset = 0;
                token.setFillColor(sf::Color::Transparent);
                break;
            case 2:
                xOffset = 32;
                yOffset = 0;
                token.setFillColor(sf::Color::Transparent);
                break;
            case 3:
                xOffset = 0;
                yOffset = 32;
                token.setFillColor(sf::Color::Transparent);
                break;
            case 4:
                xOffset = 32;
                yOffset = 32;
                token.setFillColor(sf::Color::Transparent);
        }

        token.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
        minecart.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
    }

    sf::CircleShape getShape() {
        return token;
    }

    sf::Sprite getPlayerSprite() {
        return minecart;
    }

    void move(int value, Board& b, GameLogic& g, vector<Player>& ps) {
        int mult = 1;

        while (value > 0) {
            squarePos += mult;
            value--;

            if (squarePos == 99) {
                mult = -1;
            }
        }

        if (b.getSquare(squarePos).getGold() > 0) {
            b.getSquare(squarePos).setGold(b.getSquare(squarePos).getGold() - 1);
            gold++;
        }

        token.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
        minecart.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);

        if (squarePos == 99) {
            g.setState(3);
        }

        if (b.getSquare(squarePos).getLeadsTo() != -1) {
            if (b.getSquare(squarePos).getLeadsTo() < squarePos) {
                if (diamonds > 0) {
                    g.setState(1);
                }
                else {
                    for (int i = 0; i < N_PLAYERS; i++) {
                        if (ps[i].getPlayerSprite().getTexture() != minecart.getTexture()) {
                            if (ps[i].getSquarePos() == squarePos) {
                                if (ps[i].getGold() >= 5) {
                                    ps[i].setGold(ps[i].getGold() - 5);
                                    gold += 5;
                                }
                                else {
                                    gold += ps[i].getGold();
                                    ps[i].setGold(0);
                                }
                            }
                        }
                    }

                    squarePos = b.getSquare(squarePos).getLeadsTo();
                    token.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
                    minecart.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
                    g.nextPlayer();
                }
            }
            else {
                g.setState(2);
            }
        }
        else {
            for (int i = 0; i < N_PLAYERS; i++) {
                if (ps[i].getPlayerSprite().getTexture() != minecart.getTexture()) {
                    if (ps[i].getSquarePos() == squarePos) {
                        if (ps[i].getGold() >= 5) {
                            ps[i].setGold(ps[i].getGold() - 5);
                            gold += 5;
                        }
                        else {
                            gold += ps[i].getGold();
                            ps[i].setGold(0);
                        }
                    }
                }
            }

            g.nextPlayer();
        }
    }

    int getGold() {
        return gold;
    }

    int getDiamonds() {
        return diamonds;
    }

    int getSquarePos() {
        return squarePos;
    }

    void setGold(int value) {
        gold = value;
    }

    void setDiamonds(int value) {
        diamonds = value;
    }

    void setSquarePos(int value, Board b) {
        squarePos = value;
        token.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
        minecart.setPosition(b.getSquare(squarePos).getPosX() + xOffset, b.getSquare(squarePos).getPosY() + yOffset);
    }
};

class Dice {
    int value;
    sf::Text valueText, rollText;
    sf::RectangleShape button;
    uniform_int_distribution<int> distribution;

    public:
    Dice() {
        value = 6;
        valueText.setFont(scoreFont);
        valueText.setString(to_string(value));
        valueText.setCharacterSize(40);
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

    void checkDiceRoll(sf::RenderWindow& window, Player& player, Board& board, GameLogic& game, vector<Player>& vPlayers) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        if (mousePosition.x >= button.getPosition().x && mousePosition.x <= button.getPosition().x + button.getSize().x
        && mousePosition.y >= button.getPosition().y && mousePosition.y <= button.getPosition().y + button.getSize().y) {
            this->rollDice();
            player.move(value, board, game, vPlayers);
        }
    }

    int getValue() {
        return value;
    }
};

class LadderMenu {
    sf::Text acceptText, refuseText;
    sf::RectangleShape refuseButton, acceptButton;

    public:
    LadderMenu() {
        refuseButton = sf::RectangleShape(sf::Vector2f(125,39));
        refuseButton.setPosition(640 + 15, 530);
        refuseButton.setFillColor(sf::Color(255,255,255));

        acceptButton = sf::RectangleShape(sf::Vector2f(125,39));
        acceptButton.setPosition(640 + 15, 580);
        acceptButton.setFillColor(sf::Color(255,255,255));

        acceptText.setFont(font);
        acceptText.setCharacterSize(15);
        acceptText.setColor(sf::Color(0,0,0));
        acceptText.setPosition(640 + 25, 585);
        acceptText.setString("Get Diamond");

        refuseText.setFont(font);
        refuseText.setCharacterSize(15);
        refuseText.setColor(sf::Color(0,0,0));
        refuseText.setPosition(640 + 25, 535);
        refuseText.setString("Go Upstairs");
    }

    void drawMenu(sf::RenderWindow& window, GameLogic g) {
        if (g.getState() != 0 && g.getState() != 3) {
            if (g.getState() == 1) {
                acceptText.setString("Use Diamond");
                refuseText.setString("Go Downstairs");
            }
            else if (g.getState() == 2) {
                acceptText.setString("Get Diamond");
                refuseText.setString("Go Upstairs");
            }
            window.draw(refuseButton);
            window.draw(acceptButton);
            window.draw(acceptText);
            window.draw(refuseText);
        }
    }

    void checkAccept(sf::RenderWindow& window, GameLogic& game, Player& p, vector<Player>& ps) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        if (mousePosition.x >= acceptButton.getPosition().x && mousePosition.x <= acceptButton.getPosition().x + acceptButton.getSize().x
            && mousePosition.y >= acceptButton.getPosition().y && mousePosition.y <= acceptButton.getPosition().y + acceptButton.getSize().y) {
            if (game.getState() == 1) {
                p.setDiamonds(p.getDiamonds() - 1);
            }
            else if (game.getState() == 2) {
                p.setDiamonds(p.getDiamonds() + 1);
            }

            for (int i = 0; i < N_PLAYERS; i++) {
                if (ps[i].getPlayerSprite().getTexture() != p.getPlayerSprite().getTexture()) {
                    if (ps[i].getSquarePos() == p.getSquarePos()) {
                        if (ps[i].getGold() >= 5) {
                            ps[i].setGold(ps[i].getGold() - 5);
                            p.setGold(p.getGold() + 5);
                        }
                        else {
                            p.setGold(p.getGold() + ps[i].getGold());
                            ps[i].setGold(0);
                        }
                    }
                }
            }

            game.setState(0);
            game.nextPlayer();
        }
    }

    void accept(GameLogic& game, Player& p, vector<Player>& ps) {
        if (game.getState() == 1) {
            p.setDiamonds(p.getDiamonds() - 1);
        }
        else if (game.getState() == 2) {
            p.setDiamonds(p.getDiamonds() + 1);
        }

        for (int i = 0; i < N_PLAYERS; i++) {
            if (ps[i].getPlayerSprite().getTexture() != p.getPlayerSprite().getTexture()) {
                if (ps[i].getSquarePos() == p.getSquarePos()) {
                    if (ps[i].getGold() >= 5) {
                        ps[i].setGold(ps[i].getGold() - 5);
                        p.setGold(p.getGold() + 5);
                    }
                    else {
                        p.setGold(p.getGold() + ps[i].getGold());
                        ps[i].setGold(0);
                    }
                }
            }
        }

        game.setState(0);
        game.nextPlayer();
    }

    void checkRefuse(sf::RenderWindow& window, GameLogic& game, Player& p, Board& b, vector<Player>& ps) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        if (mousePosition.x >= refuseButton.getPosition().x && mousePosition.x <= refuseButton.getPosition().x + refuseButton.getSize().x
            && mousePosition.y >= refuseButton.getPosition().y && mousePosition.y <= refuseButton.getPosition().y + refuseButton.getSize().y) {
            p.setSquarePos(b.getSquare(p.getSquarePos()).getLeadsTo(), b);

            for (int i = 0; i < N_PLAYERS; i++) {
                if (ps[i].getPlayerSprite().getTexture() != p.getPlayerSprite().getTexture()) {
                    if (ps[i].getSquarePos() == p.getSquarePos()) {
                        if (ps[i].getGold() >= 5) {
                            ps[i].setGold(ps[i].getGold() - 5);
                            p.setGold(p.getGold() + 5);
                        }
                        else {
                            p.setGold(p.getGold() + ps[i].getGold());
                            ps[i].setGold(0);
                        }
                    }
                }
            }

            game.setState(0);
            game.nextPlayer();
        }
    }

    void refuse(GameLogic& game, Player& p, Board& b, vector<Player>& ps) {
        p.setSquarePos(b.getSquare(p.getSquarePos()).getLeadsTo(), b);

        for (int i = 0; i < N_PLAYERS; i++) {
            if (ps[i].getPlayerSprite().getTexture() != p.getPlayerSprite().getTexture()) {
                if (ps[i].getSquarePos() == p.getSquarePos()) {
                    if (ps[i].getGold() >= 5) {
                        ps[i].setGold(ps[i].getGold() - 5);
                        p.setGold(p.getGold() + 5);
                    }
                    else {
                        p.setGold(p.getGold() + ps[i].getGold());
                        ps[i].setGold(0);
                    }
                }
            }
        }

        game.setState(0);
        game.nextPlayer();
    }
};

class Leaderboard {
    vector<sf::CircleShape> players;
    vector<sf::Sprite> playerSprites;
    vector<sf::Text> goldText, diamondText;
    sf::Text winText;
    TextureManager textureManager;
    sf::Sprite diamond;
    sf::Sprite gold;

    public:
    Leaderboard(int n, vector<Player> p, TextureManager tm): players(vector<sf::CircleShape>(n)), playerSprites(vector<sf::Sprite>(n)), goldText(vector<sf::Text>(n)), diamondText(vector<sf::Text>(n)), textureManager(tm) {
        int x = 655;
        int y = 180;

        diamond.setTexture(textureManager.getTexture("sprites/diamond.png"));
        gold.setTexture(textureManager.getTexture("sprites/gold.png"));
        winText.setFont(font);
        winText.setCharacterSize(50);
        winText.setPosition(120, 120);

        for (int i = 0; i < n; i++) {
            players[i] = p[i].getShape();
            playerSprites[i] = p[i].getPlayerSprite();
            players[i].setPosition(x, y + 50 * i);
            playerSprites[i].setPosition(x, y + 50 * i);

            goldText[i].setFont(scoreFont);
            goldText[i].setString("0");
            goldText[i].setCharacterSize(20);
            goldText[i].setColor(sf::Color(255,245,105));
            goldText[i].setPosition(x + 50, y + 50 * i);

            diamondText[i].setFont(scoreFont);
            diamondText[i].setString("0");
            diamondText[i].setCharacterSize(20);
            diamondText[i].setColor(sf::Color(182,232,250));
            diamondText[i].setPosition(x + 90, y + 50 * i);
        }
        diamond.setPosition(x+90, y+200);
        gold.setPosition(x+50, y+200);
    }

    void drawLeaderboard(sf::RenderWindow& window, vector<Player> p, GameLogic g) {
        for (int i = 0; i < N_PLAYERS; i++) {
            players[i].setOutlineThickness(0);
            if (g.getCurPlayer() == i) {
                players[i].setOutlineThickness(2);
            }
            window.draw(players[i]);
            window.draw(playerSprites[i]);

            goldText[i].setString(to_string(p[i].getGold()));
            window.draw(goldText[i]);

            diamondText[i].setString(to_string(p[i].getDiamonds()));
            window.draw(diamondText[i]);
        }
        window.draw(diamond);
        window.draw(gold);
    }

    void win(vector<Player> p, sf::RenderWindow& window) {
        vector<int> scores(N_PLAYERS);

        for (int i = 0; i < N_PLAYERS; i++) {
            scores[i] = p[i].getGold() + p[i].getDiamonds() * 20;
            if (p[i].getSquarePos() == 99) {
                scores[i] += 50;
            }
        }

        int winner = -1;
        int highestScore = 0;
        for (int i = 0; i < N_PLAYERS; i++) {
            if (scores[i] > highestScore) {
                highestScore = scores[i];
                winner = i;
            }
        }

        winText.setColor(sf::Color::Blue);
        winText.setString("Player " + to_string(winner + 1) + " wins!");
        window.draw(winText);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800,640), "Snakes and Ladders!");
    font.loadFromFile("Verdana.ttf");
    scoreFont.loadFromFile("FFFFORWA.TTF");

    TextureManager tm;

    GameLogic game;
    Board board(100, tm);
    vector<Player> players(N_PLAYERS);
    Dice dice;

    board.initBoard();
    for (int i = 0; i < N_PLAYERS; i++) {
        players[i] = Player(board, i+1, tm);
    }

    Leaderboard leaderboard(N_PLAYERS, players, tm);
    LadderMenu menu;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (game.getState() == 0) {
                        dice.checkDiceRoll(window, players[game.getCurPlayer()], board, game, players);
                    }
                    else if (game.getState() != 3) {
                        menu.checkAccept(window, game, players[game.getCurPlayer()], players);
                        menu.checkRefuse(window, game, players[game.getCurPlayer()], board, players);
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (game.getState() == 0) {
                    if (event.key.code == sf::Keyboard::Space) {
                        dice.rollDice();
                        players[game.getCurPlayer()].move(dice.getValue(), board, game, players);
                    }
                }
                else if (game.getState() != 3) {
                    if (event.key.code == sf::Keyboard::Z) {
                        menu.accept(game, players[game.getCurPlayer()], players);
                    }
                    if (event.key.code == sf::Keyboard::X) {
                        menu.refuse(game, players[game.getCurPlayer()], board, players);
                    }
                }
            }
        }

        window.clear();

        for (int i = 0; i < 100; i++) {
            board.getSquare(i).drawSquare(window);
        }
        board.drawStairsAndSnakes(window);
        for (int i = 0; i < N_PLAYERS; i++) {
            window.draw(players[i].getShape());
            window.draw(players[i].getPlayerSprite());
        }
        dice.drawDice(window);
        leaderboard.drawLeaderboard(window, players, game);
        menu.drawMenu(window, game);

        if (game.getState() == 3) {
            leaderboard.win(players, window);
        }

        window.display();
    }

    return 0;
}
