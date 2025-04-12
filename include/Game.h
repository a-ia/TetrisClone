#pragma once
#include "Piece.h"
#include "Utils.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class Game {
private:
    // Constants
    static const int GRID_WIDTH = 16;
    static const int GRID_HEIGHT = 30;
    static const int VISIBLE_HEIGHT = 20;
    float tile_size;
    
    // Game state
    GameState state;
    byte grid[GRID_HEIGHT][GRID_WIDTH];
    byte colliders[GRID_HEIGHT][GRID_WIDTH];
    sf::Color placedColors[GRID_HEIGHT][GRID_WIDTH];
    Piece currentPiece;
    unsigned int timer;
    unsigned int gameSpeed;
    unsigned int scoreCounter;

    // SFML objects
    sf::RenderWindow window;
    sf::Font font;
    sf::Text scoreText;
    sf::Text pausedText;
    sf::Text titleText;
    std::vector<Button> pauseMenuButtons;
    std::vector<Button> mainMenuButtons;
    
    // Sound effects
    sf::SoundBuffer fallBuffer;
    sf::Sound fallSound;
    sf::SoundBuffer lineBuffer;
    sf::Sound lineSound;
    sf::SoundBuffer tetrisBuffer;
    sf::Sound tetrisSound;
    
    // Game methods
    void processEvents();
    void update();
    void render();
    void checkLines();
    void spawnNewPiece();
    bool isGameOver();
    void drawPiece(const Piece& piece, sf::Color color);
    void drawGrid();
    void resetGame();
    void exitGame();

    // Menu methods
    void initPauseMenu();
    void initMainMenu();
    void updatePauseMenu();
    void updateMainMenu();
    void renderPauseMenu();
    void renderMainMenu();
    
public:
    Game();
    void run();
};
