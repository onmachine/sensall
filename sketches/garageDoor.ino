
// Constants
const int switchPin = 2;     // the number of the pushswitch pin
const int ledPin =  13;      // the number of the LED pin
// the request string we are looking for to respond
const char statusRequest[] = "{id:garageDoor;action:status}";
const char openMessage[] = "{id:garageDoor;message:open}";
const char closedMessage[] = "{id:garageDoor;message:closed}";
const int maxPacketSize = 29; // max needed to get status

// Variables
char packet[maxPacketSize+1];
int packetIndex = 0;
boolean isRequest = false; // flag to track if it is a request
boolean isPacket = false; // flag to track if we are looking at a packet
int switchState;         // status of the switch: LOW = closed; HIGH = Open
int previousSwitchState;  // previous status of the switch

void setup() {
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT); // initialize the LED pin as an output:
  pinMode(switchPin, INPUT); // initialize the pushswitch pin as an input:
  digitalWrite(switchPin, HIGH); // Activate internal pullup resistor
  previousSwitchState = LOW;  // Opposite to send the initial state on startup
}

void loop(){
  // check the switch state and store value
  switchState = digitalRead(switchPin);
  
  if (switchState != previousSwitchState) {
    sendStatus(switchState);
    digitalWrite(ledPin, switchState); // Light up if the door is open (HIGH)
    previousSwitchState = switchState;
  }
  
  delay(1000);  // keeps it from getting jumpy

}

int sendStatus(int state) {
    if (state == LOW) {      
      Serial.println(closedMessage);
    } 
    else {
      Serial.println(openMessage);
    }
}

void serialEvent() {
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read();
    
    if (inChar == '^') {
      isRequest = true;
      isPacket = false;
      memset(packet, 0, maxPacketSize+1);
      packetIndex = 0;
    }

    //find the beginning of a packet
    if (isRequest && inChar == '{') {
      isPacket = true;
    }
    
    // if it is a packet, add characters
    if (isRequest && isPacket && packetIndex < maxPacketSize) {
       packet[packetIndex] = inChar;
       packetIndex++;
      //Serial.print("isPacket");
    }

    // find the end of the packet and kick off a response
    // isRequest && inChar == '}' && 
    if (isRequest && inChar == '}' && strcmp(packet, statusRequest)  == 0) {
      sendStatus(digitalRead(switchPin));
      isPacket = false;
      isRequest = false;
    }
    // Debug stuff
    //Serial.print("Index: ");
    //Serial.println(packetIndex);
    //Serial.print("packet: ");
    //Serial.println(packet);
  }
}
