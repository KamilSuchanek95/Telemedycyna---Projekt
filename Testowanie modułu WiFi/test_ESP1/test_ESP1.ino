#include <ESP8266WiFi.h>

/* Statements */
const char* ssid     = "miao";
const char* password = "miau123MIAU";

const char* host = "ec2-54-221-237-246.compute-1.amazonaws.com";
const char* database = "d6vbf3bajgqsh4";
const char* user = "jdbigwjpuutcuh";
const char* streamId   = "....................";
const char* privateKey = "8e684ca67665828a0bb7567d03d1faa3bb0875749d88f65af3db0fcb0cfd924d";

void setup() {
  delay(1000);
  Serial.begin(115200);  delay(10);  Serial.println('\n');
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);  Serial.print("Connecting to ");  Serial.print(ssid); Serial.println(" ...");
  
  
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  WiFi.printDiag(Serial);

}

void loop() {
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 5432;
  if (client.connect(host, httpPort)) {
    Serial.println("Chyba polaczylo");
  }

 client.println("psql -U jdbigwjpuutcuh --password 8e684ca67665828a0bb7567d03d1faa3bb0875749d88f65af3db0fcb0cfd924d -p 5432 -h ec2-54-221-237-246.compute-1.amazonaws.com -d d6vbf3bajgqsh4 \r\n");
      String line = client.readStringUntil('\n');
      Serial.println(line);
  delay(1000);
 client.println("8e684ca67665828a0bb7567d03d1faa3bb0875749d88f65af3db0fcb0cfd924d");
 line = client.readStringUntil('\n');

 
      Serial.println(line);
  delay(1000);
 client.println("UPDATE users SET role=1 WHERE first_name='Kamil';");
line = client.readStringUntil('\n');
      Serial.println(line);
  // Read all the lines of the reply from server and print them to Serial
  while (client.connected())
  {
    if (client.available())
    {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  Serial.println();
  Serial.println("closing connection");


  
  }
