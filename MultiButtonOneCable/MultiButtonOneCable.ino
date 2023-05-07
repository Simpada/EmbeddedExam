void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {

  int value = analogRead(A2);
  if (value >= 750 && value < 1000) {
    Serial.println("Button1");
  } else if (value >= 650 && value < 750) {
    Serial.println("Button2");
  } else if (value >= 480 && value < 650) {
    Serial.println("Button3");
  } else if (value < 480) {
    Serial.println("Button4");
  } 

}
