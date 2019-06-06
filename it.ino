#include <IRremote.h>

int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

boolean recording = true;
decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (recording) {
    if (irrecv.decode(&results)) {
      Serial.println("IR code recorded!");
      //irrecv.resume(); // Receive the next value
      Serial.print("Recorded ");
      Serial.print(results.rawlen);
      Serial.println(" intervals.");
      recording = false;
    }
  } else {
    // replay mode
    Serial.println("Sending recorded IR signal!");
    irsend.sendRaw((unsigned int*) results.rawbuf, results.rawlen, 38);
    Serial.print(results.rawlen);
    Serial.println(" intervals.");
    delay(2000);
  }
}
