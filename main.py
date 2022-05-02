from machine import UART,Pin,Timer
from umqtt.simple import MQTTClient
import network
import time
import dht
led=Pin(2,Pin.OUT)
uart=UART(2,baudrate=115200,bits=8,parity=0,rx=12,tx=13,stop=1,timeout=10)
dht=dht.DHT11(Pin(25))
temp=0
humid=0

def blink(i):
  for i in range (0,i):
    led.value(1)
    time.sleep(1)
    led.value(0)
    time.sleep(1)


def dht11(timerObject):
  global temp,humid
  dht.measure()
  humid=dht.humidity()
  temp=dht.temperature()

def sub_cb(topic, msg):
  uart.write(msg)

def connectWifi(ssid,passwd):
  global wlan
  wlan=network.WLAN(network.STA_IF)         #create a wlan object
  wlan.active(True)                         #Activate the network interface
  wlan.disconnect()                         #Disconnect the last connected WiFi
  wlan.connect(ssid,passwd)                 #connect wifi
  while(wlan.ifconfig()[0]=='0.0.0.0'):
    time.sleep(1)


T=Timer(1)
T.init(period=1000,mode=Timer.PERIODIC,callback=dht11)
try:
  connectWifi("user","password")
  client=MQTTClient("Esp32","ip",port=1883,user='user',password='password')
  client.set_callback(sub_cb)                    #set callback
  client.connect()
  print("MQTT Server connected")
  client.subscribe(b"/WECHAT")                        #client subscribes to a topic
  print("MQTT subscribe succeed")
  blink(2)
  while True:
    if(client.check_msg()):
      client.wait_msg()
    if(uart.any()):
      message="ESP32:{},{};{}".format(temp,humid,uart.read())
      client.publish(b"/ESP32",message)
      blink(1)

finally:
  if(client is not None):
    client.disconnect()
  wlan.disconnect()
  wlan.active(False)








