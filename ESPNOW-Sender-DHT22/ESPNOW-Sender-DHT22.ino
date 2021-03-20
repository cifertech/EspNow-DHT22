#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

uint8_t broadcastAddress1[] = {0xBC,0xDD,0xC2,0xB3,0xC4,0xC2};
.
typedef struct temp_struct {
  float x;
  float y;
} temp_struct;

temp_struct test;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  Serial.begin(115200);

  dht.begin();
 
  WiFi.mode(WIFI_STA);

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {

  sensors_event_t event;
  
  dht.temperature().getEvent(&event);
    test.x = event.temperature;


    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));

  dht.humidity().getEvent(&event);
     test.y = event.relative_humidity;

    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
   
  esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(temp_struct));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
