//#include <stdio.h>
//#include <stdlib.h>
//#include <conio.h>
//Unfortunately, conio.h is only supported on windows
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <sstream>


#define BOARD_WIDTH     (8)
#define BOARD_HEIGHT    (8)
const int CELL_SIZE = 100; // Change the cell value to 80 if the white border below is not enough to fit 2 rows
const int BORDER_HEIGHT = 100;
const int WINDOW_SIZE = BOARD_HEIGHT * CELL_SIZE;
const int TOTAL_WINDOW_HEIGHT = WINDOW_SIZE + BORDER_HEIGHT;


enum DiskColor{
    TURN_BLACK,
    TURN_WHITE,
    TURN_NONE,
    TURN_MAX
};

struct Cell {
    DiskColor disk = DiskColor::TURN_NONE;
};

class Othello {
private:
    sf::RenderWindow window;
    Cell board[BOARD_HEIGHT][BOARD_WIDTH];
    DiskColor currentPlayer;
    sf::Font font;
    sf::Text statusText;
    std::mt19937 randomizer; // Mersenne Twister random engine
    std::uniform_int_distribution<int> randomDistribution; // Uniform distribution for random numbers
    bool invMove;
    bool pass;
    bool gameover;
    int blackC;
    int whiteC;


public:
    Othello() : window(sf::VideoMode(WINDOW_SIZE, TOTAL_WINDOW_HEIGHT), "Othello Game") {
        Init();
        InitText();
        initializeRandom();
        chooseStarter();
    }
    ~Othello() = default;

    void Init() {
        for (int y = 0; y< BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                board[y][x].disk = TURN_NONE;
            }
        }

        board[4][3].disk = board[3][4].disk = TURN_BLACK;
        board[3][3].disk = board[4][4].disk = TURN_WHITE;

        invMove = false;
        pass = false;
        gameover = false;
        whiteC = 0;
        blackC = 0;
    }

    // Initialize the font and text objects
    void InitText() {
        if (!font.loadFromFile("misaki_gothic_2nd.ttf")) { // Use a font file (e.g. "arial.ttf")
            std::cerr << "Failed to load font." << std::endl;
            // Error handling (e.g. exit the program)
            std::exit(EXIT_FAILURE);
        }
        // Initialize the text object
        statusText.setFont(font);
        statusText.setCharacterSize(40); // Set character size
        statusText.setFillColor(sf::Color::Black); // Set text color
        statusText.setPosition(10, WINDOW_SIZE + 2); // Set position on the white border
    }

    // Initialize random engine and distribution
    void initializeRandom() {
        std::random_device rdm; // Random device as a source of entropy
        randomizer = std::mt19937(rdm()); // Mersenne Twister random engine
        randomDistribution = std::uniform_int_distribution<int>(0, 1); // Distribution range from 0 to 1
    }

    void chooseStarter() {
        int randomValue = randomDistribution(randomizer);
        currentPlayer = (randomValue == 0) ? DiskColor::TURN_BLACK : DiskColor::TURN_WHITE;
    }

    void drawScreen() {
        window.clear(sf::Color::Green);

        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                cellShape.setFillColor(sf::Color::Green);
                cellShape.setOutlineColor(sf::Color::Black);
                cellShape.setOutlineThickness(1.5);
                window.draw(cellShape);

                if (board[y][x].disk != DiskColor::TURN_NONE) {
                    sf::CircleShape diskShape(CELL_SIZE / 2.0f - 5);
                    diskShape.setPosition(x * CELL_SIZE + 5, y * CELL_SIZE + 5);
                    if (board[y][x].disk == DiskColor::TURN_BLACK) {
                        diskShape.setFillColor(sf::Color::Black);
                    } else {
                        diskShape.setFillColor(sf::Color::White);
                    }
                    window.draw(diskShape);
                }
            }
        }

        sf::RectangleShape whiteBorder(sf::Vector2f(WINDOW_SIZE, BORDER_HEIGHT));
        whiteBorder.setFillColor(sf::Color::White);
        whiteBorder.setPosition(0, WINDOW_SIZE); // Position the border at the bottom of the window
        window.draw(whiteBorder);

        // Update the status text based on the current game state
        updateStatusText();

        // Draw the status text on the white border
        window.draw(statusText);
    }

    void updateStatusText() {
        if (currentPlayer == DiskColor::TURN_BLACK && invMove == false && pass == false && CheckCanPlaceAll(currentPlayer)) {
            statusText.setString("Black's turn");
        } 
        else if (currentPlayer == DiskColor::TURN_WHITE && invMove == false && pass == false && CheckCanPlaceAll(currentPlayer)) {
            statusText.setString("White's turn");
        }

        if (!CheckCanPlaceAll(currentPlayer) && currentPlayer == DiskColor::TURN_BLACK) {
            statusText.setString("Pass (No spot for Black). \n White's turn");
            currentPlayer = (currentPlayer == DiskColor::TURN_BLACK) ? DiskColor::TURN_WHITE : DiskColor::TURN_BLACK;
            pass = true;
        }
        else if (!CheckCanPlaceAll(currentPlayer) && currentPlayer == DiskColor::TURN_WHITE) {
            statusText.setString("Pass (No spot for White). \n Black's turn");
            currentPlayer = (currentPlayer == DiskColor::TURN_BLACK) ? DiskColor::TURN_WHITE : DiskColor::TURN_BLACK;
            pass = true;
        }

        if (invMove && currentPlayer == DiskColor::TURN_BLACK && gameover == false) {
            statusText.setString("Invalid move \n  Black's turn");
        }
        else if (invMove && currentPlayer == DiskColor::TURN_WHITE && gameover == false) {
            statusText.setString("Invalid move \n  White's turn");
        }

        if (!CheckCanPlaceAll(DiskColor::TURN_BLACK) && !CheckCanPlaceAll(DiskColor::TURN_WHITE)) {
            countResult(DiskColor::TURN_BLACK);
            pass = false;
            gameover = true;
            std::string kuro = std::to_string(blackC);
            std::string shiro = std::to_string(whiteC);
            if (blackC > whiteC) {
                statusText.setString("Black wins!\n" + kuro + "-" + shiro);
            }
            else if (blackC < whiteC) {
                statusText.setString("White wins!\n" + kuro + "-" + shiro);
            }
            else if (blackC == whiteC) {
                statusText.setString("TIE!\n" + kuro + "-" + shiro);
            }
        }

        // Calculate the width of the text
        float textWidth = statusText.getLocalBounds().width;

        // Calculate the x-coordinate for centering the text
        float x = (WINDOW_SIZE - textWidth) / 2.0f;

        // Set the position of the text
        statusText.setPosition(x, WINDOW_SIZE + 2); // Y-coordinate is just above the white border
    }

    void countResult(DiskColor player) {
        for (int y = 0; y< BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                if (board[y][x].disk == TURN_BLACK) {
                    blackC++;
                }
            }
        }
        whiteC = 64 - blackC;
    }

    void handleMouseClick(sf::Vector2i mousePosition) {
        // Ignore clicks on the white border
        if (mousePosition.y >= WINDOW_SIZE) {
            return;
        }

        int row = mousePosition.y / CELL_SIZE;
        int col = mousePosition.x / CELL_SIZE;

        // Make a move and switch the player if valid
        if (MoveValid(row, col, currentPlayer)) {
            makeMove(row, col, currentPlayer);
            currentPlayer = (currentPlayer == DiskColor::TURN_BLACK) ? DiskColor::TURN_WHITE : DiskColor::TURN_BLACK;
            invMove = false;
            pass = false;
        }
        else {
            invMove = true;
        }
    }

    // Make a move on the board
    void makeMove(int row, int col, DiskColor player) {
        board[row][col].disk = player;

        // Flip disks in all eight directions
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) {
                    continue; // Skip zero direction
                }
                flipDirection(row, col, dr, dc, player);
            }
        }
    }

    void flipDirection(int row, int col, int dr, int dc, DiskColor player) {
        DiskColor opponent = (player == DiskColor::TURN_BLACK) ? DiskColor::TURN_WHITE : DiskColor::TURN_BLACK;
        int newRow = row + dr;
        int newCol = col + dc;
        bool foundOpponent = false;

        while (newRow >= 0 && newRow < BOARD_HEIGHT && newCol >= 0 && newCol < BOARD_HEIGHT) {
            if (board[newRow][newCol].disk == DiskColor::TURN_NONE) {
                return; // No disks to flip
            }
            if (board[newRow][newCol].disk == player) {
                if (foundOpponent) {
                    // Flip disks back to the original cell
                    int flipRow = row + dr;
                    int flipCol = col + dc;
                    while (flipRow != newRow || flipCol != newCol) {
                        board[flipRow][flipCol].disk = player;
                        flipRow += dr;
                        flipCol += dc;
                    }
                }
                return;
            }
            foundOpponent = true;
            newRow += dr;
            newCol += dc;
        }
    }
    bool MoveValid(int row, int col, DiskColor player) {
        if (board[row][col].disk != DiskColor::TURN_NONE) {
            return false; // Cell is already occupied
        }

         // Check all eight directions
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) {
                    continue; // Skip zero direction
                }
                if (checkDirection(row, col, dr, dc, player)) {
                    return true;
                }
            }
        }

        return false;
    }

    bool CheckCanPlaceAll(DiskColor player) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_HEIGHT; x++) {
                if (MoveValid(x, y, player)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool checkDirection(int row, int col, int dr, int dc, DiskColor player) {
        DiskColor opponent = (player == DiskColor::TURN_BLACK) ? DiskColor::TURN_WHITE : DiskColor::TURN_BLACK;
        int newRow = row + dr;
        int newCol = col + dc;
        bool foundOpponent = false;

        while (newRow >= 0 && newRow < BOARD_HEIGHT && newCol >= 0 && newCol < BOARD_HEIGHT) {
            if (board[newRow][newCol].disk == DiskColor::TURN_NONE) {
                return false; // No disks to flip
            }
            if (board[newRow][newCol].disk == player) {
                return foundOpponent; // Found player's disk and there were opponent's disks in between
            }
            foundOpponent = true;
            newRow += dr;
            newCol += dc;
        }

        return false;
    }


    // The classic run portion for opening up a sfml window.
    void run() {
        std::cout << "Start オセロ" << std::endl;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    std::cout << "Exit オセロ" << std::endl;
                }
                if (event.type == sf::Event::MouseButtonPressed) 
                    handleMouseClick(sf::Mouse::getPosition(window));
            }
            if (gameover == true) {
                    continue;
                }
            drawScreen();
            window.display();
        }
    }
};


int main() {
    Othello game;
    game.run();
    return 0;
}
