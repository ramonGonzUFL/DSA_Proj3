#include "lib.hh"
#include "structures/redblack.hh"
#include "ui/button.hh"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <iostream>
#include <string>

std::vector<std::shared_ptr<UIComponent>>
load_page(const std::vector<House> &houses, int page_number,
          const sf::Font &font) {
  std::vector<std::shared_ptr<UIComponent>> components;

  // Calculate start and end indices for the page
  size_t entries_per_page = 4;
  size_t start_idx = page_number * entries_per_page;
  size_t end_idx = std::min(start_idx + entries_per_page, houses.size());

  // Fixed position for the first label set
  sf::Vector2f position(600, 100);
  float y_offset_between_houses =
      120.0f; // Vertical spacing between house entries
  float y_offset_between_labels =
      25.0f; // Vertical spacing between labels of the same house

  // Create multiple labels for each house entry in the current page
  for (size_t i = start_idx; i < end_idx; ++i) {
    const House &house = houses[i];
    std::vector<const UIComponent *> house_labels;

    // Line 1: Address information
    std::string address_info = "Address: " + house.address.to_string();
    auto address_label = std::make_shared<Label>(
        address_info, position, font, sf::Vector2f{0, 0}, 30,
        sf::Color::Transparent, sf::Color::Black);
    components.push_back(address_label);
    house_labels.push_back(address_label.get());

    // Line 2: Price and room information
    sf::Vector2f price_position = position;
    price_position.y += y_offset_between_labels;
    std::string price_info =
        "Price: $" + std::to_string(static_cast<int>(house.price)) +
        " | Area: " + std::to_string(static_cast<int>(house.area)) + " sq ft" +
        " | Rooms: " + std::to_string(house.room_count);
    auto price_label = std::make_shared<Label>(
        price_info, price_position, font, sf::Vector2f{0, 0}, 20,
        sf::Color::Transparent, sf::Color::Black);
    components.push_back(price_label);
    house_labels.push_back(price_label.get());

    // Line 3: Features
    sf::Vector2f features_position = price_position;
    features_position.y += y_offset_between_labels;
    std::string features_info = house.features;
    auto features_label = std::make_shared<Label>(
        features_info, features_position, font, sf::Vector2f{0, 0}, 15,
        sf::Color::Transparent, sf::Color::Black);
    components.push_back(features_label);
    house_labels.push_back(features_label.get());

    // Update position for next house entry
    position.y += y_offset_between_houses;
  }

  // Create a border around all components (houses)
  if (!components.empty()) {
    std::vector<const UIComponent *> all_components;
    for (const auto &comp : components) {
      all_components.push_back(comp.get());
    }

    // Add the border to the back of the components list
    auto global_border = std::make_shared<Backing>(all_components, 20.0f,
                                                   sf::Color(230, 230, 230));
    components.insert(
        components.begin(),
        global_border); // Insert at beginning so it's drawn first (at the back)
  }

  return components;
}
int main() {
  auto window =
      sf::RenderWindow(sf::VideoMode({1280u, 720u}), "House Searching Project",
                       sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(144);

  sf::Font font;

  if (!font.openFromFile("fonts/Roboto.ttf")) {
    std::cerr << "Could not load font file." << std::endl;
    return -1;
  }

  auto data = load_file("data_gen/data");

  auto loaded_stats = std::make_shared<Label>(
      "Successfully loaded " + std::to_string(data.size()) + " entries.",
      sf::Vector2f(5, 720 - 60 - 5), font, sf::Vector2f{0, 0}, 20,
      sf::Color::Transparent, sf::Color(150, 150, 150));

  auto swap_mode = std::make_shared<Button>("Swap", sf::Vector2f(20, 20), font);

  bool is_rb = true;
  auto mode_rb =
      std::make_shared<Label>("Red-Black Tree", swap_mode->right(), font);
  auto mode_bp = std::make_shared<Label>("B+ Tree", swap_mode->right(), font);

  // Price filter sliders
  float min_price = 0.0f;
  float max_price = 1000000.0f; // Assuming maximum price of 1 million

  // Min price slider
  auto min_price_slider = std::make_shared<Slider>(
      sf::Vector2f(100, 100), 0.0f, max_price, min_price, font,
      sf::Vector2f(250, 10), "Min Price");

  // Max price slider
  auto max_price_slider = std::make_shared<Slider>(
      sf::Vector2f(100, 150), 0.0f, max_price, max_price, font,
      sf::Vector2f(250, 10), "Max Price");

  // Search button
  auto search_button = std::make_shared<Button>(
      "Search", sf::Vector2f(100, 200), font, sf::Vector2f(100, 40));

  // Page navigation
  int current_page = 0;
  int total_pages = (data.size() + 3) / 4; // 4 houses per page

  auto prev_button = std::make_shared<Button>(
      "Previous", sf::Vector2f(450, 650), font, sf::Vector2f(120, 40));
  auto next_button = std::make_shared<Button>("Next", sf::Vector2f(590, 650),
                                              font, sf::Vector2f(120, 40));

  auto page_indicator =
      std::make_shared<Label>("Page " + std::to_string(current_page + 1) +
                                  " of " + std::to_string(total_pages),
                              sf::Vector2f(500, 600), font, sf::Vector2f{0, 0},
                              24, sf::Color::Transparent, sf::Color::Black);

  // Initially disable previous button on first page
  prev_button->setEnabled(false);
  if (total_pages <= 1) {
    next_button->setEnabled(false);
  }

  auto loaded = load_page(data, current_page, font);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (event->is<sf::Event::MouseButtonPressed>()) {
        const auto &mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();

        if (swap_mode->wasClicked(mouseEvent->position)) {
          is_rb = !is_rb;
        } else if (search_button->wasClicked(mouseEvent->position)) {
          // Print search criteria to cout
          std::cout << "Searching for houses with price between $"
                    << min_price_slider->getValue() << " and $"
                    << max_price_slider->getValue() << std::endl;
        } else if (prev_button->wasClicked(mouseEvent->position) &&
                   prev_button->isEnabled()) {
          current_page--;
          loaded = load_page(data, current_page, font);

          // Update button states
          next_button->setEnabled(true);
          if (current_page == 0) {
            prev_button->setEnabled(false);
          }

          // Update page indicator
          page_indicator->setText("Page " + std::to_string(current_page + 1) +
                                  " of " + std::to_string(total_pages));
        } else if (next_button->wasClicked(mouseEvent->position) &&
                   next_button->isEnabled()) {
          current_page++;
          loaded = load_page(data, current_page, font);

          // Update button states
          prev_button->setEnabled(true);
          if (current_page >= total_pages - 1) {
            next_button->setEnabled(false);
          }

          // Update page indicator
          page_indicator->setText("Page " + std::to_string(current_page + 1) +
                                  " of " + std::to_string(total_pages));
        }
      }

      // Handle slider events
      min_price_slider->handleEvent(*event);
      max_price_slider->handleEvent(*event);

      // Make sure min price doesn't exceed max price
      if (min_price_slider->getValue() > max_price_slider->getValue()) {
        min_price_slider->setValue(max_price_slider->getValue());
      }

      // Make sure max price isn't less than min price
      if (max_price_slider->getValue() < min_price_slider->getValue()) {
        max_price_slider->setValue(min_price_slider->getValue());
      }
    }

    window.clear(sf::Color::White);
    loaded_stats->draw(window);

    swap_mode->draw(window);
    if (is_rb) {
      mode_rb->draw(window);
    } else {
      mode_bp->draw(window);
    }

    // Draw price filter sliders
    min_price_slider->draw(window);
    max_price_slider->draw(window);

    // Draw search button
    search_button->draw(window);

    // Draw all labels in the current page
    for (const auto &component : loaded) {
      component->draw(window);
    }

    // Draw pagination controls
    prev_button->draw(window);
    next_button->draw(window);
    page_indicator->draw(window);

    window.display();
  }
}
