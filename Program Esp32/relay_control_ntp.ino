#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

//#define WIFI_SSID "........."
//#define WIFI_PASSWORD "..........."

unsigned long previousMillis = 0;
unsigned long interval = 10000;

#define indi_wifi 2
#define relay 13

const long utcOffside = 25200; // GMT+7 (Vietnam,Thailand.Laos,Cambodia,Indonesia)
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"}; // Indonesia format day

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffside);

void setup() {
  Serial.begin(115200);

  pinMode(indi_wifi, OUTPUT);
  //relay
  pinMode(relay, OUTPUT);
  //WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(indi_wifi, HIGH);
    delay(80);
    digitalWrite(indi_wifi, LOW);
    delay(80);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  timeClient.begin();
  digitalWrite(relay, HIGH);
}

void reconnect() {
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
//    Serial.print(millis());
//    Serial.print("Status WiFi : ");
//    Serial.println("Reconnecting...");
    digitalWrite(indi_wifi, LOW);
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

  else if (WiFi.status() == WL_CONNECTED) {
//    Serial.print("Status WiFi : ");
//    Serial.println("Connect...");
    digitalWrite(indi_wifi, HIGH);
  }
}

void loop() {
  reconnect();
  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());

  //jadwal cut off relay
  if (timeClient.getHours() == 10 && timeClient.getMinutes() == 53) {
    digitalWrite(relay, LOW);
    Serial.println("On State");
  }
  else if (timeClient.getHours() == 10 && timeClient.getMinutes() == 55) {
    digitalWrite(relay, HIGH);
    Serial.println("Off State");
  }
  delay(1000);
}
