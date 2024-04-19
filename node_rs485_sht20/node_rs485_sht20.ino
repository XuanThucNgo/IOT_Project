/*
  ______________________
  | Sensor (SHT20)   |
  |  A (Yellow)      |
  |  B (White)       |
  |  GND (Black)     |
  |  Vs (Red)        |
  _____________________
*/
#include <ModbusMaster.h>
#include <SPI.h>  
#include <LoRa.h>

#define MAX485_RE_DE  10
#define MAX485_RX     2 
#define MAX485_TX     3

#define LORA_SCK      4
#define LORA_MISO     5
#define LORA_MOSI     6
#define LORA_CS       7
#define LORA_RST      8
#define LORA_INT      1

ModbusMaster node;

SPIClass * lora_spi = NULL;

void preTransmission() {
  digitalWrite(MAX485_RE_DE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_RE_DE, 0);
}

void setup() {
  // Initialize RS485 control pins
  pinMode(MAX485_RE_DE, OUTPUT);
  digitalWrite(MAX485_RE_DE, 0);

  // Modbus communication runs at 9600 baud
  Serial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1, MAX485_RX, MAX485_TX);

  // Modbus slave ID 1
  node.begin(1, Serial1);

  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("Init RS485 succeed");
  // Initialize Ra-02
  lora_spi = new SPIClass(FSPI);
  lora_spi->begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  LoRa.setSPI(*lora_spi);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_INT);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("Init Ra-02 succeed");
}

void loop() {
  // Request 2 registers starting at 0x0001
  uint8_t result = node.readInputRegisters(0x0001, 2);
  Serial.println("Data Requested");
  Serial.println(Serial1.readString());
  if (result == node.ku8MBSuccess) {
    // Get response data from sensor
    Serial.print("First_register: ");
    float temp = node.getResponseBuffer(0) / 10.00F;
    Serial.print(temp);
    Serial.print("   Second_register: ");
    float humi = node.getResponseBuffer(1) / 10.00F;
    Serial.println(humi);
    char* packet;
    asprintf(&packet, "First_register:%f Second_register:%f", temp, humi);
    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();
    free(packet);
  }
  delay(1000);
}