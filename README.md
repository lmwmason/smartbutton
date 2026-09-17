# SmartButton

A dependency-free Arduino button library designed specifically for the MCU's
internal pull-up resistor (`INPUT_PULLUP`).

## Wiring

Connect the button directly between the input pin and GND:

    GPIO ---- [ button ] ---- GND

No external pull-up resistor is required. The library configures the pin with
`INPUT_PULLUP`, so released = HIGH and pressed = LOW.

## Install

Download `SmartButton.zip`, then in Arduino IDE use:

Sketch -> Include Library -> Add .ZIP Library...

Or copy the `SmartButton` directory into your Arduino `libraries` directory.

## Basic usage

```cpp
#include <SmartButton.h>

SmartButton button(2);

void setup() {
  button.begin();
}

void loop() {
  button.update();

  if (button.clicked()) {
    // one short click
  }

  if (button.doubleClicked()) {
    // two quick clicks
  }

  if (button.longPressed()) {
    // fires once when a long press begins
  }
}
```

`update()` must be called frequently. The library uses `millis()` and never
calls `delay()`.

## Events

| Function | Meaning |
|---|---|
| `pressed()` | Debounced transition to pressed |
| `released()` | Debounced transition to released |
| `clicked()` | Exactly one short click |
| `doubleClicked()` | Exactly two quick short clicks |
| `tripleClicked()` | Exactly three quick short clicks |
| `multiClicked(n)` | Exactly n quick short clicks |
| `longPressed()` | Long-press threshold crossed; fires once |
| `longReleased()` | Released after a long press |

Event methods are one-shot: once read as `true`, that event is consumed.

## State and timing

| Function | Meaning |
|---|---|
| `isPressed()` | Current debounced button state |
| `isHolding()` | Currently pressed and long-press threshold has passed |
| `heldFor(ms)` | Currently held for at least `ms` |
| `pressDuration()` | Current or most recent press duration in ms |
| `clickCount()` | Current/finalized click count |

`heldFor(ms)` is a state query, so it stays true while the button remains held.
If you need a one-shot action at 3 seconds, keep a boolean flag in your sketch.

## Configuration

Defaults:

```cpp
button.setDebounceTime(30);     // 30 ms
button.setLongPressTime(700);   // 700 ms
button.setMultiClickTime(350);  // 350 ms
```

The multi-click timeout intentionally delays a single-click event. After the
first short release, the library waits to see whether another click follows.
Therefore a single click is emitted only when the multi-click window expires.
This prevents a double-click from also triggering a single-click action.

## Multiple buttons

Each button has independent state:

```cpp
SmartButton left(2);
SmartButton right(3);

void setup() {
  left.begin();
  right.begin();
}

void loop() {
  left.update();
  right.update();

  if (left.clicked()) { }
  if (right.longPressed()) { }
}
```

## Notes

- Active-low / `INPUT_PULLUP` operation is intentional and fixed.
- No external dependencies.
- Avoid blocking code and long `delay()` calls, because button sampling depends
  on frequent `update()` calls.
