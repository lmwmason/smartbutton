#pragma once
#include <Arduino.h>

class SmartButton {
public:
  explicit SmartButton(uint8_t pin);

  void begin();
  void update();

  // One-shot events: true once per event.
  bool pressed();
  bool released();
  bool clicked();
  bool doubleClicked();
  bool tripleClicked();
  bool multiClicked(uint8_t count);
  bool longPressed();
  bool longReleased();

  // State / timing.
  bool isPressed() const;
  bool isHolding() const;
  bool heldFor(uint32_t ms) const;
  uint32_t pressDuration() const;
  uint8_t clickCount() const;

  // Configuration.
  void setDebounceTime(uint16_t ms);
  void setLongPressTime(uint16_t ms);
  void setMultiClickTime(uint16_t ms);

private:
  uint8_t _pin;

  bool _rawPressed = false;
  bool _stablePressed = false;
  bool _longActive = false;

  uint32_t _rawChangedAt = 0;
  uint32_t _pressedAt = 0;
  uint32_t _releasedAt = 0;

  uint16_t _debounceMs = 30;
  uint16_t _longPressMs = 700;
  uint16_t _multiClickMs = 350;

  uint8_t _pendingClicks = 0;
  uint8_t _completedClicks = 0;

  bool _evPressed = false;
  bool _evReleased = false;
  bool _evClick = false;
  bool _evDouble = false;
  bool _evTriple = false;
  bool _evLongPressed = false;
  bool _evLongReleased = false;

  void clearTransientEvents();
  static bool consume(bool &flag);
};
