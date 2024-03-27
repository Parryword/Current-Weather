/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* wifi_network_ssid = "Ekonet-Student";
const char* wifi_network_password = "";

// Replace with your network credentials
const char* soft_ap_ssid     = "ESP32-Access-Point Efe";
const char* soft_ap_password = "123456789";
int channel = 1;
int ssid_hidden = 0;
int max_connection = 9;
bool ftm_responder = false;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output2State = "off";
String output27State = "off";

//Your Domain name with URL path or IP address with path
String serverName1 = "https://api.weatherapi.com/v1/current.json?q=Izmir&key=1434b792f69942ddb51201230242303";
String serverName2 = "https://api.openweathermap.org/data/2.5/weather?q=Izmir&appid=660aad8d134eb678f8445709216b8361";

// Assign output variables to GPIO pins
const int output2 = 2;
const int output27 = 27;

// Weather data
struct ClientData {
  double temp;
  double humidity;
};

struct ClientData clientData;
String selectedCity;
String imageTemp = "<img height='50' src=' data:image/png;base64,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'/>";
String imageHumid = "<img height='50' src=' data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAYAAACtWK6eAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAABCMSURBVHhe7Z0JkFxVFYbPfT3ZIEDYdyNLwsw0MRBRKBJUIMgiKIjEoCIiKIgLJDPdk0RBkdLEmVFQi7K0xF1cSyy1Ci3BDVwLFZjp7gkxKCiLIpsJEjLT7/jfd6/lVMwks/Trfue+8xW37zl3QtXMe/2/c8+7GymKoiiKoiiKoiiKoiiKooyfcoVPLA3yEu8qGcD4WmkxKyt8UMHQD2AaE9FZve3mUfcTpZVEvlZayE13sYE4roV5LMoxHNP7r3yY9d5kAL0JGeDBvegiVG9xXsIluz5Db/a20kK0i9ViSlVeiOpbKPOShv8xZJiW9RbNgPeVFqARpIV03ce7oFqDsq04LO1s6JpVg7yr95UWoAJpIYU2eheqZc7bLhfUI3qHt5UWoF2sFtFd5VNw8b8Dcw/XMiZPM9M5/UVzl/eVJqICaQGrKrxv3dCPYNq3VuPh7riNzvjofPOE95UmoV2sFgBxfBjVeMVhOa4wQtd5W2kiKpAmU67yG1Atd974YaI3lSr8Ou8qTUK7WE0EeccRPu94kWuZMH/A/39+b6f5i/eVlNEI0iRKAzzDMK2COVlxWBYhkqzsqfE07yspowJpEqZA5yJeX+bdqXBlnelsbyspo12sJtA1wIdFBboT5sGuZYowPVggWryuaB72LUpKaARJmavv5bYoon6YjRGHxdDcuqG1H3yAoRMlTVQgKTNtGl2OL/RrvNtILty8hd7qbSUlVCApUqrwcai6UdJ40rchYe8uVXkqSb+yE1QgKdE1yLsjcpRhvtC1pMJ8lBWlAZ7pXKXRqEBSAnnHBahsSZuLEZ/O87bSYPQtVgqUa9zOTD+EOde1pE4FN/JsHUBsPBpBGsyazWwgDrt8tlnisBSRj9h1JUqDUYE0mJGHkqWzE55r1QDegoT9XG8rDUK7WA2kPMSHc0w/hnm4a2k61RGmU28omse8r0wRjSANxO5GgqpV4rB0trndUZQGoQJpEOjevBHVa53XUpaVq6xztRqECqQB4AtpxzpWosxOGlrL3skAYoX3974yBVQgU6SnynZE+0qYi1xLJng5ssuLva1MARXIFKkTvQSV3Z0kWxgqI7IVvadMEhXIFCgP8SxDtA7mLNeSITjpal1nI5xvUSaBCmQqxPR2fL7MOZnk/Jjo9d5WJoGOg0yS8iB3cES3wWzmiPlkGDJ1Wtq7QBdXTQaNIJPgsiE2EMdqmFkXh6WdC8msYmUSqEAmwZ5xMntW0gza5aUqn+ptZQKoQCZIV5UPRHUVShbGPMbLfihXd1V4jnOV8aICmSC4YHbEPMuJ+VicXTCZGOkXhSbpEwBP4GJk6A6YUkep1zPRaf2d5q/eV3aCRpAJAHH0oJI8heMoPBHtqL8yTjSCjJPuGp9hmL4HU/quhpuZaUl/0dzrfWUHaAQZBys38kyIw04jD2HLz9nG0Ae9rewEFcg4KGxJTnmyW/iEwtJShVux6lEcKpCdUK7wPHREL4UZ0obRu+BveldPlff1vjIGKpCdwG7D6RBnxS6O7ZZByg5RgeyAco2PRzX6/PLQuLRU4w5vK9tBBTIGEMcsZno3TDsKHSp2/66LL9yIOKlsFxXIWDCdhM8LnRMuUMblhzw/pUN9gkYFsh3eu5GnM9E1MPNwfeZAJDrbdwxUINth69ZkkdES54UPHgbnlyv8Cu8qo1CBbEP3et4d3xh7ZEGemMERXbv6z5qLbIsKZBtMPRkUzN9mB0wnDD9H9ohqZRQqkFGgm2H3t7JfkjwebWY3oHj76kHew/sKUIGMAh2MN6PK8xudE0ei8N/cTQQViKdc46NR5X1k2W4RdHFpkA91rqICAV33ccScPDlbufF0VjgB34pXeTv3qECAaaMjUL3NeQq4orvGB3g716hAAJLTK1DpzNb/sRDXRM89BLl/791V4SMjQ7+FuZdrUTx/4oiO7283T3o/l+Q+gkAc9tgCFcf/c6RhusTbuSXXAumuJIfwa1diLJgu716f70VVuRaIMXQ5Kk1Gx+YIE+f75UVuBdJdZXuux1nOU8YgQhRZVh7gF3g/d+RWIIZoGSo7tUTZMQu5QBd4O3fkUiClKi9Apdtwjp/zyjU+xNu5IpcCYaIzUemo+fhZzCxyP+IpkzuBlGp8ELpXuX99OQku6a7lb3f43AkE0eMMVO3OUybAUnxZXurt3JArgZTX8y6GaYV3lQmCbtbKj92Tr1WHuRII1+lcVHZauzI5Tnl0Bi32di7IjUCueyx58mXvPHNZTKM4mZqTG3IjkGefSl7r2te7ylQwdFKpxrnJRXIjEPSf7W7mks4VzCr7ENNF3g6eXAikVGXbbz7BeUoDOLlc4yO9HTR5iSD21W4uR4JTosg5mccWvEAQPQ5DZQWiNBKm08sV3tt7wRK8QAyTPcIgpNOhssLS2NCLvR0sQQsE/eTZbHSfp5SYbojOKVU4pJO3/o+gBYJ+sj0cRrtX6bGco7CXDITexbI7JU53ppIC+yCKvMbbQRLsvJqeQd4rjmgDTN2QIV2GCrvSwnVzzVbvB0WwEQTiOB+ViiN95tf/He5akZC7WHoOeHOw27barmyQBCmQUo3t60c9d69JoJ/+ip4hPti7QRFmBOFkk4F9nKM0gf3jephr/IMTSE+Fbd5hT6hVmsd0hJEz+57l4L5Pwf1BsUkSxvwdodZ6FvzjITrR28EQnECYEoHoMWLN5xBc/JO9HQxBCaRU5SOQML7cu0rzWdxT4/28HQShRRC7W8kiZyot4MQ4pvneDoJgBFK6n+35ernc3CxD7IZk3e55HAzBCITjZNTc7piotJYz0dUNJgcMp4vFNA+fuilD67E5YDCDhsEIxHDYs0oFYWdPL3WmfIIQyLvvZXtT9KSojGCIlq+qcsG7oglCILOmJ0tq7dpzJQMw0aJhDmP3/CAEwkznoAriiRUIMyITxkrOIARiZ5N6U8kOp/laNOIF0l3leQjpepRa9li4YoDFz6gWLxD8ATr3KpvMaSvQEm+LRbxAED3sDNJZzlMyxO4oKpBW0nM/74lKT4vKLsf2DMjeN0u0QHiEFqKa6zwlg8yNC8k9Eotsgbh150GuhQ4E+/ASvTeA9BzE7pyoZBc7w1r0PRIrkO4hPhCVnaCoZJt55WqSK4pErECimA5AFdTinECZHxPZh5lIxAoE+YfNPQ51npJh5uFLdpC3xSFZIJp/yKAN90psV1ikQLqqbA/jPNZ5SuYxdGxpiEUO5ooUCH7p2YboGO8qWYfxMItlnjAsUiCxoT20iyWKo1F2c6YsZEYQpsMRQRQ5zMT9EjmgK1IgQKOHMBDxO70pCnECWV3lCE8jFYgwDFPHmntYXOAXJ5CtuNaxCkQcHFHn8Ex53zdxv3BbRG14DOkIujSY2hFFxO0bIE4giB52VHaO8xRBHBDH8g41EicQricLpMT93grumZG3uE3eF80kawxUIPIw6BqLW9wm7ouGi2xnhop7G6JQhBzEzsAWhcQnsbiLrHiMvFm9EgUidup03mGB60IkCkTs4pu8A4GIm24iSiBX3pnsGK6veIWCxHFvb4pBlEB22Td5j26POlBkMmPloKztSEUJxNST3fp0F3e5FNoKsqa9y8pBDM3Ap8S8SXFEbGimt0UgK4KwCkQ4EdVVIKnBUXJxVSByKRiNIOlhYo0gwrHzsVQgaeH7ryoQuUTMKpD00BxEOjaC2HsoBlkRJLKDsYpohN1BaU/jYZTYmYpA7L3b6kwZiBKI4eTiahSRiwokTSCQTahGnKcIpI4cxN5DMcjKQQw9jUoFIpdhPOSe8rYIpOUgT6DYPESRyQjX6Ulvi0CUQPo6zeOotjhPEchzfQvMP70tAmkRxPKorxVhsMB7J1EgG32tCMMQPeBNMYgTCC5yBZW+6pWHvWdVZ8pBnEBwlQdQ6WChPOw9G3SmHOQJpED3otI3WfIYKcR0n7fFIE4gIzE9iShS864ihyETk30LKQp5STpTHZ+/dI4iBcP0KzzY7L0ThTiB3Nhp7CksP/OuIoTY0M/XvQgyEYa8CGJh+gM+7bQTRQabIkN3e1sUMgVikiknv3COknUQ8e9CsfdMHCIF0tdp/oXqR85Tsg76xLd/pMOIjPgyI4jj9yji3orkkKeQefzG2+IQKxCE7CFUdzlPyTC/NobWe1scYgXS22megUju8K6SXX6C7pXI/MMiuYtlp53cjkrc/J4csQHR48feFologSBZt6Fbx0SyCnKP3g4jbnrJaEQLJIHpi/gUtQgnJzyNRPFz3haLeIH0Fc3vUP3ceUqG+A0ivPjoLj+CWCJai89/O0fJAHZjjXXOlE0QAulrN3ZM5JvOUzLA9x9nutPbogkjggAm6kclakuZQNkSG/rIF4omiEVtwQikvkfyuvfjzlNayE3T25JZDkEQjEBuONgwM30J5h9di9ICNpChm9fOM8Fs7heMQCz9RfNnA63AfM61KE1kBOL4RF+HCWq1Z1ACsczaTF/DjbrVu0rz+CGK+HGPbcEDNzxKFZ6Lv8xOcZjnWpSUeYQNndXfYeyGGkERXASx9BXNg8hHemA+71qUFKlDHNeGKA5LkAKxIB+x3ayPOk9Jkc9BHDd7OziCFYgFUeRDqL7tPCUFbjcRrfF2kASZg4yme4jnmpi+AnOJa1EaRM0YurA30K7Vfwk6glj625GPEK2A+SfXojSAR/BkXRG6OCzBC8TS32nuNkzvgfmIa1GmwDPEtKq30+Ri04xcCMTSWzS3IZK8E6auHZk8z6Ks7iuaLzs3fHIjEAsiyXeRdVmRaCSZOJvQrerp6zSf8n4uCD5J3x6lKr8a1U0ohyQNys6w5wp2Qxyfd25+yKVALBDJSag+jdKRNChj8ZgxdBUS8lyut8mtQCzlKi9AXvJZmC91Lco2VPEFuQIJeRCLnyZDrnKQbcGNH4jq9Gpi+jpcaEUZxW14fJ6bZ3FYch1B/svq+3nGyAhdDXMlyn5JY355Do+Km9oiun5tR7IHcq5RgYwCecmpqHpRFiUNecNuEcr0vr0eou+sPiOMJbNTRQWyDd3reX+qUxcuzFVwp7vW4InxTbjFRHRN71HmL75NASqQMSjVeCmSkusN03Fw21xrkAww0wdGptGtN86XdwJU2qhAdkDPBp4VD9M7YF6K0pk0hsNGdKe+0WbohrWdRmcXjIEKZBx0VfkoXKg3oCyHO9+1iuUhlG9AHLf0Fc09rkkZCxXIBOiucDEydDr6IZfBlTbA+DeUz+N3v7W/0+jOL+NEBTIJSoN8IEX0Sly9t+FJfDyaspqj2GOX74MoPoPf87bnY/rrJxfo26mJoAKZAl1DXMBX8MWIKhfBPQ3lQJTd7c9ayCaUv6P8BHf3q6h/1dcRzj5VzUYF0iDKG3g2D9MpMG05BuVQFDsZMu1XxcMoD6PY3GIA5ae4qXf0dso8NDNrqEBSYE2FZ+FbezSurs1T2lEOQ5mLYkVjo0wBZTLYLpONDlYMttiN8oaYaWh4Gg3cON/Y9RpKA1GBNIFSjXfFhd4XecDecOfEqI2h/WHviXpPfMF3gz0T/ya5H8gZ7HjEFvxsE35mN+R+CvXf0ZV7Av/iGfz/T0QxPd57tNls/72SHiqQFlJ+gKNomNrqdSoYpgiCcAJh/GcoLiDDiafRSO/hmlgriqIoiqIoiqIoipJriP4DdT+zVyOLHEYAAAAASUVORK5CYII='/>";
void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output2, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output2, LOW);
  digitalWrite(output27, LOW);

  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("GPIO 2 on");
              weatherRequest();
              output2State = "on";
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("GPIO 2 off");
              output2State = "off";
              digitalWrite(output2, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".center {margin-left: auto; margin-right: auto}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server by Efe</h1>");
            client.println("<select name='city' id='city' />");
            client.println("<option value='Adana'>Adana</option>");
            client.println("<option value='Ankara'>Ankara</option>");
            client.println("<option value='Antalya'>Antalya</option>");
            client.println("<option value='Istanbul'>Istanbul</option>");
            client.println("<option value='Izmir' selected>Izmir</option>");
            client.println("</select>");

            client.println("<p>");
            client.println("<table class='center'>");
            client.println("<tr>");
            client.println("<th>"+imageTemp+"</th>");
            client.println("<th>"+String(clientData.temp)+"</th>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<th>"+imageHumid+"</th>");
            client.println("<th>"+String(clientData.humidity)+"</th>");
            client.println("</tr>");
            client.println("</table>");
            client.println("</p>");

            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>GPIO 2 - State " + output2State + "</p>");
            // If the output2State is off, it displays the ON button       
            if (output2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
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
      String serverPath = serverName2;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      Serial.println(serverPath.c_str());
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
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
        double temp = doc["main"]["temp"];
        double humidity = doc["main"]["humidity"];
        clientData.temp = temp;
        clientData.humidity = humidity;
        Serial.println(clientData.temp);
        Serial.println(clientData.humidity);
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