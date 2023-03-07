void setup() {
  // put your setup code here, to run once:
  // LED0〜3の端子を出力にする
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED0, HIGH); // LED0をON(HIGH)
  delay(100);
  digitalWrite(LED1, HIGH); // LED1をON(HIGH)
  delay(100);
  digitalWrite(LED2, HIGH); // LED2をON(HIGH)
  delay(100);
  digitalWrite(LED3, HIGH); // LED3をON(HIGH)
  delay(100);

  digitalWrite(LED0, LOW);  // LED0をOFF(LOW)
  delay(100);
  digitalWrite(LED1, LOW);  // LED1をOFF(LOW)
  delay(100);
  digitalWrite(LED2, LOW);  // LED2をOFF(LOW)
  delay(100);
  digitalWrite(LED3, LOW);  // LED3をOFF(LOW)
  delay(100);
}
