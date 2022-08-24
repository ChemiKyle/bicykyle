#!/usr/bin/env python3

from flask import Flask, render_template, request, jsonify
from datetime import datetime
from settings import *
import mqtt_server
import copy
import json

app = Flask(__name__)

@app.route('/', methods=['GET'])
def home():
    # empty maps serving as schemas
    global current_data

    # clear data on page load
    # TODO: implement toggle between kph and mph
    current_data = {
        "millis": [],
        "x": [],
        "y": [],
        "name": "MPH"
    }

    return render_template(
        'index.html',
        current_data = current_data
    )


@app.route('/ajax', methods=['GET'])
def respond():
    global current_data

    if (len(current_data["x"]) == 0):
        # HACK: duplicate last send
        # TODO: error handling on frontend
        return jsonify(current_data)

    cached_data = copy.deepcopy(current_data)
    cached_data = jsonify(cached_data)

    current_data = {
        "millis": [],
        "x": [],
        "y": [],
        "name": "MPH"
    }

    return(cached_data)


def on_message(client, userdata, msg):
    global current_data
    # print("message received")
    # print(str(msg.payload.decode("utf-8")))

    point_data = json.loads(msg.payload.decode("utf-8"))

    # TODO: compute power from mph and user-defined trainer
    # TODO: find someone on the internet who has shared raw /data/ for trainers

    current_data["millis"].append(point_data["time"])
    current_data["x"].append(datetime.now())
    current_data["y"].append(point_data["mph"])

    # TODO: store in a db if velocity > 0


if __name__ == '__main__':
    global current_data

    client = mqtt_server.mqtt_init()
    client.on_message = on_message
    client.loop_start()

    app.run(host = "0.0.0.0", debug = True)
