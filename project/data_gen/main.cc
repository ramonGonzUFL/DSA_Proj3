#include "lib.hh"
#include "structures/quadtree.hh"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>

// since these aren't part of the main program, i'll just use statics for
// simplicity.
static std::random_device rd;
static std::mt19937 gen(rd());
static size_t count = 100000;
static std::string cardinality = "se";
static float width = 40000;
static float height = 40000;
static std::uniform_real_distribution<float> x_dist(0, width);
static std::uniform_real_distribution<float> y_dist(0, height);
static float min_price = 400000;
static float max_price = 3000000;
static std::uniform_real_distribution<float> price_dist(min_price, max_price);
static std::vector<std::string> features = {
    "Large Kitchen",     "Pool",
    "Master Suite",      "Home Office",
    "Finished Basement", "Hardwood Floors",
    "Gourmet Kitchen",   "High Ceilings",
    "Walk-in Closet",    "Fireplace"};

int main() {
  Quadtree<House> world{0, width, 0, height};

  for (size_t i = 0; i < count; i++) {
    // Position generation
    float x = x_dist(gen);
    float y = y_dist(gen);
    sf::Vector2f pos{x, y};

    // Price Generation
    float price = price_dist(gen) * 5;
    int count = 5;

    std::vector<House *> near = world.find_in_radius(pos, 1600);

    for (House *h : near) {
      price += h->price;
    }
    count += near.size();
    price = price / count;

    // Address Generation
    bool is_street = gen() % 2;

    std::string house_num;
    std::string road_num;
    if (is_street) {
      house_num = std::to_string(static_cast<int>(x / 2));
      road_num = std::to_string(static_cast<int>(y / 200));
    } else {
      house_num = std::to_string(static_cast<int>(y / 2));
      road_num = std::to_string(static_cast<int>(x / 200));
    }

    std::string suffix;
    char final_digit = road_num.back();
    switch (final_digit) {
    case '1':
      suffix = "st";
      break;
    case '2':
      suffix = "nd";
      break;
    case '3':
      suffix = "rd";
      break;
    default:
      suffix = "th";
      break;
    }
    road_num = road_num + suffix;

    float base_area = price * 0.000175;
    std::uniform_real_distribution<float> area_dist(base_area - 100,
                                                    base_area + 100);
    float area = area_dist(gen);
    if (area < 50) {
      area = 50;
    }

    int base_roomcount = static_cast<int>(price / 330000);
    std::uniform_int_distribution<int> room_dist(base_roomcount - 2,
                                                 base_roomcount + 2);
    int roomcount = room_dist(gen);
    if (roomcount < 1) {
      roomcount = 1;
    }

    int bathroom_center = roomcount / 2;
    int bathroom_delta = roomcount / 2;
    std::uniform_int_distribution<int> dist(0, bathroom_delta * 2);
    int bathcount = bathroom_center + (dist(gen) - dist(gen));
    if (bathcount < 1)
      bathcount = 1;
    if (bathcount > roomcount)
      bathcount = roomcount;

    std::uniform_int_distribution<int> num_features_dist(1, 3);
    size_t num_features = num_features_dist(gen);

    std::vector<std::string> selected_features;
    std::vector<int> indices;
    for (size_t i = 0; i < features.size(); i++) {
      indices.push_back(i);
    }

    std::shuffle(indices.begin(), indices.end(), gen);
    for (size_t i = 0; i < num_features; i++) {
      selected_features.push_back(features[indices[i]]);
    }

    std::string features_str;
    for (size_t i = 0; i < selected_features.size(); i++) {
      features_str += selected_features[i];
      if (i < selected_features.size() - 1) {
        features_str += ", ";
      }
    }

    Address address{house_num, cardinality, road_num, is_street ? "st" : "ave"};

    House h{address,
            pos,
            price,
            area,
            static_cast<unsigned int>(roomcount),
            static_cast<unsigned int>(bathcount),
            features_str};

    std::cout << h;
    world.add_item(std::move(h));
  }
}
