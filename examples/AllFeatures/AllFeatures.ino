#include <SmartButton.h>

SmartButton button(2);

void setup() {
  Serial.begin(115200);

  // Wiring:
  // GPIO 2 ---- BUTTON ---- GND
  // No external resistor is required.
  button.begin();

  button.setDebounceTime(30);
  button.setLongPressTime(700);
  button.setMultiClickTime(350);
}

void loop() {
  // Call this as often as possible. Do not put long delay() calls in loop().
  button.update();

  if (button.pressed())
    Serial.println("Pressed");

  if (button.released())
    Serial.println("Released");

  if (button.clicked())
    Serial.println("Single click");

  if (button.doubleClicked())
    Serial.println("Double click");

  if (button.tripleClicked())
    Serial.println("Triple click");

  // Example for 4 or more clicks:
  if (button.multiClicked(4))
    Serial.println("4 clicks");

  if (button.longPressed())
    Serial.println("Long press started");

  if (button.isHolding()) {
    // True continuously after the long-press threshold.
  }

  if (button.heldFor(3000)) {
    // True continuously while held for at least 3 seconds.
    // Add your own one-shot guard if the action should happen only once.
  }

  if (button.longReleased()) {
    Serial.print("Long press released after ");
    Serial.print(button.pressDuration());
    Serial.println(" ms");
  }
}
