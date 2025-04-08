#pragma once

#include <SFML/System/Vector2.hpp>
#include <memory>
class Positioned {
public:
  virtual sf::Vector2f get_position() const = 0;
};

template <class T> class Quadtree {
  std::unique_ptr<Quadtree<T>> top_left;
  std::unique_ptr<Quadtree<T>> top_right;
  std::unique_ptr<Quadtree<T>> bottom_left;
  std::unique_ptr<Quadtree<T>> bottom_right;

  float left = 0.0f;
  float right = 1.0f;
  float top = 0.0f;
  float bottom = 1.0f;

  std::unique_ptr<T> held;

  // Expands the quadtree towards a target.
  void expand_towards(sf::Vector2f target) {
    bool left = target.x < left;
    bool up = target.y < top;

    std::unique_ptr<Quadtree<T>> new_child;
    new_child->top_left.swap(top_left);
    new_child->top_right.swap(top_right);
    new_child->bottom_left.swap(bottom_left);
    new_child->bottom_right.swap(bottom_right);
    new_child->left = left;
    new_child->right = right;
    new_child->top = top;
    new_child->bottom = bottom;

    // Recalculate bounds
    if (left)
      left -= right - left;
    else
      right += right - left;

    if (up)
      up -= bottom - top;
    else
      bottom += bottom - top;

    new_child->held.swap(held);

    // Set child to correct position
    if (left && up)
      bottom_right.swap(new_child);
    else if (left && !up)
      top_right.swap(new_child);
    else if (!left && up)
      bottom_left.swap(new_child);
    else if (!left && !up)
      top_left.swap(new_child);
  }

  bool is_in_bounds(const sf::Vector2f item) const {
    return item.x >= left && item.x <= right && item.y >= top &&
           item.y <= bottom;
  }

public:
  void add_item(T &&item) {
    const sf::Vector2f pos = item.get_position();
    while (!is_in_bounds(pos))
      expand_towards(pos);
  }
};

template <> class Quadtree<Positioned>;
