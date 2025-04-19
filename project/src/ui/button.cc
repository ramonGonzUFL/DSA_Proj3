#include "./button.hh"

bool UIComponent::contains(const sf::Vector2f &point) const {
  return point.x >= position.x && point.x <= position.x + size.x &&
         point.y >= position.y && point.y <= position.y + size.y;
}

void UIContainer::addComponent(std::shared_ptr<UIComponent> component) {
  children.push_back(component);
}

void UIContainer::removeComponent(std::shared_ptr<UIComponent> component) {
  // Find the component in our children vector and remove it
  auto it = std::find(children.begin(), children.end(), component);
  if (it != children.end()) {
    children.erase(it);
  }
}

void UIContainer::draw(sf::RenderWindow &window) const {
  for (const auto &child : children) {
    if (child->isVisible()) {
      child->draw(window);
    }
  }
}

Panel::Panel(sf::Vector2f size, sf::Color color) {
  this->size = size;
  background.setSize(size);
  background.setFillColor(color);
}

void Panel::draw(sf::RenderWindow &window) const { window.draw(background); }

void Panel::setPosition(const sf::Vector2f &pos) {
  position = pos;
  background.setPosition(pos);
}

void Panel::setSize(const sf::Vector2f &s) {
  size = s;
  background.setSize(s);
}

void Panel::setColor(sf::Color color) { background.setFillColor(color); }

sf::Color Panel::getColor() const { return background.getFillColor(); }

bool UIContainer::handleEvent(const sf::Event &) { return false; }

void UIComponent::setPosition(const sf::Vector2f &pos) { position = pos; }

sf::Vector2f UIComponent::getPosition() const { return position; }

void UIComponent::setSize(const sf::Vector2f &s) { size = s; }

sf::Vector2f UIComponent::getSize() const { return size; }

void UIComponent::setVisible(bool v) { visible = v; }

bool UIComponent::isVisible() const { return visible; }

void UIComponent::setEnabled(bool e) { enabled = e; }

bool UIComponent::isEnabled() const { return enabled; }

sf::Vector2f UIComponent::right(float margin) const {
  return sf::Vector2f(position.x + size.x + margin, position.y);
}

sf::Vector2f UIComponent::below(float margin) const {
  return sf::Vector2f(position.x, position.y + size.y + margin);
}

Button::Button(const std::string &buttonText, sf::Vector2f position,
               const sf::Font &font, sf::Vector2f size, unsigned int fontSize,
               sf::Color backgroundColor, sf::Color textColor)
    : text(font, buttonText, fontSize) {

  this->position = position;
  normalColor = backgroundColor;
  text.setFillColor(textColor);

  // Calculate size if not provided
  sf::FloatRect textBounds = text.getLocalBounds();
  if (size.x == 0 && size.y == 0) {
    size = {textBounds.size.x + 30, 60};
  }
  this->size = size;

  // Setup rectangle
  rect.setPosition(position);
  rect.setSize(size);
  rect.setFillColor(normalColor);

  // Center text
  text.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f,
                  textBounds.position.y + textBounds.size.y / 2.0f});
  text.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
}

void Button::draw(sf::RenderWindow &window) const {
  window.draw(rect);
  window.draw(text);
}

void Button::setPosition(const sf::Vector2f &pos) {
  position = pos;
  rect.setPosition(pos);

  // Update text position
  sf::FloatRect textBounds = text.getLocalBounds();
  text.setPosition({position.x + size.x / 2.0f - textBounds.size.x / 2.0f,
                    position.y + size.y / 2.0f - textBounds.size.y / 2.0f});
}

void Button::setSize(const sf::Vector2f &s) {
  size = s;
  rect.setSize(s);

  // Update text position
  sf::FloatRect textBounds = text.getLocalBounds();
  text.setPosition({position.x + size.x / 2.0f - textBounds.size.x / 2.0f,
                    position.y + size.y / 2.0f - textBounds.size.y / 2.0f});
}

bool Button::wasClicked(const sf::Vector2i &mousePosition) const {
  if (!enabled)
    return false;

  sf::FloatRect bounds = rect.getGlobalBounds();
  return bounds.contains({static_cast<float>(mousePosition.x),
                          static_cast<float>(mousePosition.y)});
}

void Button::setOnClick(std::function<void()> callback) { onClick = callback; }

bool Button::handleEvent(const sf::Event &event) {
  if (!enabled)
    return false;

  if (event.is<sf::Event::MouseButtonPressed>()) {
    auto &mouseEvent = *event.getIf<sf::Event::MouseButtonPressed>();
    if (mouseEvent.button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos(mouseEvent.position.x, mouseEvent.position.y);
      if (contains(mousePos)) {
        currentState = State::Pressed;
        rect.setFillColor(pressedColor);
        return true;
      }
    }
  } else if (event.is<sf::Event::MouseButtonReleased>()) {
    auto &mouseEvent = *event.getIf<sf::Event::MouseButtonReleased>();
    if (mouseEvent.button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos(mouseEvent.position.x, mouseEvent.position.y);
      if (currentState == State::Pressed && contains(mousePos)) {
        currentState = State::Hover;
        rect.setFillColor(hoverColor);
        if (onClick)
          onClick();
        return true;
      }
      currentState = State::Normal;
      rect.setFillColor(normalColor);
    }
  } else if (event.is<sf::Event::MouseMoved>()) {
    auto &mouseEvent = *event.getIf<sf::Event::MouseMoved>();
    sf::Vector2f mousePos(mouseEvent.position.x, mouseEvent.position.y);
    if (contains(mousePos)) {
      if (currentState != State::Pressed) {
        currentState = State::Hover;
        rect.setFillColor(hoverColor);
      }
    } else {
      if (currentState != State::Pressed) {
        currentState = State::Normal;
        rect.setFillColor(normalColor);
      }
    }
  }

  return false;
}

void Button::setEnabled(bool e) {
  enabled = e;
  if (!enabled) {
    currentState = State::Disabled;
    rect.setFillColor(disabledColor);
  } else {
    currentState = State::Normal;
    rect.setFillColor(normalColor);
  }
}

const sf::Font &Button::getFont() const { return text.getFont(); }

Label::Label(const std::string &labelText, sf::Vector2f position,
             const sf::Font &font, sf::Vector2f size, unsigned int fontSize,
             sf::Color backgroundColor, sf::Color textColor)
    : text(font, labelText, fontSize) {

  this->position = position;
  text.setFillColor(textColor);

  sf::FloatRect textBounds = text.getLocalBounds();
  if (size.x == 0 && size.y == 0) {
    size = {textBounds.size.x + 30, 60};
  }
  this->size = size;

  // Text Centering
  text.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f,
                  textBounds.position.y + textBounds.size.y / 2.0f});
  text.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});

  rect.setPosition(position);
  rect.setSize(size);
  rect.setFillColor(backgroundColor);
}

void Label::draw(sf::RenderWindow &window) const {
  window.draw(rect);
  window.draw(text);
}

void Label::setPosition(const sf::Vector2f &pos) {
  position = pos;
  rect.setPosition(pos);

  text.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
}

void Label::setSize(const sf::Vector2f &s) {
  size = s;
  rect.setSize(s);

  text.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
}

void Label::setText(const std::string &labelText) {
  text.setString(labelText);

  // Recalculate text bounds and re-center
  sf::FloatRect textBounds = text.getLocalBounds();
  text.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f,
                  textBounds.position.y + textBounds.size.y / 2.0f});
  text.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
}

std::string Label::getText() const { return text.getString(); }

void Label::setTextColor(sf::Color color) { text.setFillColor(color); }

sf::Color Label::getTextColor() const { return text.getFillColor(); }

void Label::setBackgroundColor(sf::Color color) { rect.setFillColor(color); }

sf::Color Label::getBackgroundColor() const { return rect.getFillColor(); }

Backing::Backing(const std::vector<const UIComponent *> &uiComponents,
                 float margin, sf::Color backgroundColor) {

  if (uiComponents.empty()) {
    position = {0, 0};
    size = {0, 0};
    background.setPosition(position);
    background.setSize(size);
    background.setFillColor(backgroundColor);
    this->margin = margin;
    return;
  }

  components = uiComponents;
  this->margin = margin;

  // Find the bounding box for all components
  float minX = uiComponents[0]->getPosition().x;
  float minY = uiComponents[0]->getPosition().y;
  float maxX = minX + uiComponents[0]->getSize().x;
  float maxY = minY + uiComponents[0]->getSize().y;

  for (const auto &component : uiComponents) {
    float compLeft = component->getPosition().x;
    float compTop = component->getPosition().y;
    float compRight = compLeft + component->getSize().x;
    float compBottom = compTop + component->getSize().y;

    minX = std::min(minX, compLeft);
    minY = std::min(minY, compTop);
    maxX = std::max(maxX, compRight);
    maxY = std::max(maxY, compBottom);
  }

  // Set position and size with margins
  position = {minX - margin, minY - margin};
  size = {maxX - minX + 2 * margin, maxY - minY + 2 * margin};

  // Create a rectangle with margins around the components
  background.setPosition(position);
  background.setSize(size);
  background.setFillColor(backgroundColor);
}

void Backing::draw(sf::RenderWindow &window) const { window.draw(background); }

void Backing::setPosition(const sf::Vector2f &pos) {
  position = pos;
  background.setPosition(pos);
}

void Backing::setColor(sf::Color color) { background.setFillColor(color); }

Slider::Slider(sf::Vector2f position, float min, float max, float initial,
               const sf::Font &font, sf::Vector2f size,
               const std::string &label, sf::Color trackColor,
               sf::Color handleColor)
    : minValue(min), maxValue(max), currentValue(initial),
      valueText(font, "", 14), labelText(font, label, 14.0) {

  this->position = position;
  this->size = size;

  // Setup track
  track.setPosition(position);
  track.setSize(size);
  track.setFillColor(trackColor);

  // Setup handle
  float handlePosition =
      (currentValue - minValue) / (maxValue - minValue) * size.x;
  handle.setSize(sf::Vector2f(20, size.y));
  handle.setOrigin({10, 5}); // Center handle on track
  handle.setPosition({position.x + handlePosition, position.y + size.y / 2});
  handle.setFillColor(handleColor);

  // Setup texts
  if (!label.empty()) {
    labelText.setFillColor(sf::Color::Black);
    labelText.setPosition({position.x, position.y - 20});
  }

  valueText.setString(std::to_string(static_cast<int>(currentValue)));
  valueText.setCharacterSize(14);
  valueText.setFillColor(sf::Color::Black);
  updateValueTextPosition();
}

void Slider::draw(sf::RenderWindow &window) const {
  window.draw(track);
  window.draw(handle);

  if (!labelText.getString().isEmpty()) {
    window.draw(labelText);
  }

  if (showValue) {
    window.draw(valueText);
  }
}

bool Slider::handleEvent(const sf::Event &event) {
  if (!enabled)
    return false;

  if (event.is<sf::Event::MouseButtonPressed>()) {
    auto &mouseEvent = *event.getIf<sf::Event::MouseButtonPressed>();
    if (mouseEvent.button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos(mouseEvent.position.x, mouseEvent.position.y);
      if (handle.getGlobalBounds().contains(mousePos)) {
        isDragging = true;
        return true;
      } else if (track.getGlobalBounds().contains(mousePos)) {
        // Jump to clicked position
        updatePosition(mousePos.x);
        isDragging = true;
        return true;
      }
    }
  } else if (event.is<sf::Event::MouseButtonReleased>()) {
    auto &mouseEvent = *event.getIf<sf::Event::MouseButtonReleased>();
    if (mouseEvent.button == sf::Mouse::Button::Left && isDragging) {
      isDragging = false;
      return true;
    }
  } else if (event.is<sf::Event::MouseMoved>()) {
    auto &mouseEvent = *event.getIf<sf::Event::MouseMoved>();
    if (isDragging) {
      updatePosition(mouseEvent.position.x);
      return true;
    }
  }

  return false;
}

void Slider::setPosition(const sf::Vector2f &pos) {
  sf::Vector2f offset = pos - position;
  position = pos;
  track.setPosition(position);
  handle.setPosition(handle.getPosition() + offset);

  if (!labelText.getString().isEmpty()) {
    labelText.setPosition({position.x, position.y - 20});
  }

  updateValueTextPosition();
}

float Slider::getValue() const { return currentValue; }

void Slider::setValue(float value) {
  currentValue = std::clamp(value, minValue, maxValue);
  float handlePosition =
      (currentValue - minValue) / (maxValue - minValue) * size.x;
  handle.setPosition({position.x + handlePosition, position.y + size.y / 2});
  valueText.setString(std::to_string(static_cast<int>(currentValue)));
  updateValueTextPosition();
}

void Slider::setShowValue(bool show) { showValue = show; }

void Slider::setColors(sf::Color trackColor, sf::Color handleColor) {
  track.setFillColor(trackColor);
  handle.setFillColor(handleColor);
}

void Slider::updatePosition(float mouseX) {
  float newX = std::clamp(mouseX, position.x, position.x + size.x);
  float percentage = (newX - position.x) / size.x;
  currentValue = minValue + percentage * (maxValue - minValue);
  handle.setPosition({newX, position.y + size.y / 2});
  valueText.setString(std::to_string(static_cast<int>(currentValue)));
  updateValueTextPosition();
}

void Slider::updateValueTextPosition() {
  valueText.setPosition(
      {handle.getPosition().x - valueText.getLocalBounds().size.x / 2,
       position.y + size.y + 5});
}
