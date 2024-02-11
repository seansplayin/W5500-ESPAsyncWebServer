# W5500-ESPAsyncWebServer
W5500 Ethernet Adapter based version of ESPAsyncWebServer Supporting Websocket and asychronous webserver 

This sketch has been tested on Arduino IDE 2.2.1 with the ESP32-S3-DevkitC-1 MCU using Espressif board package 2.0.14. 
and uses a fork ESP_Async_WebServer by mathieucarbou available at https://github.com/mathieucarbou/ESPAsyncWebServer which was forked 
from https://github.com/yubox-node-org/ESPAsyncWebServer to include all concurrency fixes as of 02-10-24.
This might work with the master version of ESPAsycWebServer but I have not tested.

What it does W5500-ESPAsyncWebServer does?
Create a webpage with three buttons that when actuated will pull each buttons respective pin from low to high. This webserver 
supports multiple concurrently connected clients and button states are instantly updated on each clients webpage when changed.

