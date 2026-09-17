#include <SmartButton.h>

// Wiring:
//
// GPIO 2 ---- BUTTON ---- GND
//
// No external resistor is required.
// SmartButton automatically enables INPUT_PULLUP.

SmartButton button(2);

void setup() {
  Serial.begin(115200);

  button.begin();

  // Debounce time for mechanical switch noise.
  button.setDebounceTime(30);

  // Holding the button for at least 700 ms
  // is classified as a long press.
  button.setLongPressTime(700);

  // Time allowed between consecutive clicks.
  button.setMultiClickTime(350);
}

void loop() {
  // update() should be called as frequently as possible.
  button.update();


  // ==================================================
  // Physical edge events
  //
  // These are independent from gesture classification
  // and may occur together with a gesture.
  // ==================================================

  if (button.pressed()) {
    Serial.println("PRESSED");
  }

  if (button.released()) {
    Serial.println("RELEASED");
  }


  // ==================================================
  // Exclusive gesture events
  //
  // A single completed action produces exactly one
  // gesture from this group.
  // ==================================================

  if (button.clicked()) {
    Serial.println("CLICK");
  }

  else if (button.doubleClicked()) {
    Serial.println("DOUBLE CLICK");
  }

  else if (button.tripleClicked()) {
    Serial.println("TRIPLE CLICK");
  }

  else if (button.multiClicked(4)) {
    Serial.println("4 CLICKS");
  }

  else if (button.multiClicked(5)) {
    Serial.println("5 CLICKS");
  }

  else if (button.longPressed()) {
    Serial.println("LONG PRESS");
  }


  // ==================================================
  // State queries
  //
  // These are states, not gesture events.
  // They may remain true for multiple loop iterations.
  // ==================================================

  if (button.isPressed()) {
    // The button is currently pressed.
  }

  if (button.isHolding()) {
    // The button is currently in the long-press state.
  }

  if (button.heldFor(3000)) {
    // The button has currently been held for at least 3 seconds.
    // This remains true until the button is released.
  }

  uint32_t duration = button.pressDuration();

  // duration contains the current press duration while pressed,
  // or the duration of the most recent press after release.
}
