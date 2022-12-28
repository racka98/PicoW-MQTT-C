# IoT Device Off
# Jon Durrant - 3-Oct-2022
#
# Send message {'on': False}
# To topic TNG/<<device>>/TPC/LED/req 
# Takes <<device>> as first parameter on command line
# Experts following env variables to be set
# MQTT_USER
# MQTT_PASSWD
# MQTT_HOST
# MQTT_PORT

import paho.mqtt.client as paho
from paho import mqtt
import json
import time
import sys
import os



#Check we have a target as a parameter on command line
if (len(sys.argv) != 2):
    print("Require target ID as parater")
    sys.exit()
targetId = sys.argv[1]

# Grab environment variables
clientId=os.environ.get("MQTT_CLIENT")
user=os.environ.get("MQTT_USER")
passwd=os.environ.get("MQTT_PASSWD")
host= os.environ.get("MQTT_HOST")
port=int(os.environ.get("MQTT_PORT"))
print("MQTT %s:%d"%(host,port))
if (len(clientId) > 6):
   print("Client: %s..."%clientId[0:4])
else: 
   print("Client: %s..."%clientId)   
if (len(user) > 6):
   print("User: %s..."%user[0:4])
else:
    print("User: %s"%user) 

#Set up topic name
subTopic = "TNG/" + targetId + "/#"
ledTopic = "TNG/" + targetId + "/TPC/LED/req"

# The callback for when the client receives a CONNACK response from the broker.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Rcv topic=" +msg.topic+" msg="+str(msg.payload))

# Connect to the broker
client = paho.Client(client_id=clientId)
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
client.username_pw_set(username=user, password=passwd)
client.on_connect = on_connect
client.on_message = on_message
client.connect(host, port, 60)

#Maintain connection loop in thread
client.loop_start()

#Subscribe to the Topic so we can see what was sent
client.subscribe( subTopic )

#Publish on Message
j = {'on': False}
p = json.dumps(j)
print("Publishing LED message %s"%p)
infot = client.publish(ledTopic, p,retain=False, qos=1)
infot.wait_for_publish()


#Stay running so we can see message arrive
time.sleep(30)