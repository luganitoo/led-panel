//////////WIFI//////////
// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "UAIFAI";
const char* password = "em313mar";

// Set web server port number to 80
//WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String Col0State = "off";
String Col1State = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
//////////WIFI//////////

//////////FASTLED//////////
#include <FastLED.h>
#define LED_PIN     2
#define NUM_LEDS    300
#define NUM_ROWS    5
CRGB leds[NUM_LEDS];
CRGB red = CRGB(255, 0, 0);
CRGB orange = CRGB(255, 128, 0);
CRGB green = CRGB(0, 255, 0);
CRGB blue = CRGB(0, 0, 255);
CRGB violet = CRGB(128, 0, 255);
CRGB black = CRGB(0, 0, 0);

#define UPDATES_PER_SECOND 900
//////////FASTLED//////////


//////////VARIABLES//////////
int analogPin = A0;
int strobePin = 4;
int resetPin = 5;

int spectrumValue[5];
int spectrumValueEven[5];
int spectrumValueOdd[5];
int filter = 220;
int i, j;
//////////VARIABLES//////////

#include <FS.h>
#include <ESPAsyncWebServer.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setup() {
    Serial.begin(9600);

    pinMode(analogPin, INPUT);
    pinMode(strobePin, OUTPUT);
    pinMode(resetPin, OUTPUT);

    // MSGEQ
    digitalWrite(resetPin, LOW);
    digitalWrite(strobePin, HIGH);

    //////////FASTLED//////////
    delay( 3000 );
    FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  255 );
    //////////FASTLED//////////

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //server.begin();

    // File system
    SPIFFS.begin();

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String());
    });

    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/style.css", "text/css");
    });

    // Route to load style.css file
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/script.js", "text/javascript");
    });

      // Route to set GPIO to HIGH
    server.on("/music", HTTP_GET, [](AsyncWebServerRequest *request){
      playMusic();    
      request->send(SPIFFS, "/index.html", String());
    });

    server.begin();

}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /col0/on") >= 0) {
              Serial.println("COL-0 on");
              Col0State = "on";
              ledColumn(0, 30, red);
              FastLED.show();
              FastLED.delay(1);
            } else if (header.indexOf("GET /col0/off") >= 0) {
              Serial.println("COL-0 off");
              Col0State = "off";
              ledColumn(0, 0, red);
              FastLED.show();
              FastLED.delay(1);
            } else if (header.indexOf("GET /col1/on") >= 0) {
              Serial.println("COL-1 on");
              Col1State = "on";
              ledColumn(1, 30, red);
              FastLED.show();
              FastLED.delay(1);
            } else if (header.indexOf("GET /col1/off") >= 0) {
              Serial.println("COL-1 off");
              Col1State = "off";
              ledColumn(1, 0, red);
              FastLED.show();
              FastLED.delay(1);
            }
            
            // // Display the HTML web page
            // client.println("<!DOCTYPE html><html>");
            // client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            // client.println("<link rel=\"icon\" href=\"data:,\">");
            // // CSS to style the on/off buttons 
            // // Feel free to change the background-color and font-size attributes to fit your preferences
            // client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            // client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            // client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            // client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // // Web Page Heading
            // client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // // Display current state, and ON/OFF buttons for GPIO 5  
            // client.println("<p>Col 0 " + Col0State + "</p>");
            // // If the Col0State is off, it displays the ON button       
            // if (Col0State=="off") {
            //   client.println("<p><a href=\"/col0/on\"><button class=\"button\">ON</button></a></p>");
            // } else {
            //   client.println("<p><a href=\"/col0/off\"><button class=\"button button2\">OFF</button></a></p>");
            // } 
               
            // // Display current state, and ON/OFF buttons for GPIO 4  
            // client.println("<p>Col 1 " + Col1State + "</p>");
            // // If the Col1State is off, it displays the ON button       
            // if (Col1State=="off") {
            //   client.println("<p><a href=\"/col1/on\"><button class=\"button\">ON</button></a></p>");
            // } else {
            //   client.println("<p><a href=\"/col1/off\"><button class=\"button button2\">OFF</button></a></p>");
            // }
            // client.println("</body></html>");
            
            // // The HTTP response ends with another blank line
            // client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}


// LED CONTROL FUNCTION
void ledColumn(int colNum, int spectrumValue, CRGB color){
  
  int shifter = colNum * 30; // GET THE SHIFT BY THE COLUMN NUMBER
  spectrumValue = spectrumValue + shifter; // SHIFT THE VALUES TO COMPARRE TO THE CURRENT COLUMN
  // EVEN (UPWARD)
  if (colNum % 2 == 0){
    for(int i = shifter; i < 30+shifter; i++){ // LOOP THROUGH THE COLUMN LEDS
      if(i < spectrumValue){ // CHECK IF THE LED NUMBER IS LOWER THAN THE SPECTRUMVALUE
        leds[i] = color;
      }
      else{ // TURN IT OFF IF IT IS NOT
        leds[i] = CRGB::Black;
      }
    }
  }
  
  // ODD (DOWNWARD)
  else{
    for(int i = shifter; i < 30+shifter; i++){ // LOOP THROUGH THE COLUMN LEDS
      if(i > spectrumValue){ // CHECK IF THE LED NUMBER IS BIGGER THAN THE SPECTRUMVALUE
        leds[i] = color;

      }
      else{ // TURN IT OFF IF IT IS NOT
        leds[i] = CRGB::Black;
        
      }
    }
  }
}


void playMusic(){
  //MSGEQ7
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  for (i = 0; i < 5; i++) {
    delayMicroseconds(36);
    digitalWrite(strobePin, LOW);
    delayMicroseconds(18);
    spectrumValue[i] = analogRead(analogPin);
    spectrumValue[i] = constrain(spectrumValue[i], filter, 1023);
    spectrumValueEven[i] = map(spectrumValue[i], filter, 1023, 0, 30);
    spectrumValueOdd[i] = map(spectrumValue[i], filter, 1023, 30, 0);
    Serial.print(spectrumValueEven[i]);
    Serial.print("/");
    Serial.print(spectrumValueOdd[i]);
    Serial.print(" - ");
    delayMicroseconds(18);
    digitalWrite(strobePin, HIGH);
  }
  Serial.println();

  //////////+++FASTLED+++//////////
   ledColumn(0, spectrumValueEven[0], red);
   ledColumn(1, spectrumValueOdd[1], orange);
   ledColumn(2, spectrumValueEven[2], green);
   ledColumn(3, spectrumValueOdd[3], blue);
   ledColumn(4, spectrumValueEven[4], violet);
   ledColumn(5, spectrumValueOdd[4], violet);
   ledColumn(6, spectrumValueEven[3], blue);
   ledColumn(7, spectrumValueOdd[2], green);
   ledColumn(8, spectrumValueEven[1], orange);
   ledColumn(9, spectrumValueOdd[0], red);
   FastLED.show();
   FastLED.delay(1);
}
