#include "Game.h"
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>

Game::Game() 
    : tile_size(45.0f),
      timer(0),
      gameSpeed(20),
      scoreCounter(0),
      state(GameState::MAIN_MENU)
{
    // Create window
    window.create(sf::VideoMode(tile_size * GRID_WIDTH, tile_size * VISIBLE_HEIGHT), 
                  "Tetris", 
                  sf::Style::Titlebar | sf::Style::Close);
    window.setKeyRepeatEnabled(true);
    window.setFramerateLimit(60);
    
    // Initialize grid arrays
    resetGame();
    
    // Load font
    if (!font.loadFromFile("../PixgamerRegular-OVD6A.ttf")) {
    }
    
    // Setup score text
    scoreText.setFont(font);
    scoreText.setCharacterSize(15);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(10, 25);
    scoreText.setScale(1.5f, 1.5f);
    scoreText.setString("Lines: 0");
    
    // Setup paused text
    pausedText.setFont(font);
    pausedText.setCharacterSize(40);
    pausedText.setFillColor(sf::Color::Red);
    pausedText.setString("PAUSED");
    pausedText.setPosition(
        (window.getSize().x / 2.f) - (pausedText.getGlobalBounds().width / 2.f),
        window.getSize().y / 8.f
    );

    titleText.setFont(font);
    titleText.setCharacterSize(60);
    titleText.setString("Tetris by Sharon");  
    titleText.setLetterSpacing(1.2f);

    sf::FloatRect textBounds = titleText.getLocalBounds();
    titleText.setOrigin(textBounds.left + textBounds.width / 2.f, 0.f);
    titleText.setPosition(window.getSize().x / 2.f, 50.f);


    // Initialize menus
    initPauseMenu();
    initMainMenu();
    
    // Load sound effects
    if (!fallBuffer.loadFromFile("../sounds/fall.wav") ||
        !lineBuffer.loadFromFile("../sounds/line.wav") ||
        !tetrisBuffer.loadFromFile("../sounds/tetris.wav")) {
    }
    
    fallSound.setBuffer(fallBuffer);
    lineSound.setBuffer(lineBuffer);
    tetrisSound.setBuffer(tetrisBuffer);
    
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Create first piece
    spawnNewPiece();
}

void Game::run() {
  while (window.isOpen() && state != GameState::EXIT) {
      processEvents();
      update();
      render();
  }
  window.close();
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            state = GameState::EXIT;
        }

        switch (state) {
            case GameState::PLAYING:
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        state = GameState::PAUSED;
                    }
                    else if (event.key.code == sf::Keyboard::Left &&
                        currentPiece.a.x != 0 && currentPiece.b.x != 0 && 
                        currentPiece.c.x != 0 && currentPiece.d.x != 0 &&
                        (colliders[currentPiece.a.y][currentPiece.a.x - 1]) != 2 && 
                        (colliders[currentPiece.b.y][currentPiece.b.x - 1]) != 2 &&
                        (colliders[currentPiece.c.y][currentPiece.c.x - 1]) != 2 && 
                        (colliders[currentPiece.d.y][currentPiece.d.x - 1]) != 2) {
                        currentPiece.a.x--;
                        currentPiece.b.x--;
                        currentPiece.c.x--;
                        currentPiece.d.x--;
                    }
                    else if (event.key.code == sf::Keyboard::Right &&
                        currentPiece.a.x != GRID_WIDTH-1 && currentPiece.b.x != GRID_WIDTH-1 && 
                        currentPiece.c.x != GRID_WIDTH-1 && currentPiece.d.x != GRID_WIDTH-1 &&
                        (colliders[currentPiece.a.y][currentPiece.a.x + 1]) != 2 && 
                        (colliders[currentPiece.b.y][currentPiece.b.x + 1]) != 2 &&
                        (colliders[currentPiece.c.y][currentPiece.c.x + 1]) != 2 && 
                        (colliders[currentPiece.d.y][currentPiece.d.x + 1]) != 2) {
                        currentPiece.a.x++;
                        currentPiece.b.x++;
                        currentPiece.c.x++;
                        currentPiece.d.x++;
                    }
                    else if (event.key.code == sf::Keyboard::Up) {
                        rotatePiece(currentPiece, colliders);
                    }
                }

                if (event.type == sf::Event::KeyReleased) {
                    gameSpeed = 10;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                    gameSpeed = 1;
                }
                break;

            case GameState::PAUSED:
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    state = GameState::PLAYING;
                }
                break;

            case GameState::MAIN_MENU:
                // No event handling for buttons here - moved to updateMainMenu()
                break;

            case GameState::GAME_OVER:
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    resetGame();
                    state = GameState::PLAYING;
                }
                break;
        }
    }
}

void Game::update() {
    // Get mouse position
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    switch (state) {
        case GameState::PLAYING:
            // Refresh the grid-array
            for (int i = 0; i < GRID_HEIGHT; i++) {
                for (int j = 0; j < GRID_WIDTH; j++) {
                    if (colliders[i][j] == 2) {
                        grid[i][j] = 2;
                    } else {
                        grid[i][j] = 0;
                    }
                }
            }
            
            // Clock logic
            if (timer > gameSpeed) {
                // Collision checks
                if (grid[currentPiece.a.y + 1][currentPiece.a.x] == 2 ||
                    grid[currentPiece.b.y + 1][currentPiece.b.x] == 2 ||
                    grid[currentPiece.c.y + 1][currentPiece.c.x] == 2 ||
                    grid[currentPiece.d.y + 1][currentPiece.d.x] == 2 ||
                    currentPiece.a.y == VISIBLE_HEIGHT-1 || 
                    currentPiece.b.y == VISIBLE_HEIGHT-1 || 
                    currentPiece.c.y == VISIBLE_HEIGHT-1 || 
                    currentPiece.d.y == VISIBLE_HEIGHT-1) {
                    
                    fallSound.play();
                    
                    // Lock the piece in place
                    grid[currentPiece.a.y][currentPiece.a.x] = 2;
                    grid[currentPiece.b.y][currentPiece.b.x] = 2;
                    grid[currentPiece.c.y][currentPiece.c.x] = 2;
                    grid[currentPiece.d.y][currentPiece.d.x] = 2;
                    
                    colliders[currentPiece.a.y][currentPiece.a.x] = 2;
                    colliders[currentPiece.b.y][currentPiece.b.x] = 2;
                    colliders[currentPiece.c.y][currentPiece.c.x] = 2;
                    colliders[currentPiece.d.y][currentPiece.d.x] = 2;
                    
                    // Store the colors
                    placedColors[currentPiece.a.y][currentPiece.a.x] = getPieceColor(currentPiece.color);
                    placedColors[currentPiece.b.y][currentPiece.b.x] = getPieceColor(currentPiece.color);
                    placedColors[currentPiece.c.y][currentPiece.c.x] = getPieceColor(currentPiece.color);
                    placedColors[currentPiece.d.y][currentPiece.d.x] = getPieceColor(currentPiece.color);
                    
                    // Check and clear completed lines
                    checkLines();
                    
                    // Check for game over
                    if (isGameOver()) {
                        state = GameState::GAME_OVER;
                        return;
                    }
                    
                    // Spawn new piece
                    spawnNewPiece();
                }
                else {
                    // Move piece down
                    grid[currentPiece.a.y + 1][currentPiece.a.x] = 1;
                    grid[currentPiece.b.y + 1][currentPiece.b.x] = 1;
                    grid[currentPiece.c.y + 1][currentPiece.c.x] = 1;
                    grid[currentPiece.d.y + 1][currentPiece.d.x] = 1;
                    
                    currentPiece.a.y++;
                    currentPiece.b.y++;
                    currentPiece.c.y++;
                    currentPiece.d.y++;
                }
                
                timer = 0;
            }
            else {
                timer++;
            }
            break;
            
        case GameState::PAUSED:
            updatePauseMenu();
            break;
            
        case GameState::MAIN_MENU:
            updateMainMenu();
            break;
            
        case GameState::GAME_OVER:
            // Wait for key press to restart
            break;
    }
}

void Game::render() {
    window.clear(sf::Color::White);
    
    switch (state) {
        case GameState::PLAYING:
            // Draw the current piece
            drawPiece(currentPiece, getPieceColor(currentPiece.color));
            
            // Draw the grid
            drawGrid();
            
            // Draw score
            window.draw(scoreText);
            break;
            
        case GameState::PAUSED:
            // Draw the game in the background
            drawPiece(currentPiece, getPieceColor(currentPiece.color));
            drawGrid();
            window.draw(scoreText);
            
            // Draw pause menu overlay
            renderPauseMenu();
            break;
            
        case GameState::MAIN_MENU:
            renderMainMenu();
            break;
            
        case GameState::GAME_OVER:
            // Draw the grid (game over state)
            drawGrid();
            
            // Draw game over text
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setString("GAME OVER");
            gameOverText.setPosition(
                (window.getSize().x / 2.f) - (gameOverText.getGlobalBounds().width / 2.f),
                window.getSize().y / 4.f
            );
            window.draw(gameOverText);
            
            // Draw final score
            sf::Text finalScoreText;
            finalScoreText.setFont(font);
            finalScoreText.setCharacterSize(30);
            finalScoreText.setFillColor(sf::Color::Black);
            
            char temp[256];
            sprintf(temp, "Final Score: %i", scoreCounter);
            finalScoreText.setString(temp);
            
            finalScoreText.setPosition(
                (window.getSize().x / 2.f) - (finalScoreText.getGlobalBounds().width / 2.f),
                window.getSize().y / 2.f
            );
            window.draw(finalScoreText);
            
            // Draw restart prompt
            sf::Text restartText;
            restartText.setFont(font);
            restartText.setCharacterSize(20);
            restartText.setFillColor(sf::Color::Black);
            restartText.setString("Press ENTER to restart");
            restartText.setPosition(
                (window.getSize().x / 2.f) - (restartText.getGlobalBounds().width / 2.f),
                window.getSize().y / 1.5f
            );
            window.draw(restartText);
            break;
    }
    
    window.display();
}

void Game::checkLines() {
    byte tetris_row = 0;
    
    for (int i = 0; i < GRID_HEIGHT; i++) {
        byte blocks_in_a_row = 0;
        
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (colliders[i][j] == 2) {
                blocks_in_a_row++;
            }
        }
        
        if (blocks_in_a_row == GRID_WIDTH) {
            tetris_row++;
            
            // Play appropriate sound
            if (tetris_row >= 4) {
                tetrisSound.play();
            } else {
                lineSound.play();
            }
            
            // Move lines down
            for (int k = i; k > 0; k--) {
                for (int l = 0; l < GRID_WIDTH; l++) {
                    colliders[k][l] = colliders[k - 1][l];
                    placedColors[k][l] = placedColors[k-1][l];
                }
            }
            
            // Update score
            scoreCounter++;
            char temp[256];
            sprintf(temp, "Lines: %i", scoreCounter);
            scoreText.setString(temp);
        }
    }
}

void Game::spawnNewPiece() {
    currentPiece = CreatePiece(static_cast<PIECE_TYPE>((rand() % 7)));
}

bool Game::isGameOver() {
    for (int i = 0; i < GRID_WIDTH; i++) {
        if (colliders[0][i] == 2) {
            return true;
        }
    }
    return false;
}

void Game::drawPiece(const Piece& piece, sf::Color color) {
    sf::RectangleShape blockA(sf::Vector2f(tile_size-1, tile_size-1));
    blockA.setPosition(tile_size * piece.a.x, tile_size * piece.a.y);
    blockA.setFillColor(color);
    blockA.setOutlineThickness(1);
    blockA.setOutlineColor(sf::Color::Black);
    window.draw(blockA);
    
    sf::RectangleShape blockB(sf::Vector2f(tile_size-1, tile_size-1));
    blockB.setPosition(tile_size * piece.b.x, tile_size * piece.b.y);
    blockB.setFillColor(color);
    blockB.setOutlineThickness(1);
    blockB.setOutlineColor(sf::Color::Black);
    window.draw(blockB);
    
    sf::RectangleShape blockC(sf::Vector2f(tile_size-1, tile_size-1));
    blockC.setPosition(tile_size * piece.c.x, tile_size * piece.c.y);
    blockC.setFillColor(color);
    blockC.setOutlineThickness(1);
    blockC.setOutlineColor(sf::Color::Black);
    window.draw(blockC);
    
    sf::RectangleShape blockD(sf::Vector2f(tile_size-1, tile_size-1));
    blockD.setPosition(tile_size * piece.d.x, tile_size * piece.d.y);
    blockD.setFillColor(color);
    blockD.setOutlineThickness(1);
    blockD.setOutlineColor(sf::Color::Black);
    window.draw(blockD);
}

void Game::drawGrid() {
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (grid[i][j] == 1) {
                sf::RectangleShape activeBlock(sf::Vector2f(tile_size-1, tile_size-1));
                activeBlock.setPosition(tile_size * j, tile_size * i);
                activeBlock.setFillColor(getPieceColor(currentPiece.color));
                activeBlock.setOutlineThickness(1);
                activeBlock.setOutlineColor(sf::Color::Black);
                window.draw(activeBlock);
            }
            if (colliders[i][j] == 2) {
                sf::RectangleShape placedBlock(sf::Vector2f(tile_size-1, tile_size-1));
                placedBlock.setPosition(tile_size * j, tile_size * i);
                placedBlock.setFillColor(placedColors[i][j]);
                placedBlock.setOutlineThickness(1);
                placedBlock.setOutlineColor(sf::Color::Black);
                window.draw(placedBlock);
            }
        }
    }
}

void Game::resetGame() {
    // Clear the grid and colliders
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            grid[i][j] = 0;
            colliders[i][j] = 0;
            placedColors[i][j] = sf::Color::White;
        }
    }
    
    // Reset score
    scoreCounter = 0;
    scoreText.setString("Lines: 0");
    
    // Reset game speed
    gameSpeed = 10;
    timer = 0;
    
    // Create a new piece
    spawnNewPiece();
}

void Game::initPauseMenu() {
    float buttonWidth = 200.f;
    float buttonHeight = 45.f;
    float centerX = (window.getSize().x / 2.f) - (buttonWidth / 2.f);
    
    // Pause menu buttons
    pauseMenuButtons.push_back(
        Button(centerX, window.getSize().y / 2.5f, buttonWidth, buttonHeight,
               font, "Continue", sf::Color(70, 70, 70, 200), sf::Color(150, 150, 150, 255), sf::Color(20, 20, 20, 200))
    );
    
    pauseMenuButtons.push_back(
        Button(centerX, window.getSize().y / 1.8f, buttonWidth, buttonHeight,
               font, "Restart", sf::Color(70, 70, 70, 200), sf::Color(150, 150, 150, 255), sf::Color(20, 20, 20, 200))
    );
    
    pauseMenuButtons.push_back(
        Button(centerX, window.getSize().y / 1.4f, buttonWidth, buttonHeight,
               font, "Main Menu", sf::Color(70, 70, 70, 200), sf::Color(150, 150, 150, 255), sf::Color(20, 20, 20, 200))
    );
}

void Game::initMainMenu() {
    float buttonWidth = 200.f;
    float buttonHeight = 45.f;
    float centerX = (window.getSize().x / 2.f) - (buttonWidth / 2.f);

    mainMenuButtons.clear();

    // Main menu buttons
    mainMenuButtons.push_back(
        Button(centerX, window.getSize().y / 1.8f, buttonWidth, buttonHeight,
               font, "Play", sf::Color(70, 70, 70, 200), sf::Color(150, 150, 150, 255), sf::Color(20, 20, 20, 200))
    );
    
    mainMenuButtons.push_back(
        Button(centerX, window.getSize().y / 1.4f, buttonWidth, buttonHeight,
               font, "Exit", sf::Color(70, 70, 70, 200), sf::Color(150, 150, 150, 255), sf::Color(20, 20, 20, 200))
    );
}


void Game::updatePauseMenu() {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    for (auto& button : pauseMenuButtons) {
        button.update(mousePos);

        if (button.isClicked()) {
            if (&button == &pauseMenuButtons[0]) {
                // Continue
                state = GameState::PLAYING;
            }
            else if (&button == &pauseMenuButtons[1]) {
                // Restart
                resetGame();
                state = GameState::PLAYING;
            }
            else if (&button == &pauseMenuButtons[2]) {
                // Main Menu
                resetGame();
                state = GameState::MAIN_MENU;
            }
        }
    } 
}

void Game::updateMainMenu() {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    for (auto& button : mainMenuButtons) {
        button.update(mousePos);
        
        if (button.isClicked()) {
            std::string label = button.getText();
            
            if (label == "Play") {
                resetGame();
                state = GameState::PLAYING;
            } else if (label == "Exit") {
                state = GameState::EXIT;
            }
        }
    }
}

//void Game::updateMainMenu() {
//    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    // Update all buttons (hover states only)
//    for (auto& button : mainMenuButtons) {
//        button.update(mousePos);
//    }
    // No click handling here
//}

void Game::renderPauseMenu() {
    // Semi-transparent background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(window.getSize()));
    background.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(background);
    
    // Draw pause text
    window.draw(pausedText);
    
    // Draw buttons
    for (auto& button : pauseMenuButtons) {
        button.render(window);
    }
}

void Game::renderMainMenu() {
    window.clear(sf::Color(230, 230, 230));

    std::string titleStr = "Tetris by Sharon";
    float startY = window.getSize().y / 6.f;

    float totalWidth = 0.f;
    std::vector<float> advances;

    // First pass: measure width of each character
    for (char c : titleStr) {
        const sf::Glyph& glyph = font.getGlyph(c, 50, false);
        advances.push_back(glyph.advance);
        totalWidth += glyph.advance;
    }

    // Calculate starting X to center the whole thing
    float startX = (window.getSize().x / 2.f) - (totalWidth / 2.f);

    float x = startX;
    for (size_t i = 0; i < titleStr.size(); ++i) {
        sf::Text charText;
        charText.setFont(font);
        charText.setCharacterSize(50);
        charText.setString(titleStr[i]);
        charText.setFillColor(getPieceColor(i % 7));
        charText.setPosition(x, startY);
        window.draw(charText);

        x += advances[i]; // move forward by actual character width
    }

    // Draw buttons
    for (auto& button : mainMenuButtons) {
        button.render(window);
    }
}

