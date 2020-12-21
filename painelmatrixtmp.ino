//////////WIFI//////////
// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "UAIFAI";
const char* password = "em313mar";

// Set web server port number to 80
WiFiServer server(80);

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
    FastLED.setBrightness(  200 );
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
            
            // COLUMN CONTROL
            if (header.indexOf("GET /col0/on") >= 0) {
              Serial.println("COL-0 on");
              Col0State = "on";
              ledColumn(0, 30, red);
            } else if (header.indexOf("GET /col0/off") >= 0) {
              Serial.println("COL-0 off");
              Col0State = "off";
              ledColumn(0, 0, red);
            } else if (header.indexOf("GET /col1/on") >= 0) {
              Serial.println("COL-1 on");
              Col1State = "on";
              ledColumn(1, 0, red);
            } else if (header.indexOf("GET /col1/off") >= 0) {
              Serial.println("COL-1 off");
              Col1State = "off";
              ledColumn(1, 30, red);
            }
            

            // DISPLAY IT
            FastLED.show();
            FastLED.delay(1);


            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}");
            client.println("#matrix {
                color: #444;
                width: 300px;
                height: 200px;
                background: black;
                display: flex;
                flex-direction: column;
                flex-wrap: wrap;
                }
                .matrix-column {
                    display: flex;
                    flex-direction: column;
                    flex-wrap: wrap;
                }
                .matrix-cell {
                    background-color: black;
                    outline: 1px solid white;
                    float: left;
                    height: 20px;
                    width: 30px;
                }
                .center {
                    margin: auto;
                    padding: 10px;
                    display: flex;
                    justify-content: space-between;
                }
                .qcs {
                    padding: 14px;
                    margin: 2px;
                    border-radius: 14px;
                    display: inline-block;
                }
                #qcs-w {
                    padding-top: 30px;
                }

                
                .matrix-menu-btn {
                    height: 30px;
                    width: 100px;
                    color: white;
                    border: 1px solid white;
                    margin: 10px;
                    background: #575757;
                }
                .matrix-menu-btn > span {
                    line-height: 30px;
                }
            ");
            
            
            client.println("</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>LUGANO MATRIX EQ-5</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>Col 0 " + Col0State + "</p>");
            // If the Col0State is off, it displays the ON button       
            if (Col0State=="off") {
              client.println("<p><a href=\"/col0/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/col0/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>Col 1 " + Col1State + "</p>");
            // If the Col1State is off, it displays the ON button       
            if (Col1State=="off") {
              client.println("<p><a href=\"/col1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/col1/off\"><button class=\"button button2\">OFF</button></a></p>");
            }


            // MENU BUTTONS
            client.println("
                <div id=\"matrix-menu\" class=\"center\">
                ");
                if(matrixMenuBtn == "music"){ 
                     client.println("<div class=\"matrix-menu-btn active\" id=\"matrix-music\"><span>Music</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-all\"><span>All</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-col\"><span>Column</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-cell\"><span>Cell</span></div>
                     ");
                }
                else if(matrixMenuBtn == "all"){
                    client.println("<div class=\"matrix-menu-btn\" id=\"matrix-music\"><span>Music</span></div>
                                    <div class=\"matrix-menu-btn active\" id=\"matrix-all\"><span>All</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-col\"><span>Column</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-cell\"><span>Cell</span></div>
                     ");
                }
                else if(matrixMenuBtn == "col"){
                    client.println("<div class=\"matrix-menu-btn\" id=\"matrix-music\"><span>Music</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-all\"><span>All</span></div>
                                    <div class=\"matrix-menu-btn active\" id=\"matrix-col\"><span>Column</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-cell\"><span>Cell</span></div>
                     ");
                }
                else if(matrixMenuBtn == "cell"){
                    client.println("<div class=\"matrix-menu-btn\" id=\"matrix-music\"><span>Music</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-all\"><span>All</span></div>
                                    <div class=\"matrix-menu-btn\" id=\"matrix-col\"><span>Column</span></div>
                                    <div class=\"matrix-menu-btn active\" id=\"matrix-cell\"><span>Cell</span></div>
                     ");
                }
            client.println("</div>");

            // MATRIX
            client.println("
            <div id=\"matrix\" class=\"center\">
                <div class=\"matrix-column\" id=\"col-0\"> 
                  <div class=\"matrix-cell\" id=\"cel-09\"></div>
                  <div class=\"matrix-cell\" id=\"cel-08\"></div>
                  <div class=\"matrix-cell\" id=\"cel-07\"></div>
                  <div class=\"matrix-cell\" id=\"cel-06\"></div>
                  <div class=\"matrix-cell\" id=\"cel-05\"></div>
                  <div class=\"matrix-cell\" id=\"cel-04\"></div>
                  <div class=\"matrix-cell\" id=\"cel-03\"></div>
                  <div class=\"matrix-cell\" id=\"cel-02\"></div>
                  <div class=\"matrix-cell\" id=\"cel-01\"></div>
                  <div class=\"matrix-cell\" id=\"cel-00\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-1\">
                  <div class=\"matrix-cell\" id=\"cel-19\"></div>
                  <div class=\"matrix-cell\" id=\"cel-18\"></div>
                  <div class=\"matrix-cell\" id=\"cel-17\"></div>
                  <div class=\"matrix-cell\" id=\"cel-16\"></div>
                  <div class=\"matrix-cell\" id=\"cel-15\"></div>
                  <div class=\"matrix-cell\" id=\"cel-14\"></div>
                  <div class=\"matrix-cell\" id=\"cel-13\"></div>
                  <div class=\"matrix-cell\" id=\"cel-12\"></div>
                  <div class=\"matrix-cell\" id=\"cel-11\"></div>
                  <div class=\"matrix-cell\" id=\"cel-10\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-2>
                  <div class=\"matrix-cell\" id=\"cel-29\"></div>
                  <div class=\"matrix-cell\" id=\"cel-28\"></div>
                  <div class=\"matrix-cell\" id=\"cel-27\"></div>
                  <div class=\"matrix-cell\" id=\"cel-26\"></div>
                  <div class=\"matrix-cell\" id=\"cel-25\"></div>
                  <div class=\"matrix-cell\" id=\"cel-24\"></div>
                  <div class=\"matrix-cell\" id=\"cel-23\"></div>
                  <div class=\"matrix-cell\" id=\"cel-22\"></div>
                  <div class=\"matrix-cell\" id=\"cel-21\"></div>
                  <div class=\"matrix-cell\" id=\"cel-20\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-3\">
                  <div class=\"matrix-cell\" id=\"cel-39\"></div>
                  <div class=\"matrix-cell\" id=\"cel-38\"></div>
                  <div class=\"matrix-cell\" id=\"cel-37\"></div>
                  <div class=\"matrix-cell\" id=\"cel-36\"></div>
                  <div class=\"matrix-cell\" id=\"cel-35\"></div>
                  <div class=\"matrix-cell\" id=\"cel-34\"></div>
                  <div class=\"matrix-cell\" id=\"cel-33\"></div>
                  <div class=\"matrix-cell\" id=\"cel-32\"></div>
                  <div class=\"matrix-cell\" id=\"cel-31\"></div>
                  <div class=\"matrix-cell\" id=\"cel-30\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-4>
                  <div class=\"matrix-cell\" id=\"cel-49\"></div>
                  <div class=\"matrix-cell\" id=\"cel-48\"></div>
                  <div class=\"matrix-cell\" id=\"cel-47\"></div>
                  <div class=\"matrix-cell\" id=\"cel-46\"></div>
                  <div class=\"matrix-cell\" id=\"cel-45\"></div>
                  <div class=\"matrix-cell\" id=\"cel-44\"></div>
                  <div class=\"matrix-cell\" id=\"cel-43\"></div>
                  <div class=\"matrix-cell\" id=\"cel-42\"></div>
                  <div class=\"matrix-cell\" id=\"cel-41\"></div>
                  <div class=\"matrix-cell\" id=\"cel-40\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-5>
                  <div class=\"matrix-cell\" id=\"cel-59\"></div>
                  <div class=\"matrix-cell\" id=\"cel-58\"></div>
                  <div class=\"matrix-cell\" id=\"cel-57\"></div>
                  <div class=\"matrix-cell\" id=\"cel-56\"></div>
                  <div class=\"matrix-cell\" id=\"cel-55\"></div>
                  <div class=\"matrix-cell\" id=\"cel-54\"></div>
                  <div class=\"matrix-cell\" id=\"cel-53\"></div>
                  <div class=\"matrix-cell\" id=\"cel-52\"></div>
                  <div class=\"matrix-cell\" id=\"cel-51\"></div>
                  <div class=\"matrix-cell\" id=\"cel-50\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-6>
                  <div class=\"matrix-cell\" id=\"cel-69\"></div>
                  <div class=\"matrix-cell\" id=\"cel-68\"></div>
                  <div class=\"matrix-cell\" id=\"cel-67\"></div>
                  <div class=\"matrix-cell\" id=\"cel-66\"></div>
                  <div class=\"matrix-cell\" id=\"cel-65\"></div>
                  <div class=\"matrix-cell\" id=\"cel-64\"></div>
                  <div class=\"matrix-cell\" id=\"cel-63\"></div>
                  <div class=\"matrix-cell\" id=\"cel-62\"></div>
                  <div class=\"matrix-cell\" id=\"cel-61\"></div>
                  <div class=\"matrix-cell\" id=\"cel-60\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-7\">
                  <div class=\"matrix-cell\" id=\"cel-79\"></div>
                  <div class=\"matrix-cell\" id=\"cel-78\"></div>
                  <div class=\"matrix-cell\" id=\"cel-77\"></div>
                  <div class=\"matrix-cell\" id=\"cel-76\"></div>
                  <div class=\"matrix-cell\" id=\"cel-75\"></div>
                  <div class=\"matrix-cell\" id=\"cel-74\"></div>
                  <div class=\"matrix-cell\" id=\"cel-73\"></div>
                  <div class=\"matrix-cell\" id=\"cel-72\"></div>
                  <div class=\"matrix-cell\" id=\"cel-71\"></div>
                  <div class=\"matrix-cell\" id=\"cel-70\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-8\">
                  <div class=\"matrix-cell\" id=\"cel-89\"></div>
                  <div class=\"matrix-cell\" id=\"cel-88\"></div>
                  <div class=\"matrix-cell\" id=\"cel-87\"></div>
                  <div class=\"matrix-cell\" id=\"cel-86\"></div>
                  <div class=\"matrix-cell\" id=\"cel-85\"></div>
                  <div class=\"matrix-cell\" id=\"cel-84\"></div>
                  <div class=\"matrix-cell\" id=\"cel-83\"></div>
                  <div class=\"matrix-cell\" id=\"cel-82\"></div>
                  <div class=\"matrix-cell\" id=\"cel-81\"></div>
                  <div class=\"matrix-cell\" id=\"cel-80\"></div>
                </div>
                <div class=\"matrix-column\" id=\"col-9\">
                  <div class=\"matrix-cell\" id=\"cel-99\"></div>
                  <div class=\"matrix-cell\" id=\"cel-98\"></div>
                  <div class=\"matrix-cell\" id=\"cel-97\"></div>
                  <div class=\"matrix-cell\" id=\"cel-96\"></div>
                  <div class=\"matrix-cell\" id=\"cel-95\"></div>
                  <div class=\"matrix-cell\" id=\"cel-94\"></div>
                  <div class=\"matrix-cell\" id=\"cel-93\"></div>
                  <div class=\"matrix-cell\" id=\"cel-92\"></div>
                  <div class=\"matrix-cell\" id=\"cel-91\"></div>
                  <div class=\"matrix-cell\" id=\"cel-90\"></div>
                </div>
              </div>            
            ");


            // COLORS MENU
            client.println("
            <div id=\"qcs-w\" style=\"display: block;\"> 
              <div class=\"qcs\" onclick=\"pC('#ff0000');\" style=\"background-color:#ff0000;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#ffa000');\" style=\"background-color:#ffa000;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#ffc800');\" style=\"background-color:#ffc800;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#ffe0a0');\" style=\"background-color:#ffe0a0;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#ffffff');\" style=\"background-color:#ffffff;\"></div> 
              <div class=\"qcs qcsb\" onclick=\"pC('#000000');\" style=\"background-color:#000000;\"></div>
              <br> 
              <div class=\"qcs\" onclick=\"pC('#ff00ff');\" style=\"background-color:#ff00ff;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#0000ff');\" style=\"background-color:#0000ff;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#00ffc8');\" style=\"background-color:#00ffc8;\"></div> 
              <div class=\"qcs\" onclick=\"pC('#08ff00');\" style=\"background-color:#08ff00;\"></div> 
              <div class=\"qcs\" onclick=\"pC('rnd');\" style=\"background-color:var(--c-3);padding:4px 8px;transform:translateY(-10px);\">R</div>
            </div>
            ");



























            // END HTML
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
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