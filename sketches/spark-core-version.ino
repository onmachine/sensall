/**
 ******************************************************************************
 *  
 * Details: https://github.com/onmachine/sensall/
 * 
 ******************************************************************************

*/


#include "rest_client.h" // see: https://github.com/llad/spark-restclient/

int doorSensor = D0;  // sensor location
int switchState;         // status of the switch: LOW = closed; HIGH = Open
int previousSwitchState;  // previous status of the switch
int counter = 0; // seconds counter to monitor how long door is open
int warningDelay = 60; // setting for how long to wait before sending a warning
int ttl = 86400; // Time to Live for events (required but not sure how it benefits me)
int warningSent = 0; // track if we sent a warning for the open door

char serverName[] = "api.pushingbox.com";  // use pushingbox to send alerts or update google
char devid[] = "yourPushingBoxDeviceIDforStatus"; //Scenario for update of status
char alertDevid[] = "yourPushingBoxDeviceIDforAlers"; // for the alert functionality
char allClearDevID[] = "yourPushingBoxDeviceIDforAllClear"; // for the all clear message on PushingBox
char sensorID[] = "garageDoor";  // sensor identifier for logging
char topic[] = "yourTopic-garageDoor"; // topic for the spark publish service

RestClient client = RestClient(serverName);

void setup() {
    
    pinMode(doorSensor, INPUT_PULLUP);
    Serial.begin(9600);
    switchState = digitalRead( doorSensor ); //read the sensor initially
    previousSwitchState = switchState; //set the previous state
    pushStatus(""); //send the intial status
    
    //Spark API so we can call functions and/or variables
    Spark.function("pushStatus", pushStatus);
    Spark.function("sendAlert", sendAlert);
    Spark.function("sendAllClear", sendAllClear);
    Spark.variable("switchState", &switchState, INT);

}

void loop() {

    // check the switch state and store value
    switchState = digitalRead(doorSensor);
    
    
    // If there was a change in state, send the state
    if (switchState != previousSwitchState) {
        if (switchState == LOW) {
            Spark.publish( topic, "closed", ttl, PRIVATE );
            sendStatus( "closed" );
        }
        
        else {
            Spark.publish( topic, "open", ttl, PRIVATE );
            sendStatus( "open" );
        }
        previousSwitchState = switchState; // keeps state from getting sent until next change

    }
    

    // Warning if door is open too long
    if ( switchState == HIGH ){
        
        if ( counter == warningDelay ) {
            
            Spark.publish(topic, "openWarning", ttl, PRIVATE);
            sendStatus("openWarning");
            sendAlert("");
            warningSent = 1;
        }
        counter += 1;
    }
    
    else {
        // send an all clear alert if the openWarning was sent but now closed (LOW)
        if (warningSent == 1) {
            Spark.publish(topic, "AllClear", ttl, PRIVATE);
            sendStatus("AllClear");
            sendAllClear("");
            warningSent = 0; // reset the warning sent indicator after AllClear
        }
        counter = 0;  // reset the counter if door is closed
    }
  
    delay(1000);  // bounce control and timer

}


// Triggers the Status update on PushingBox
void sendStatus(String state){
    
    // construct the query string and convert to a char array
    String path_string = String ("/pushingbox?");
    path_string = path_string + "devid=" + devid + "&sensorID=" + sensorID + "&status=" + state;
    char path_char[path_string.length()];
    path_string.toCharArray(path_char, path_string.length()+1);
    
    // make the request
    String response = "";
    int statusCode = client.get(path_char, &response);

}
    

// Triggers the OpenWarning email on PushingBox
int sendAlert(String args) {
    
    String path_string = String ("/pushingbox?");
    path_string = path_string + "devid=" + alertDevid;
    char path_char[path_string.length()];
    path_string.toCharArray(path_char, path_string.length()+1);
    
    String response = "";
    int statusCode = client.get(path_char, &response);

    return 1;

}

// Triggers the allClear email on PushingBox
int sendAllClear(String args) {
    

    String path_string = String ("/pushingbox?");
    path_string = path_string + "devid=" + allClearDevID;
    char path_char[path_string.length()];
    path_string.toCharArray(path_char, path_string.length()+1);
    
    String response = "";
    int statusCode = client.get(path_char, &response);

    return 1;

}

// Push the status -- used to instantly check the status.
int pushStatus(String args) {
    
    if (switchState == LOW) {
        Spark.publish(topic, "closedPushed", ttl, PRIVATE);
        sendStatus("closed");
    }
    else {
        Spark.publish(topic, "openPushed", ttl, PRIVATE);
        sendStatus("open");
    }
    
    return 1;
}
