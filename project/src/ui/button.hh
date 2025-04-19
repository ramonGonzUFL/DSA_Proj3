#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Mouse.hpp>
#include <functional>
#include <memory>
#include <vector>

// Base UI Component
class UIComponent {
protected:
  sf::Vector2f position;
  sf::Vector2f size;
  bool visible = true;
  bool enabled = true;

public:
  virtual ~UIComponent() = default;

  virtual void draw(sf::RenderWindow &window) const = 0;

  virtual bool contains(const sf::Vector2f &point) const;

  virtual void setPosition(const sf::Vector2f &pos);
  virtual sf::Vector2f getPosition() const;

  virtual void setSize(const sf::Vector2f &s);
  virtual sf::Vector2f getSize() const;

  virtual void setVisible(bool v);
  virtual bool isVisible() const;

  virtual void setEnabled(bool e);
  virtual bool isEnabled() const;

  sf::Vector2f right(float margin = 20.0f) const;

  sf::Vector2f below(float margin = 20.0f) const;
};

// Container for UI components
class UIContainer : public UIComponent {
protected:
  std::vector<std::shared_ptr<UIComponent>> children;

public:
  void addComponent(std::shared_ptr<UIComponent> component);

  void removeComponent(std::shared_ptr<UIComponent> component);

  void draw(sf::RenderWindow &window) const override;

  virtual bool handleEvent(const sf::Event &);
};

// Panel Component
class Panel : public UIComponent {
protected:
  sf::RectangleShape background;
  float cornerRadius = 0.0f;

public:
  Panel(sf::Vector2f size = {0, 0}, sf::Color color = sf::Color(220, 220, 220));

  void draw(sf::RenderWindow &window) const override;

  void setPosition(const sf::Vector2f &pos) override;

  void setSize(const sf::Vector2f &s) override;

  void setColor(sf::Color color);

  sf::Color getColor() const;
};

// Button Implementation
class Button : public UIComponent {
protected:
  sf::Text text;
  sf::RectangleShape rect;
  std::function<void()> onClick;

  enum class State { Normal, Hover, Pressed, Disabled };
  State currentState = State::Normal;

  sf::Color normalColor = sf::Color(200, 200, 200);
  sf::Color hoverColor = sf::Color(220, 220, 220);
  sf::Color pressedColor = sf::Color(180, 180, 180);
  sf::Color disabledColor = sf::Color(150, 150, 150);

public:
  Button(const std::string &buttonText, sf::Vector2f position,
         const sf::Font &font, sf::Vector2f size = sf::Vector2f{0, 0},
         unsigned int fontSize = 30,
         sf::Color backgroundColor = sf::Color(200, 200, 200),
         sf::Color textColor = sf::Color::Black);

  void draw(sf::RenderWindow &window) const override;

  void setPosition(const sf::Vector2f &pos) override;

  void setSize(const sf::Vector2f &s) override;

  bool wasClicked(const sf::Vector2i &mousePosition) const;

  void setOnClick(std::function<void()> callback);

  bool handleEvent(const sf::Event &event);

  void setEnabled(bool e) override;

  // Get the underlying font for other UI elements to use
  const sf::Font &getFont() const;

  friend class Label;
  friend class Backing;
};

class Label : public UIComponent {
private:
  sf::Text text;
  sf::RectangleShape rect;

public:
  Label(const std::string &labelText, sf::Vector2f position,
        const sf::Font &font, sf::Vector2f size = sf::Vector2f{0, 0},
        unsigned int fontSize = 30,
        sf::Color backgroundColor = sf::Color(240, 240, 240),
        sf::Color textColor = sf::Color::Black);

  void draw(sf::RenderWindow &window) const override;

  void setPosition(const sf::Vector2f &pos) override;

  void setSize(const sf::Vector2f &s) override;

  void setText(const std::string &labelText);

  std::string getText() const;

  void setTextColor(sf::Color color);

  sf::Color getTextColor() const;

  void setBackgroundColor(sf::Color color);

  sf::Color getBackgroundColor() const;
};

class Backing : public UIComponent {
private:
  sf::RectangleShape background;
  std::vector<const UIComponent *> components;
  float margin;

public:
  Backing(const std::vector<const UIComponent *> &uiComponents,
          float margin = 20.0f,
          sf::Color backgroundColor = sf::Color(220, 220, 220));

  template <typename... Args>
  Backing(float margin, sf::Color backgroundColor, const UIComponent &first,
          const Args &...rest) {
    std::vector<const UIComponent *> components = {&first, &rest...};
    *this = Backing(components, margin, backgroundColor);
  }

  template <typename... Args>
  Backing(const UIComponent &first, const Args &...rest) {
    std::vector<const UIComponent *> components = {&first, &rest...};
    *this = Backing(components, 20.0f, sf::Color(220, 220, 220));
  }

  void draw(sf::RenderWindow &window) const override;

  void setPosition(const sf::Vector2f &pos) override;

  void setColor(sf::Color color);
};

// Slider class for numeric range selection
class Slider : public UIComponent {
private:
  sf::RectangleShape track;
  sf::RectangleShape handle;
  float minValue;
  float maxValue;
  float currentValue;
  bool isDragging = false;
  sf::Text valueText;
  sf::Text labelText;
  bool showValue = true;

public:
  Slider(sf::Vector2f position, float min, float max, float initial,
         const sf::Font &font, sf::Vector2f size = sf::Vector2f(200, 10),
         const std::string &label = "",
         sf::Color trackColor = sf::Color(180, 180, 180),
         sf::Color handleColor = sf::Color(100, 100, 100));

  void draw(sf::RenderWindow &window) const override;

  bool handleEvent(const sf::Event &event);

  void setPosition(const sf::Vector2f &pos) override;

  float getValue() const;

  void setValue(float value);

  void setShowValue(bool show);

  void setColors(sf::Color trackColor, sf::Color handleColor);

private:
  void updatePosition(float mouseX);

  void updateValueTextPosition();
};
