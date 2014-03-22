int doorSensor = D0;  // sensor location
int switchState;         // status of the switch: LOW = closed; HIGH = Open
int previousSwitchState;  // previous status of the switch
int counter = 0;

char serverName[] = "api.pushingbox.com";
char devid[] = "yourDevID for Google Form"; //Scenario for update of status
char alertDevid[] = "yourDevID for Email Alert"; // for the alert email scenario
char sensorID[] = "garageDoor";
char topic[] = "yourTopic"; //for spark publish

TCPClient client;

boolean lastConnected = false;                 // State of the connection last time through the main loop


void setup() {
    
    pinMode( doorSensor, INPUT_PULLUP );
    Serial.begin(9600);
    switchState = digitalRead( doorSensor ); //read the sensor initially
    previousSwitchState = switchState; //set the previous state
    pushStatus(""); //send the intial status
    
    //Spark API so we can call functions and/or variables
    Spark.function( "pushStatus", pushStatus );
    Spark.function( "sendAlert", sendAlert );
    Spark.variable( "switchState", &switchState, INT );

}

void loop() {

    // check the switch state and store value
    switchState = digitalRead(doorSensor);

    // Warnings for open doors at roughly 2,5 and 10 minutes
    if ( switchState == HIGH ){
        
        if (counter == 120 || 600 || 1200) {
            
            Spark.publish(topic, "openWarning", 60, PRIVATE);
            sendStatus("openWarning");
            sendAlert("");
        }
        counter += 1;
    }
    
    else counter = 0;
  
  
    if (switchState != previousSwitchState) {
        if (switchState == LOW) {
            Spark.publish( topic, "closed", 60, PRIVATE );
            sendStatus( "closed" );
        }
        
        else {
            Spark.publish( topic, "open", 60, PRIVATE );
            sendStatus( "open" );
        }
        previousSwitchState = switchState;

    }
    
    if (!client.connected() && lastConnected) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();
    }
    lastConnected = client.connected();
    

    delay(998);  // bounce control and timer, 1000ms is too much

}

void sendStatus(String state){
    client.stop();
    
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
    
}

// send email alert through pushingbox
int sendAlert(String args) {
    
    Serial.println("connecting ...");
    if (client.connect(serverName, 80) ) {
        client.print("GET /pushingbox?devid=");
        client.print(alertDevid);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverName);
        client.println("User-Agent: Arduino");
        client.println();
        
    }
    
    else {
        Serial.println("Connection failed");
    }
    
    return 1;

}

//  manually push the status to google form/spreadsheet
int pushStatus(String args) {
    
    if (switchState == LOW) {
        Spark.publish( topic, "closed", 60, PRIVATE );
        sendStatus( "closed" );
    }
    else {
        Spark.publish( topic, "open", 60, PRIVATE );
        sendStatus( "open" );
    }
    
    return 1;
}
