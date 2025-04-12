#pragma once
#include <cstdint>
#include <SFML/Graphics.hpp>

typedef uint8_t byte;

struct Vec2
{
    byte y, x;
    Vec2(){}
    Vec2(byte dy, byte dx) : x(dx), y(dy){}
};

enum PIECE_TYPE
{
    S, Z, L, J, SQR, I, T
};

// Game states
enum class GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    EXIT
};

// Button class for menus
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    bool pressed;    // Button is currently being held down
    bool clicked;    // Button was just clicked this frame
    
public:
    Button(float x, float y, float width, float height, 
           sf::Font& font, std::string text, 
           sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor);
    
    // Accessors
    std::string getText() const;
    bool isClicked() const;
    
    // Functions
    void update(const sf::Vector2f& mousePos);
    void render(sf::RenderWindow& window);
    
    // For collision detection
    sf::FloatRect getGlobalBounds() const;
    bool contains(const sf::Vector2f& point) const;
};
