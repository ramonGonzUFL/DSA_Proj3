#pragma once
#include <SFML/System/Vector2.hpp>
#include <istream>
#include <ostream>
#include <string>

struct Address {
  std::string house_number; // 1234
  std::string cardinal;     // sw
  std::string road_number;  // 46th
  std::string road_type;    // st

  friend std::ostream &operator<<(std::ostream &os, const Address &address);
  friend std::istream &operator>>(std::istream &is, Address &address);

  std::string to_string() const;
};

struct House {
  Address address;
  sf::Vector2f position;
  float price;
  float area;
  unsigned int room_count;
  unsigned int bathroom_count;
  std::string features;

  friend std::ostream &operator<<(std::ostream &os, const House &house);
  friend std::istream &operator>>(std::istream &is, House &house);

  std::string to_string() const;
};
