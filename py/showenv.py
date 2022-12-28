import os

clientId=os.environ.get("MQTT_CLIENT")
user=os.environ.get("MQTT_USER")
passwd=os.environ.get("MQTT_PASSWD")
host= os.environ.get("MQTT_HOST")
port=int(os.environ.get("MQTT_PORT"))
print("MQTT_HOST: %s, MQTT_PORT:%d"%(host,port))
print("MQTT_CLIENT: %s"%(clientId))
print("MQTT_USER: %s"%(user))
print("MQTT_PASSWORD: %s"%(passwd))
