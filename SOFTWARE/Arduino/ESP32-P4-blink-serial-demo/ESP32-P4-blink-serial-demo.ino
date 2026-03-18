//Simple demo that blinks the LED
//and reports its status over the serial

#define LED_PIN 2

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  delay(100);  // small delay for USB enumeration
  Serial.println("Serial ready!");
}

void loop() {
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED ON");
  delay(1500);

  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED OFF");
  delay(1500);
}
