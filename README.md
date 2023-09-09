# esp32_mqtt
## ESP32 sensor reading 
- ph on pin 34
- tds on pin 35
- temperature on pin 5
- turbidity on pin 36

## Transfer data using mqtt
## serve data using python Flask 

### esp.ino:
- change constants required for wifi
- change constants required for mqtt broker
- upload esp.ino on esp32

### server.py:
- install the requirements
- change constants required for mqtt broker
- run on server using python3
- api available on http://ip_address/average/{topic}
