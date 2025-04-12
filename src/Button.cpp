#include "Utils.h"

Button::Button(float x, float y, float width, float height, 
               sf::Font& font, std::string buttonText, 
               sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor)
    : idleColor(idleColor), hoverColor(hoverColor), activeColor(activeColor),
      pressed(false), clicked(false)
{
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(idleColor);
    
    text.setFont(font);
    text.setString(buttonText);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(24);
    
    // Center text in button
    text.setPosition(
        shape.getPosition().x + (shape.getGlobalBounds().width / 2.f) - (text.getGlobalBounds().width / 2.f),
        shape.getPosition().y + (shape.getGlobalBounds().height / 2.5f) - (text.getGlobalBounds().height / 2.f)
    );
}

std::string Button::getText() const {
    return text.getString();
}

void Button::update(const sf::Vector2f& mousePos) {
    // Reset clicked state at the beginning of update
    clicked = false;
    
    // Check if mouse is hovering
    bool isHovering = shape.getGlobalBounds().contains(mousePos);
    bool mouseIsDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    
    if (isHovering) {
        if (mouseIsDown) {
            // Mouse is being held down over the button
            shape.setFillColor(activeColor);
            pressed = true;
        } else {
            // Mouse is hovering but not pressed
            shape.setFillColor(hoverColor);
            
            // If button was pressed and now released, trigger a click
            if (pressed) {
                clicked = true;
                pressed = false;
            }
        }
    } else {
        // Mouse is not over button
        shape.setFillColor(idleColor);
        pressed = false;
    }
}

void Button::render(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked() const {
    return clicked;
}

sf::FloatRect Button::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

bool Button::contains(const sf::Vector2f& point) const {
    return shape.getGlobalBounds().contains(point);
}
