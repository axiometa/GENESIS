#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN     38
#define NUM_LEDS    135
#define BRIGHTNESS  200
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define WIFI_SSID "Axiometa" // Replace with your WiFi SSID
#define WIFI_PASS "Axiometa" // Replace with your WiFi Password

WebServer server(80);

uint8_t red = 0;
uint8_t green = 0;
uint8_t blue = 0;

// HTML for the web interface
const char htmlPage[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>RGB LED Control</title>
    <style>
        body { font-family: Arial; text-align: center; }
        input[type=range] { width: 80%; }
    </style>
</head>
<body>
    <h1>RGB LED Control</h1>
    <label for="red">Red</label><br>
    <input type="range" id="red" name="red" min="0" max="255" oninput="updateColor()"><br><br>
    <label for="green">Green</label><br>
    <input type="range" id="green" name="green" min="0" max="255" oninput="updateColor()"><br><br>
    <label for="blue">Blue</label><br>
    <input type="range" id="blue" name="blue" min="0" max="255" oninput="updateColor()"><br><br>
    <script>
        function updateColor() {
            let r = document.getElementById('red').value;
            let g = document.getElementById('green').value;
            let b = document.getElementById('blue').value;
            let xhttp = new XMLHttpRequest();
            xhttp.open("GET", `/setcolor?r=${r}&g=${g}&b=${b}`, true);
            xhttp.send();
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
    // Initialize FastLED
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    // Initialize serial monitor
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup web server routes
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", htmlPage);
    });

    server.on("/setcolor", HTTP_GET, []() {
        if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
            red = server.arg("r").toInt();
            green = server.arg("g").toInt();
            blue = server.arg("b").toInt();
            fill_solid(leds, NUM_LEDS, CRGB(red, green, blue));
            FastLED.show();
            server.send(200, "text/plain", "Color updated");
        } else {
            server.send(400, "text/plain", "Missing parameters");
        }
    });

    // Start the web server
    server.begin();
    Serial.println("Web server started.");
    Serial.println("Access the server at your ESP32's IP address.");
}

void loop() {
    server.handleClient();
}
