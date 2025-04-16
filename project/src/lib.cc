#include "lib.hh"
#include <iomanip>

std::ostream &operator<<(std::ostream &os, const Address &address) {
  os << address.house_number << " " << address.cardinal << " "
     << address.road_number << " " << address.road_type;
  return os;
}

std::istream &operator>>(std::istream &is, Address &address) {
  is >> address.house_number >> address.cardinal >> address.road_number >>
      address.road_type;
  return is;
}

std::string Address::to_string() const {
  return house_number + " " + cardinal + " " + road_number + " " + road_type;
}

std::ostream &operator<<(std::ostream &os, const House &house) {
  os << house.address << " " << house.position.x << " " << house.position.y
     << " " << std::fixed << std::setprecision(2) << house.price << " "
     << house.area << " " << house.room_count << " " << house.bathroom_count
     << " " << house.features << std::endl;
  return os;
}

std::istream &operator>>(std::istream &is, House &house) {
  is >> house.address >> house.position.x >> house.position.y >> house.price >>
      house.area >> house.room_count >> house.bathroom_count;
  std::getline(is, house.features);
  return is;
}
std::string House::to_string() const {
  std::ostringstream price_stream;
  price_stream << std::fixed << std::setprecision(2) << price;
  return address.to_string() + " " + std::to_string(position.x) + " " +
         std::to_string(position.y) + " " + price_stream.str() + " " +
         std::to_string(area) + " " + std::to_string(room_count) + " " +
         std::to_string(bathroom_count) + " " + features;
}
