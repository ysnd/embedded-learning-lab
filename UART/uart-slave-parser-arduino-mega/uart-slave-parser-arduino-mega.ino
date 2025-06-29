String command = "";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Ready to receive from ESP32...");
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available()) {
    char c = Serial1.read();
    command += c;

    if(c == '\n') {
      command.trim();
      Serial.print("Received: ");
      Serial.print(command);
    if (command == "LED ON") {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial1.println("LED turned ON");
    }
    else if (command == "LED OFF") {
      digitalWrite(LED_BUILTIN, LOW);
      Serial1.println("LED Turned OFF");
    }
    else {
      Serial1.println("Unknown Command");
    }
    command = "";
    }
  }
}
