#include "SmartButton.h"

SmartButton::SmartButton(uint8_t pin) : _pin(pin) {}

void SmartButton::begin() {
  // INPUT_PULLUP: button must connect GPIO to GND.
  pinMode(_pin, INPUT_PULLUP);

  bool nowPressed = (digitalRead(_pin) == LOW);
  _rawPressed = nowPressed;
  _stablePressed = nowPressed;
  _rawChangedAt = millis();

  if (nowPressed) _pressedAt = millis();
}

void SmartButton::clearTransientEvents() {
  _evPressed = false;
  _evReleased = false;
  _evClick = false;
  _evDouble = false;
  _evTriple = false;
  _evLongPressed = false;
  _evLongReleased = false;
  _completedClicks = 0;
}

void SmartButton::update() {
  clearTransientEvents();
  const uint32_t now = millis();
  const bool raw = (digitalRead(_pin) == LOW);

  // Debounce raw transitions.
  if (raw != _rawPressed) {
    _rawPressed = raw;
    _rawChangedAt = now;
  }

  if (_stablePressed != _rawPressed &&
      (uint32_t)(now - _rawChangedAt) >= _debounceMs) {
    _stablePressed = _rawPressed;

    if (_stablePressed) {
      _pressedAt = now;
      _longActive = false;
      _evPressed = true;
    } else {
      _releasedAt = now;
      _evReleased = true;

      if (_longActive) {
        _evLongReleased = true;
        _longActive = false;
        // Long presses are not counted as clicks.
        _pendingClicks = 0;
      } else {
        // A short press/release becomes part of a click sequence.
        if (_pendingClicks < 255) ++_pendingClicks;
      }
    }
  }

  // Long press begins once after threshold.
  if (_stablePressed && !_longActive &&
      (uint32_t)(now - _pressedAt) >= _longPressMs) {
    _longActive = true;
    _evLongPressed = true;
    // Prevent clicks preceding/overlapping a long hold from being finalized.
    _pendingClicks = 0;
  }

  // Finalize short-click sequence only after no further click arrives.
  // This avoids firing single-click before we know it isn't a double-click.
  if (!_stablePressed && _pendingClicks > 0 &&
      (uint32_t)(now - _releasedAt) >= _multiClickMs) {
    _completedClicks = _pendingClicks;
    _pendingClicks = 0;

    if (_completedClicks == 1) _evClick = true;
    if (_completedClicks == 2) _evDouble = true;
    if (_completedClicks == 3) _evTriple = true;
  }
}

bool SmartButton::consume(bool &flag) {
  if (!flag) return false;
  flag = false;
  return true;
}

bool SmartButton::pressed()       { return consume(_evPressed); }
bool SmartButton::released()      { return consume(_evReleased); }
bool SmartButton::clicked()       { return consume(_evClick); }
bool SmartButton::doubleClicked() { return consume(_evDouble); }
bool SmartButton::tripleClicked() { return consume(_evTriple); }
bool SmartButton::longPressed()   { return consume(_evLongPressed); }
bool SmartButton::longReleased()  { return consume(_evLongReleased); }

bool SmartButton::multiClicked(uint8_t count) {
  if (_completedClicks != count || count == 0) return false;
  _completedClicks = 0;
  if (count == 1) _evClick = false;
  if (count == 2) _evDouble = false;
  if (count == 3) _evTriple = false;
  return true;
}

bool SmartButton::isPressed() const { return _stablePressed; }
bool SmartButton::isHolding() const { return _stablePressed && _longActive; }

bool SmartButton::heldFor(uint32_t ms) const {
  return _stablePressed && (uint32_t)(millis() - _pressedAt) >= ms;
}

uint32_t SmartButton::pressDuration() const {
  if (_stablePressed) return (uint32_t)(millis() - _pressedAt);
  return (uint32_t)(_releasedAt - _pressedAt);
}

uint8_t SmartButton::clickCount() const {
  return _completedClicks ? _completedClicks : _pendingClicks;
}

void SmartButton::setDebounceTime(uint16_t ms) { _debounceMs = ms; }
void SmartButton::setLongPressTime(uint16_t ms) { _longPressMs = ms; }
void SmartButton::setMultiClickTime(uint16_t ms) { _multiClickMs = ms; }
