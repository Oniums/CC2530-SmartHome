# CC2530-SmartHome
初学者的大学课设：   
基于CC2530的室内环境检测系统    
三个CC2530:一个协调器，两个终端节点   
使用Z-Stack协议栈进行Zigbee组网通信    
协调器通过串口与ESP32通信传输传感数据，ESP32作为网关进行无线通信发送MQTT报文给订阅方     
DHT11温湿度传感器，BH1750光强传感器，MQ2烟雾传感器    
舵机控制

没什么参考价值，发布只是为了查看方便

硬件实物图     
![image](https://user-images.githubusercontent.com/55424074/166281119-f94afe99-cc25-4b04-bc81-808c9f69d687.png)
