import paho.mqtt.client as mqtt
from flask import Flask, jsonify

app = Flask(__name__)

# MQTT broker details
broker_address = "localhost"
broker_port = 1883  # Default MQTT port
username = "gemastik"
password = "ezet"

# Global variables for temperature data
topic_data = {}
window_size = 10  # Number of data points to consider for average

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code {rc}")
    client.subscribe('temperature')
    client.subscribe('ph')
    client.subscribe('tds')
    client.subscribe('turbidity')

def on_message(client, userdata, msg):
    global topic_data

    # Extract topic name and payload
    topic = msg.topic
    value = float(msg.payload.decode())
    print(f"Received {topic} reading: {value}")

    # Initialize data buffer for the topic if not exists
    if topic not in topic_data:
        topic_data[topic] = []

    # Add data point to the data buffer
    topic_data[topic].append(value)

    # Check if the data buffer exceeds the window size
    if len(topic_data[topic]) > window_size:
        # Calculate the average over the specified window
        average_value = sum(topic_data[topic]) / len(topic_data[topic])
        print(f"Average {topic} over {window_size} readings: {average_value}")

        # Clear the data buffer to start a new window
        topic_data[topic] = []

@app.route('/average/<string:topic>', methods=['GET'])
def get_average(topic):
    if topic not in topic_data:
        return jsonify({'error': f'Data for topic {topic} not available'}), 404

    if len(topic_data[topic]) == 0:
        return jsonify({'error': f'No data available for topic {topic}'}), 404

    # Calculate the average for the specified topic
    average_value = sum(topic_data[topic]) / len(topic_data[topic])
    return jsonify({f'average_{topic}': average_value})


if __name__ == '__main__':
    # Start the MQTT client
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(username, password)
    client.connect(broker_address, broker_port, 60)
    client.loop_start()

    # Start the Flask app
    app.run(host='0.0.0.0', port=80)