#include "SmartButton.h"

SmartButton::SmartButton(uint8_t pin)
  : _pin(pin) {
}

void SmartButton::begin() {
  // Use the MCU's internal pull-up resistor.
  // Wiring: GPIO ---- BUTTON ---- GND
  pinMode(_pin, INPUT_PULLUP);

  const bool current = (digitalRead(_pin) == LOW);

  _rawPressed = current;
  _stablePressed = current;
  _rawChangedAt = millis();

  if (current) {
    _pressedAt = millis();
  }
}

bool SmartButton::consumeEdge(bool &flag) {
  if (!flag) {
    return false;
  }

  flag = false;
  return true;
}

void SmartButton::setGesture(
  GestureType type,
  uint8_t count
) {
  // Never overwrite a gesture that has not been consumed yet.
  if (_gesture != GESTURE_NONE) {
    return;
  }

  _gesture = type;
  _gestureCount = count;
}

bool SmartButton::consumeGesture(
  GestureType type,
  uint8_t count
) {
  if (_gesture != type) {
    return false;
  }

  if (
    type == GESTURE_MULTI &&
    _gestureCount != count
  ) {
    return false;
  }

  // Completely consume the classified gesture.
  // The same physical action cannot be reported as another gesture.
  _gesture = GESTURE_NONE;
  _gestureCount = 0;

  return true;
}

void SmartButton::update() {
  const uint32_t now = millis();

  // INPUT_PULLUP uses active-low logic.
  // LOW means pressed and HIGH means released.
  const bool raw = (digitalRead(_pin) == LOW);

  // Detect a raw electrical state change.
  if (raw != _rawPressed) {
    _rawPressed = raw;
    _rawChangedAt = now;
  }

  // Apply debounce before accepting the new state.
  if (
    _stablePressed != _rawPressed &&
    (uint32_t)(now - _rawChangedAt) >= _debounceMs
  ) {
    _stablePressed = _rawPressed;

    if (_stablePressed) {
      // A new physical press has started.
      _pressedAt = now;
      _longActive = false;
      _evPressed = true;
    } else {
      // The physical button has been released.
      _releasedAt = now;
      _evReleased = true;

      if (_longActive) {
        // The action was already classified as a long press.
        // Releasing it must not generate a normal click.
        _longActive = false;
        _pendingClicks = 0;
      } else {
        // Add this short press to the current click sequence.
        if (_pendingClicks < 255) {
          ++_pendingClicks;
        }
      }
    }
  }

  // Classify a long press once the threshold is reached.
  if (
    _stablePressed &&
    !_longActive &&
    (uint32_t)(now - _pressedAt) >= _longPressMs
  ) {
    _longActive = true;

    // A long press is its own exclusive gesture.
    // Discard any pending short-click sequence.
    _pendingClicks = 0;

    setGesture(GESTURE_LONG);
  }

  // Finalize a short-click sequence only after the multi-click
  // timeout expires. This delay is required to distinguish a
  // single click from a double, triple, or multi-click.
  if (
    !_stablePressed &&
    !_longActive &&
    _pendingClicks > 0 &&
    (uint32_t)(now - _releasedAt) >= _multiClickMs
  ) {
    const uint8_t count = _pendingClicks;

    _pendingClicks = 0;

    if (count == 1) {
      setGesture(GESTURE_CLICK, 1);
    }
    else if (count == 2) {
      setGesture(GESTURE_DOUBLE, 2);
    }
    else if (count == 3) {
      setGesture(GESTURE_TRIPLE, 3);
    }
    else {
      setGesture(GESTURE_MULTI, count);
    }
  }
}


// ==================================================
// Physical edge events
// ==================================================

bool SmartButton::pressed() {
  return consumeEdge(_evPressed);
}

bool SmartButton::released() {
  return consumeEdge(_evReleased);
}


// ==================================================
// Mutually exclusive gesture events
// ==================================================

bool SmartButton::clicked() {
  return consumeGesture(GESTURE_CLICK);
}

bool SmartButton::doubleClicked() {
  return consumeGesture(GESTURE_DOUBLE);
}

bool SmartButton::tripleClicked() {
  return consumeGesture(GESTURE_TRIPLE);
}

bool SmartButton::multiClicked(uint8_t count) {
  // Counts 1, 2, and 3 have dedicated gesture methods.
  // multiClicked() is intentionally reserved for 4 or more clicks
  // to prevent multiple APIs from matching the same gesture.
  if (count < 4) {
    return false;
  }

  return consumeGesture(
    GESTURE_MULTI,
    count
  );
}

bool SmartButton::longPressed() {
  return consumeGesture(GESTURE_LONG);
}


// ==================================================
// State and timing
// ==================================================

bool SmartButton::isPressed() const {
  return _stablePressed;
}

bool SmartButton::isHolding() const {
  return (
    _stablePressed &&
    _longActive
  );
}

bool SmartButton::heldFor(uint32_t ms) const {
  if (!_stablePressed) {
    return false;
  }

  return (
    (uint32_t)(millis() - _pressedAt) >= ms
  );
}

uint32_t SmartButton::pressDuration() const {
  if (_stablePressed) {
    return (
      uint32_t
    )(millis() - _pressedAt);
  }

  return (
    uint32_t
  )(_releasedAt - _pressedAt);
}


// ==================================================
// Configuration
// ==================================================

void SmartButton::setDebounceTime(uint16_t ms) {
  _debounceMs = ms;
}

void SmartButton::setLongPressTime(uint16_t ms) {
  _longPressMs = ms;
}

void SmartButton::setMultiClickTime(uint16_t ms) {
  _multiClickMs = ms;
}
