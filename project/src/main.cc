#include "lib.hh"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

int main() {
  auto window = sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Hello World");
  window.setFramerateLimit(144);

  // Load font (use default if no file)
  sf::Font font;

  if (!font.openFromFile("fonts/Roboto.ttf")) {
    std::cerr << "Could not load font file." << std::endl;
    return -1;
  }

  sf::Text text(font, "Hello", 50);
  text.setFillColor(sf::Color::White);
  text.setPosition(sf::Vector2f(100, 100));

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    window.clear();
    window.draw(text); // Render the text
    window.display();
  }
}
