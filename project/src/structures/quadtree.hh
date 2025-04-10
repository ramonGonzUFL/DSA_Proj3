#pragma once

#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

template <typename T> sf::Vector2f get_position(const T &value);

// For debug purposes.
template <> sf::Vector2f get_position<sf::Vector2f>(const sf::Vector2f &value) {
  return value;
}

// TODO: Implement equivalent method for data entries.

template <class T> class Quadtree {
  std::unique_ptr<Quadtree<T>> top_left{nullptr};
  std::unique_ptr<Quadtree<T>> top_right{nullptr};
  std::unique_ptr<Quadtree<T>> bottom_left{nullptr};
  std::unique_ptr<Quadtree<T>> bottom_right{nullptr};

  float left = 0.0f;
  float right = 1.0f;
  float top = 0.0f;
  float bottom = 1.0f;

  std::unique_ptr<T> held{nullptr};

  // Expands the quadtree towards a target.
  void expand_towards(sf::Vector2f target) {
    bool is_left = target.x < left;
    bool is_up = target.y < top;

    std::unique_ptr<Quadtree<T>> new_child{new Quadtree<T>};
    new_child->top_left.swap(top_left);
    new_child->top_right.swap(top_right);
    new_child->bottom_left.swap(bottom_left);
    new_child->bottom_right.swap(bottom_right);
    new_child->left = left;
    new_child->right = right;
    new_child->top = top;
    new_child->bottom = bottom;

    // Recalculate bounds
    if (is_left)
      left -= right - left;
    else
      right += right - left;

    if (is_up)
      top -= bottom - top;
    else
      bottom += bottom - top;

    new_child->held.swap(held);

    // Set child to correct position
    if (is_left && is_up)
      bottom_right.swap(new_child);
    else if (is_left && !is_up)
      top_right.swap(new_child);
    else if (!is_left && is_up)
      bottom_left.swap(new_child);
    else if (!is_left && !is_up)
      top_left.swap(new_child);
  }

  bool is_in_bounds(const sf::Vector2f item) const {
    return item.x >= left && item.x < right && item.y >= top && item.y < bottom;
  }

  void add_to_child(std::unique_ptr<T> &&item) {
    sf::Vector2f pos = get_position(*item);

    float mid_x = (left + right) / 2.0f;
    float mid_y = (top + bottom) / 2.0f;

    // Figure out where to put the new item, creating that subnode if necessary.
    if (pos.y < mid_y) {
      if (pos.x < mid_x) {
        if (!top_left)
          top_left = std::make_unique<Quadtree<T>>();
        top_left->left = left;
        top_left->right = mid_x;
        top_left->top = top;
        top_left->bottom = mid_y;
        top_left->add_item(std::move(item));
      } else {
        if (!top_right)
          top_right = std::make_unique<Quadtree<T>>();
        top_right->left = mid_x;
        top_right->right = right;
        top_right->top = top;
        top_right->bottom = mid_y;
        top_right->add_item(std::move(item));
      }
    } else {
      if (pos.x < mid_x) {
        if (!bottom_left)
          bottom_left = std::make_unique<Quadtree<T>>();
        bottom_left->left = left;
        bottom_left->right = mid_x;
        bottom_left->top = mid_y;
        bottom_left->bottom = bottom;
        bottom_left->add_item(std::move(item));
      } else {
        if (!bottom_right)
          bottom_right = std::make_unique<Quadtree<T>>();
        bottom_right->left = mid_x;
        bottom_right->right = right;
        bottom_right->top = mid_y;
        bottom_right->bottom = bottom;
        bottom_right->add_item(std::move(item));
      }
    }
  }

  void find_in_radius_helper(sf::Vector2f center, float radius,
                             std::vector<T *> &result) {
    // We want to early return if the search circle is out of range. (Otherwise
    // we visit every single node)
    float dx = std::max(0.0f, std::max(center.x - right, left - center.x));
    float dy = std::max(0.0f, std::max(center.y - bottom, top - center.y));
    if (dx * dx + dy * dy > radius * radius) {
      return; // Early exit if outside radius
    }

    if (held) {
      sf::Vector2f pos = get_position(*held);
      float dist_sq = (pos - center).lengthSquared();
      if (dist_sq <= radius * radius) {
        result.push_back(held.get());
      }
    }

    // We then recursively search children (Preorder traversal)
    if (top_left) {
      top_left->find_in_radius_helper(center, radius, result);
    }
    if (top_right) {
      top_right->find_in_radius_helper(center, radius, result);
    }
    if (bottom_left) {
      bottom_left->find_in_radius_helper(center, radius, result);
    }
    if (bottom_right) {
      bottom_right->find_in_radius_helper(center, radius, result);
    }
  }

public:
  void add_item(std::unique_ptr<T> &&item) {
    const sf::Vector2f pos = get_position(*item);
    while (!is_in_bounds(pos))
      expand_towards(pos);

    if (held) {
      add_to_child(std::move(item));
    } else {
      held = std::move(item);
    }
  }

  void add_item(T &&item) { add_item(std::make_unique<T>(std::move(item))); }

  std::vector<T *> find_in_radius(sf::Vector2f center, float radius) {
    std::vector<T *> result;
    find_in_radius_helper(center, radius, result);
    return result;
  }
};
