import json
import pymongo
import paho.mqtt.client as mqtt
from datetime import datetime

# MongoDB configuration
mongo_client = pymongo.MongoClient("mongodb://localhost:27017/")
db = mongo_client["smartheater"]
collection = db["sensor_readings"]

# MQTT configuration
mqtt_broker_address = '34.142.152.99'
mqtt_topic = 'smartheater'

def data_preprocess(message):
 return json.loads(message)

def on_message(client, userdata, message):
 payload = message.payload.decode('utf-8')
 print(f'Received message: {payload}')
 # Convert MQTT timestamp to datetime
 timestamp = datetime.utcnow() # Use current UTC time
 datetime_obj = timestamp.strftime("%Y-%m-%dT%H:%M:%S")
 # Process the payload and insert into MongoDB with proper timestamp
 document = {
  'timestamp': datetime_obj,
  'data': data_preprocess(payload)
 }
 collection.insert_one(document)
 print(f"Inserted document {document}")
 print('Data ingested into MongoDB')

client = mqtt.Client()
client.on_message = on_message

# Connect to MQTT broker
client.connect(mqtt_broker_address, 1883, 60)

# Subscribe to MQTT topic
client.subscribe(mqtt_topic)

# Start the MQTT loop
client.loop_forever()
