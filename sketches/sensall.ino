#include <SPI.h>
#include <Ethernet.h>

// MAC address
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// Ethernet Shield
//byte mac[] = {  
//  0x90, 0xA2, 0xDA, 0x0D, 0x3F, 0xD1 };

// Ethernet Uno
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0D, 0x60, 0x15 };

char serverName[] = "api.pushingbox.com";
const char devid[] = "v56F4F2A48FCB259"; //Scenario
const char sensorID[] = "garageDoor";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;


// xbee serial setup
String packet = String("");
boolean isPacket = false; // flag to track if we are looking at a packet
boolean packetComplete = false;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // reserve 200 bytes for the inputString:
  packet.reserve(200);


  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("ready");

}

void loop() {
  if (packetComplete == true) {
    boolean didSend = parsePacket();
    if (didSend == false) {
      Serial.println("failure");
    }
    packetComplete = false;
  }

  //delay(10);
}

void serialEvent() {

  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read();

    //find the beginning of a packet
    if (inChar == '{') {
      isPacket = true;
      packet = "";
    }

    // if it is a packet, add characters
    if (isPacket) {
      packet += inChar;
      //Serial.print("isPacket");
    }

    // find the end of the packet and kick off a response
    if (inChar == '}') {
      packetComplete = true;
      isPacket = false;
    }
  }
}

boolean parsePacket() {
  boolean result = false;
  if (packet.substring(23,29) == "closed") {
    Serial.println("closed");
    result = sendStatus("closed");

  }

  if (packet.substring(23,27) == "open") {
    Serial.println("open");
    result = sendStatus("open");

  }

  return result;

}

boolean sendStatus( char *state)
{
  boolean result = false;

  Serial.println("connecting ...");
  if (client.connect(serverName, 80) ) {
    client.print("GET /pushingbox?devid=");
    client.print(devid);
    client.print("&sensorID=");
    client.print(sensorID);
    client.print("&status=");
    client.print(state);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.println("User-Agent: Arduino");
    client.println();
  }
  else {
    Serial.println("Connection failed");
  }
  
 // response string
  if (client.connected()) {
    Serial.println("Connected");
    
    if(client.find("HTTP/1.1") && client.find("200 OK") ){
      Serial.println("Success");
      result = true; 
    }
    else
       Serial.println("Dropping connection - no 200 OK");
  }
  else {
    Serial.println("Disconnected"); 
  }
  client.stop();
  client.flush();
  
  return result;
}

