#ifndef MQTT_H_
#define MQTT_H_

// TODO: move to include
using namespace std;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
    // used to act on messages from the MQTT server
    for (int i=0; i < length; i++) {
        char receivedChar = (char)payload[i];
        Serial.print(receivedChar);
        if (receivedChar == '0'){
            // turn off
        }
        else if (receivedChar == '1') {
            // turn back on
        }
    }
    Serial.println();
}


void reconnect() {
    // attempt to connect to MQTT
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // assign random client ID to avoid collisions
        // adapted from: https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
        String clientID = "bicykyle";
        clientID += String(random(0xffff), HEX);

        if (client.connect(clientID.c_str())) {
            Serial.println("connected");
            client.subscribe(MQTT_TOPIC);
        } else {
            // keep trying to connect
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

#endif // MQTT_H_
