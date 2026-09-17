#pragma once
#include <Arduino.h>

class SmartButton {
public:
  explicit SmartButton(uint8_t pin);

  void begin();
  void update();

  // Physical edge events.
  // These are independent from gesture classification.
  bool pressed();
  bool released();

  // Mutually exclusive gesture events.
  // A single action produces exactly one of these gestures.
  bool clicked();
  bool doubleClicked();
  bool tripleClicked();
  bool multiClicked(uint8_t count);
  bool longPressed();

  // State and timing.
  bool isPressed() const;
  bool isHolding() const;
  bool heldFor(uint32_t ms) const;
  uint32_t pressDuration() const;

  // Configuration.
  void setDebounceTime(uint16_t ms);
  void setLongPressTime(uint16_t ms);
  void setMultiClickTime(uint16_t ms);

private:
  enum GestureType : uint8_t {
    GESTURE_NONE,
    GESTURE_CLICK,
    GESTURE_DOUBLE,
    GESTURE_TRIPLE,
    GESTURE_MULTI,
    GESTURE_LONG
  };

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

  bool _evPressed = false;
  bool _evReleased = false;

  GestureType _gesture = GESTURE_NONE;
  uint8_t _gestureCount = 0;

  bool consumeEdge(bool &flag);
  bool consumeGesture(GestureType type, uint8_t count = 0);
  void setGesture(GestureType type, uint8_t count = 0);
};
