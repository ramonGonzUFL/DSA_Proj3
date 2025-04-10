#include "structures/quadtree.hh"
#include <SFML/System/Vector2.hpp>
#include <cassert>
#include <iostream>

int main() {
  try {
    Quadtree<sf::Vector2f> quadtree;

    quadtree.add_item(std::make_unique<sf::Vector2f>(0.5f, 0.5f));   // center
    quadtree.add_item(std::make_unique<sf::Vector2f>(0.25f, 0.25f)); // top-left
    quadtree.add_item(
        std::make_unique<sf::Vector2f>(0.75f, 0.25f)); // top-right
    quadtree.add_item(
        std::make_unique<sf::Vector2f>(0.25f, 0.75f)); // bottom-left
    quadtree.add_item(
        std::make_unique<sf::Vector2f>(0.75f, 0.75f)); // bottom-right

    // Check if bounds expansion works
    quadtree.add_item(
        std::make_unique<sf::Vector2f>(-1.0f, -1.0f)); // far top-left
    quadtree.add_item(
        std::make_unique<sf::Vector2f>(2.0f, 2.0f)); // far bottom-right

    // Check if we can search in a radius
    auto points = quadtree.find_in_radius(sf::Vector2f(0.5f, 0.5f), 0.4f);

    // Check that we found exactly 5 points (center + 4 quadrants)
    if (points.size() != 5) {
      throw std::runtime_error("Expected to find 5 points, but found " +
                               std::to_string(points.size()));
    }

    // Try a smaller radius (the corner points are approx 0.3535 away)
    auto points2 = quadtree.find_in_radius(sf::Vector2f(0.5f, 0.5f), 0.1f);

    // Check that we found exactly 1 point (only the center point should be
    // within 0.1 radius)
    if (points2.size() != 1) {
      throw std::runtime_error("Expected to find 1 point, but found " +
                               std::to_string(points2.size()));
    }

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error during quadtree test: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Unknown error during quadtree test" << std::endl;
    return 1;
  }
}
