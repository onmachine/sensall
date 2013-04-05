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

EthernetServer server(80);


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
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

}

void loop() {
    // listen for incoming clients
  EthernetClient serverClient = server.available();
  if (serverClient) {
    //Serial.println("new client");
    Serial.print('\n');
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    char request[60]; // handle up to 60 chars
    memset(request, 0, 60);
    int requestIndex = 0;
    boolean firstLine = true;
    //const char garageDoorRequest[] = "GET /garageDoor HTTP/1.1";
    boolean validRequest = false;
    
    
    while (serverClient.connected()) {
      
      if (serverClient.available()) {
        char c = serverClient.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          serverClient.println("HTTP/1.1 200 OK");
          serverClient.println("Content-Type: text/html");
          serverClient.println("Connection: close");
          serverClient.println();
          serverClient.println("<!DOCTYPE HTML>");
          serverClient.println("<html>");
          if (validRequest) {
            serverClient.print("Valid Request for: ");
            serverClient.print(request);
            serverClient.print(" has been initiated!");
          }
          else {
            serverClient.print("Request for: ");
            serverClient.print(request);
            serverClient.print(" failed to match a valid sensor");
          }
          serverClient.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          // if this is still the first line
          if (firstLine) {
            // debug
            //Serial.print("Request: ");
            //Serial.println(request);
            
            // if the request is for something I know, broadcast the status request on serial
            // TODO: remove hardcoding and pass off to a parser function.
            if (strcmp(request, "GET /garageDoor HTTP/1.1") == 0) {
              delay(1000);
              Serial.println("^{id:garageDoor;action:status}");
              validRequest = true;
            }
            firstLine = false;  // no more first line
          }
          
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          if (firstLine) {
            request[requestIndex] = c;
            requestIndex++;
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    serverClient.stop();
    //Serial.println("client disonnected");
  }

  
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
