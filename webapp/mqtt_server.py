#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import json
from settings import *

def on_connect(client, userdata, flags, rc):
    print("Connected with code: {}".format(str(rc)))
    topics = ["bicykyle"]
    [client.subscribe(topic) for topic in topics]


# used only for debugging
# global settings is not shared between files, must be redefined in webapp
def on_message(client, userdata, msg):
    global current_data
    # print("message received")
    # print(str(msg.payload.decode("utf-8")))

    point_data = json.loads(msg.payload.decode("utf-8"))
    # for d in ["time", "mph"]:
    #     current_data[d].append(point_data[d])

    current_data["millis"].append(point_data["time"])
    current_data["x"].append(point_data["time"])
    current_data["y"].append(point_data["mph"])

    print("updated")
    print(current_data)

    # conn = create_connection()
    # add_timestamp(conn, location_id)
    # conn.close()


def mqtt_init():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("localhost", 1883, 60)
    # must be done in webapp.py
    # client.loop_forever()
    return(client)


def main():
    client = mqtt_init()
    client.loop_forever()
    if KeyboardInterrupt:
        client.disconnect()
        client.loop_stop()


if __name__ == '__main__':
    main()
