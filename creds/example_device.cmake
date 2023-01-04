# This file contains all the device credentials.

# To use this file, set the appropriate credentials then rename it to "device.cmake"
# The program will not compile without correctly naming this file

# Configure WiFi credentials
message("Configuring WiFi Credentials.....")
if(NOT WIFI_SSID_SET)
    set(WIFI_SSID_SET "WiFiName") # Put your own WiFi SSID
    message("WiFi SSID set: ${WIFI_SSID_SET}")
endif()

if(NOT WIFI_PASSWORD_SET)
    set(WIFI_PASSWORD_SET "MyPassword") # Put your own WiFi Password
    message("WiFi PASSWORD set: *******")
endif()

# Configure MQTT credentials
message("Configuring MQTT Credentials.....")
if(NOT MQTT_CLIENT_SET)
    set(MQTT_CLIENT_SET "device1") # Put your own MQTT Client ID
    message("MQTT Client ID set: ${MQTT_CLIENT_SET}")
endif()

if(NOT MQTT_USER_SET)
    set(MQTT_USER_SET "rackadev") # Put your own MQTT USER
    message("MQTT USER set: *******")
endif()

if(NOT MQTT_PASSWD_SET)
    set(MQTT_PASSWD_SET "MyPassword") # Put your own MQTT password
    message("MQTT Password set: *******")
endif()

if(NOT MQTT_HOST_SET)
    set(MQTT_HOST_SET "www.mqtt.com") # Put your own MQTT HOST
    message("MQTT HOST set: *******")
endif()

if(NOT MQTT_PORT_SET)
    set(MQTT_PORT_SET 9090) # Put your own MQTT PORT
    message("MQTT PORT set: ${MQTT_PORT_SET}")
endif()
