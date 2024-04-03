#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE 11

using namespace std; 


const char* wifi_network_ssid = "Ekonet-Student";
const char* wifi_network_password = "";

const char* soft_ap_ssid     = "ESP32-Access-Point Efe";
const char* soft_ap_password = "123456789";
int channel = 1;
int ssid_hidden = 0;
int max_connection = 9;
bool ftm_responder = false;

WiFiServer server(80);

String header;

String query = "https://api.openweathermap.org/data/2.5/weather?q=Izmir&appid=660aad8d134eb678f8445709216b8361";

struct ClientData {
  double temp;
  double humidity;
  double windSpeed;
  double roomTemp;
};

struct ClientData clientData;

String selectedCity = "Izmir";

DHT dht(DHTPIN, DHTTYPE);

String imageTemp = "<img height='60' src=' data:image/png;base64,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'/>";
String imageHumid = "<img height='60' src=' data:image/png;base64,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'/>";
String imageWind = "<img height='60' src=' data:image/png;base64,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'/>";
String imageSensor = "<img height='60' src=' data:image/png;base64,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'/>";

void setup() {
  Serial.begin(115200);
  Serial.print("Setting AP (Access Point)…");

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password, channel, ssid_hidden, max_connection, ftm_responder);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();

  // Connect to the WiFi
  WiFi.begin(wifi_network_ssid); 
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  weatherRequest();

  dht.begin();

  delay(2000);

  float t = dht.readTemperature();

  if (isnan(t)) {
    Serial.println("Failed to read from sensor");
    return;
  }

  Serial.println(t);
 }

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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

            if (header.indexOf("GET /?q") >= 0) {
              Serial.println("User requested weather data");

              selectedCity = extractParam(header);
              buildQuery();
              weatherRequest();
              readSensor();
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".center {margin-left: auto; margin-right: auto}");
            client.println(".form-select{display:block;width:100%;padding:.375rem 2.25rem .375rem .75rem;-moz-padding-start:calc(0.75rem - 3px);font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#fff;background-image:url('data:image/svg+xml,%3csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'%3e%3cpath fill='none' stroke='%23343a40' stroke-linecap='round' stroke-linejoin='round' stroke-width='2' d='M2 5l6 6 6-6'/%3e%3c/svg%3e');background-repeat:no-repeat;background-position:right .75rem center;background-size:16px 12px;border:1px solid #ced4da;border-radius:.25rem;transition:border-color .15s ease-in-out,box-shadow .15s ease-in-out;-webkit-appearance:none;-moz-appearance:none;appearance:none}");
            client.println(".cell-text { font-size: 1.5em; padding-left: 0.5em; text-align:left; }");
            client.println(".button { background-color: #20bf6b; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #4b6584;}</style></head>");
            client.println("<body style=''><h1>Current Weather</h1>");
            client.println("<script>function setCity() {const $select = document.getElementById('city'); const cityName = $select.value; document.getElementById('query').href = '?q=' + cityName;}</script>");
            client.println("<p>");
            client.println("<table class='center'>");
            client.println("<tr>");
            client.println("<td colspan='2' style='padding-bottom: 1em'><select class='form-select' name='city' id='city'/>");
            client.println("<option value='Adana'>Adana</option>");
            client.println("<option value='Ankara'>Ankara</option>");
            client.println("<option value='Antalya'>Antalya</option>");
            client.println("<option value='Istanbul'>Istanbul</option>");
            client.println("<option value='Izmir'>Izmir</option>");
            client.println("</select></td>");
            client.println("</tr>");            
            client.println("<tr>");
            client.println("<td>"+imageTemp+"</td>");
            client.println("<td class='cell-text'>"+getTemperature()+"</td>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td>"+imageHumid+"</td>");
            client.println("<td class='cell-text'>"+String(clientData.humidity)+"%</td>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td>"+imageWind+"</td>");
            client.println("<td class='cell-text'>"+String(clientData.windSpeed)+" m/s</td>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td>"+imageSensor+"</td>");
            client.println("<td class='cell-text'>"+String(clientData.roomTemp)+"\xB0""C</td>");
            client.println("</tr>");
            client.println("</table>");
            client.println("</p>");

            client.println("<p><a id='query' href='/?q'><button onclick='setCity()' class=\"button\">UPDATE</button></a></p>");
            client.println("<script>function setSelect() {const $select = document.getElementById('city'); const $options = Array.from($select.options); const optionToSelect = $options.find(item => item.text === '" + selectedCity + "'); optionToSelect.selected = true;} setSelect();</script>");
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

void weatherRequest() {
 if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      http.setUserAgent("ESP32 - IEU");

      // String serverPath = serverName1;
      String serverPath = query;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      Serial.println(serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
          Serial.println("deserializeJson() failed");
          return;
        }
        clientData.temp = doc["main"]["temp"];
        clientData.humidity = doc["main"]["humidity"];
        clientData.windSpeed = doc["wind"]["speed"];
        Serial.println(clientData.temp);
        Serial.println(clientData.humidity);
        Serial.println(clientData.windSpeed);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}

String getTemperature() {
  double celcius = clientData.temp - 273;
  return String(celcius) + "\xB0""C";
}

String extractParam(String httpRequest) {
    String s = httpRequest;
    String query = "?q=";
    
    int start = s.indexOf(query);
    if (start == -1) {
        Serial.println("Query not found");
        return "";
    }
    start += query.length(); // Skip the query itself
    int end = s.indexOf(" HTTP/1.1", start);
    String value = s.substring(start, end);
    
    Serial.print("The value of the query is: ");
    Serial.println(value);
    return value;
}

void buildQuery() {
  query = "https://api.openweathermap.org/data/2.5/weather?q="+selectedCity+"&appid=660aad8d134eb678f8445709216b8361";
}

void readSensor() {
  float t = dht.readTemperature();

  if (isnan(t)) {
    Serial.println("Failed to read from sensor");
    clientData.roomTemp = -273;
  }

  Serial.println(t);
  clientData.roomTemp = t;
}