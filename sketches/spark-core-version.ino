int doorSensor = D0;  // sensor location
int switchState;         // status of the switch: LOW = closed; HIGH = Open
int previousSwitchState;  // previous status of the switch

char serverName[] = "api.pushingbox.com";
const char devid[] = "yourPushingboxDevid"; //Scenario
const char sensorID[] = "garageDoor";

TCPClient client;


void setup() {
    pinMode(doorSensor, INPUT_PULLUP);
    previousSwitchState = LOW;  // assume it is closed
    
    Serial.begin(9600);
    
}

void loop() {
    
  // check the switch state and store value
  switchState = digitalRead(doorSensor);
  
  if (switchState != previousSwitchState) {
      if (switchState == LOW) {
        Spark.publish("yourtopic", "closed", 60, PRIVATE);
        sendStatus("closed");
      }
      else {
          Spark.publish("yourtopic", "open", 60, PRIVATE);
          sendStatus("open");
      }
    previousSwitchState = switchState;
  }
  
  delay(1000);  // keeps it from getting jumpy

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
