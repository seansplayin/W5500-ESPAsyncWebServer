// 02-10-24: compiles and runs successfully. sketch creates three buttons on the webpage that when activated
// will pull up it's respective pin (2, 42, 41) high when the button on the webpage is clicked. 
// properly

#include <SPI.h>
#include <WebServer_ESP32_SC_W5500.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

const int ledPin1 = 2;
bool ledState1 = false; // Initial state is off
const int ledPin2 = 42;
bool ledState2 = false; // Initial state is off
const int ledPin3 = 41;
bool ledState3 = false; // Initial state is off

#define W5500_MOSI 11 // W5500 Ethernet Adapter
#define W5500_MISO 13 // W5500 Ethernet Adapter
#define W5500_SCK 12  // W5500 Ethernet Adapter
#define W5500_SS 10   // W5500 Ethernet Adapter
#define W5500_INT 4   // Define the SPI2 (HSPI) pins for the W5500 Ethernet Adapter

// MAC address for your W5500 Ethernet Adapter
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // Replace with your MAC address

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
  <script>
    var ws;
    function connectWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + '/ws');
      ws.onmessage = function(event) {
        if (event.data === 'updateButtons') {
          location.reload();
        }
      };
    }
    connectWebSocket();
    function toggleCheckbox(element) {
      var xhr = new XMLHttpRequest();
try {
  // ... existing code

  // Add try-catch block
  try {
    var pin = parseInt(element.id);
    var state = parseInt(element.checked ? 1 : 0);
  } catch (error) {
    console.error("Error converting input:", error.message, "Input value:", element.id, element.checked);
    return; // Early exit if conversion fails
  }

  // ... rest of the function logic using pin and state
} catch (error) {
  // Handle other errors during the function
  console.error("Error in toggleCheckbox:", error.message);
}

      if (element.checked) {
        xhr.open("GET", "/update?output=" + element.id + "&state=1", true);
      } else {
        xhr.open("GET", "/update?output=" + element.id + "&state=0", true);
      }
      xhr.send();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
          if (xhr.status == 200) {
            console.log(xhr.responseText);
          } else {
    console.error("Error updating button state:", xhr.status);
  
         }
        }
      }
    }
  </script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    buttons += "<h4>Output - GPIO 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(ledPin1) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 42</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"42\" " + outputState(ledPin2) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 41</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"41\" " + outputState(ledPin3) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(int output)
{
  if (digitalRead(output))
  {
    return "checked";
  }
  else
  {
    return "";
  }
}

// Insert debugging statements to track the flow of execution

void handleWebSocketMessage(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  Serial.println("WebSocket message received");
  if (type == WS_EVT_DATA)
  {
    if (len > 0)
    {
      String msg = "";
      for (size_t i = 0; i < len; i++)
      {
        msg += (char)data[i];
      }
      Serial.println("WebSocket message content: " + msg); // Log WebSocket message content
      if (msg == "updateButtons")
      {
        server->textAll("updateButtons");
        Serial.println("updateButtons message sent");
      }
    }
  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Serial monitor initialized");

// Configure pin 2 as an output
  pinMode(ledPin1, OUTPUT);
  // Configure pin 42 as an output
  pinMode(ledPin2, OUTPUT);
  // Configure pin 41 as an output
  pinMode(ledPin3, OUTPUT);

  // Initialize SPI for the W5500 Ethernet adapter
  SPI.begin();
  Serial.println("SPI initialized");

  // Initialize the Ethernet adapter with the specified MAC address and pins
  ETH.begin(W5500_MISO, W5500_MOSI, W5500_SCK, W5500_SS, W5500_INT, 25, SPI3_HOST, mac);
  Serial.println("Ethernet initialized");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  Serial.println("Root route initialized");

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received update request");
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    try {
      inputMessage1 = request->getParam("output")->value();
      inputMessage2 = request->getParam("state")->value();

      // Add try-catch block
      try {
        int pin = inputMessage1.toInt();
        int state = inputMessage2.toInt();
        Serial.print("Received button state change request - Pin: ");
        Serial.print(pin);
        Serial.print(", New State: ");
        Serial.println(state);

        digitalWrite(pin, state);

        ws.textAll("updateButtons");
        Serial.println("updateButtons message sent");
        request->send(200, "text/plain", "OK");

        Serial.println("Button state updated");
        Serial.print("Button ID: ");
        Serial.println(pin);
        Serial.print("New state: ");
        Serial.println(state);
      } catch (const std::exception& e) {
        Serial.print("Error updating pin state: ");
        Serial.println(e.what());
        request->send(500, "text/plain", "Error updating button state");
        return;
      }
    } catch (const std::exception& e) {
      // Handle other errors during request processing
      Serial.print("Error handling request: ");
      Serial.println(e.what());
      request->send(500, "text/plain", "Internal server error");
      return;
    }

    Serial.print("Received output parameter: ");
    Serial.println(inputMessage1);
    Serial.print("Received state parameter: ");
    Serial.println(inputMessage2);
  });
  Serial.println("Update route initialized");

  // WebSocket events
  ws.onEvent(handleWebSocketMessage); // Declaration is here
  server.addHandler(&ws);
  Serial.println("WebSocket event handler initialized");

  // Start server
  server.begin();
  Serial.println("Server started");
}



void loop()
{
}
