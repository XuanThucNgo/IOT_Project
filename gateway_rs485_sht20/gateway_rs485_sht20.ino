#include <SPI.h>
#include <LoRa.h>

#define MAX485_RE_DE  2
#define MAX485_RX     14 
#define MAX485_TX     15

#define LORA_SCK      4
#define LORA_MISO     5
#define LORA_MOSI     6
#define LORA_CS       22
#define LORA_RST      21
#define LORA_INT      5


SPIClass * lora_spi = NULL;


char* temp;
char* humi;

void setup() {
  // Initialize RS485 control pins
  pinMode(MAX485_RE_DE, OUTPUT);
  digitalWrite(MAX485_RE_DE, 1);

  // Modbus communication runs at 115200 baud
  Serial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1, MAX485_RX, MAX485_TX);

  Serial.println("Init RS485 succeed");
  Serial1.println("Init RS485 succeed");

  // Initialize Ra-02
  // lora_spi = new SPIClass(FSPI);
  // lora_spi->begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  // LoRa.setSPI(*lora_spi);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_INT);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    Serial1.println("Init Ra-02 succeed");
    while (1);
  }

  // put the radio into receive mode
  LoRa.receive();
  Serial.println("Init Ra-02 succeed");
  Serial1.println("Init Ra-02 succeed");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("Received packet: ");
    Serial1.println("Received packet: ");

    char* packet = (char*)malloc( (packetSize+1) * sizeof(char*) );
    char* buf = packet;
    while (LoRa.available()) {
      *buf++ = (char)LoRa.read();
      // Serial.print(*buf);
    }
    *buf= '\0';
    Serial.println(packet);
    Serial1.println(packet);
    Serial.print("with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial1.print("with RSSI ");
    Serial1.println(LoRa.packetRssi());


    free(packet);
  }
  delay(200);
}