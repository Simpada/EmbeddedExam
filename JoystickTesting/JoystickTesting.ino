
#include <ezButton.h>

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define SW_PIN   2  // Arduino pin connected to SW  pin

ezButton button(SW_PIN);

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int bValue = 0; // To store value of the button

void setup() {
  Serial.begin(9600) ;
  button.setDebounceTime(20);
}

void loop() {

  button.loop(); // MUST call the loop() function first

  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  bValue = button.getState();

    if (button.isPressed()) {
    Serial.println("The button is pressed");
  }

  if (button.isReleased()) {
    Serial.println("The button is released");
  }


  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.println(yValue);
  delay(200);
}

/*
Base x = 520
  0 towards wires
  1023 away from wires
Base y = 502
  0 right of wires
  1023 left of wires
*/