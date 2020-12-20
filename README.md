
# TTGO ESP32-OLED (WIFI_Lora_32)

## LILYGO TTGO LORA32 868 MHz ESP32 LoRa OLED 0,96 polegadas Blue Display bluetooth WIFI ESP-32 Módulo de placa de desenvolvimento com Antena

![diagram](/img/image.webp)
![ttgo1](/img/ttgo01.jpeg)
![ttgo2](/img/ttgo02.jpeg)
![ttgo3](/img/ttgo03.jpeg)
![ttgo4](/img/ttgo04.jpeg)

## Description

A antena de 868 MHz precisa estar em conexão com a interface IPEX (se a antena não estiver conectada, pode danificar o chip LoRa)
Carga da bateria de lítio e circuito de descarga, quando a bateria estiver cheia, o azul LED irá parar de funcionar. Ao usar, preste atenção ao positivo e negativo da bateria, caso contrário ela será danificada!
Com a entrada de sinal de toque da porta IO tela sensível ao toque, você precisa adicionar o capacitor pull-down 100nF a este pino!
Nota: este produto não inclui a bateria.

Exemplo:

Este produto é um chip SX1276 baseado em ESP32 WIFI aumentado OLED, ou seja, modem remoto LoRa, frequência de 868 MHz, alta sensibilidade é superior a 148dBm, potência de saída de + 20dBm, alta confiabilidade, longa distância de transmissão.
a antena wi-fi de 32 MB Flash integrada, display oled azul de 0,96 polegadas, circuito de carregamento de bateria de lítio, interface CP2102 e chip serial USB, o suporte perfeito para ambiente de desenvolvimento, pode ser usado para verificação de programa e o desenvolvimento de produto é muito fácil e rápido.
Tensão de operação: 3,3 V a 7 V
Faixa de temperatura de operação: -40 ° C a + 90 ° C
Suporte para análise de protocolo de software Sniffer, modos Station, SoftAP e Wi-Fi Direct
Taxas de dados: 150 Mbps @ 11n HT40., 72 Mbps @ 11n HT20, 54 Mbps @ 11g, 11 Mbps @ 11b
potência de transmissão: 19,5 dBm @ 11b, 16,5 dBm @ 11g, 15,5 dBm @ 11n
sensibilidade do receptor até -98 dBm
Taxa de transferência sustentada de UDP de 135 Mbps

## Package included

- 2 x ESP32 LoRa OLED Module
- 2 x Line
- 4 x Pin
- 2 x 868mhz Spring Antenna

## 868mhz OLED LoRaSender

```C++
#include 
#include 
#include 
#include "SSD1306.h"
#include "images.h"

#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)
#define BAND 868E6 // 915E6

unsigned int counter = 0;

SSD1306 display (0x3c, 4, 15);
String rssi = "RSSI -";
String packSize = "-";
String packet;


void setup () {
  pinMode (16, OUTPUT);
  pinMode (2, OUTPUT);
  
  digitalWrite (16, LOW); // set GPIO16 low to reset OLED
  delay (50);
  digitalWrite (16, HIGH); // while OLED is running, GPIO16 must go high
  
  Serial.begin (9600);
  while (! Serial);
  Serial.println ();
  Serial.println ("LoRa Sender Test");
  
  SPI.begin (SCK, MISO, MOSI, SS);
  LoRa.setPins (SS, RST, DI0);
  if (! LoRa.begin (868)) {
    Serial.println ("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
// LoRa.receive ();
  Serial.println ("init ok");
  display.init ();
  display.flipScreenVertically ();
  display.setFont (ArialMT_Plain_10);
  delay (1500);
}

void loop () {
  display.clear ();
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);
  
  display.drawString (0, 0, "Sending packet:");
  display.drawString (90, 0, String (counter));
  display.display ();

  // send packet
  LoRa.beginPacket ();
  LoRa.print ("hello");
  LoRa.print (counter);
  LoRa.endPacket ();

  counter ++;
  digitalWrite (2, HIGH); // turn the LED on (HIGH is the voltage level)
  delay (1000); // wait for a second
  digitalWrite (2, LOW); // turn the LED off by making the voltage LOW
  delay (1000); // wait for a second
}




```

## 868mhz OLED LoRA Receiver

```C++

#include 
#include 
#include 
#include "SSD1306.h"
#include "images.h"

#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)
#define BAND 868E6 // 915E6

SSD1306 display (0x3c, 4, 15);
String rssi = "RSSI -";
String packSize = "-";
String packet;



void loraData () {
  display.clear ();
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);
  display.drawString (0, 15, "Received" + packSize + "bytes");
  display.drawStringMaxWidth (0, 26, 128, packet);
  display.drawString (0, 0, rssi);
  display.display ();
}

void cbk (int packetSize) {
  packet = "";
  packSize = String (packetSize, DEC);
  for (int i = 0; i   rssi = "RSSI" + string (LoRa.packetRssi (), DEC);
  loraData ();
}

void setup () {
  pinMode (16, OUTPUT);
  digitalWrite (16, LOW); // set GPIO16 low to reset OLED
  delay (50);
  digitalWrite (16, HIGH); // while OLED is running, GPIO16 must go high,
  
  Serial.begin (9600);
  while (! Serial);
  Serial.println ();
  Serial.println ("LoRa Receiver Callback");
  SPI.begin (SCK, MISO, MOSI, SS);
  LoRa.setPins (SS, RST, DI0);
  if (! LoRa.begin (868E6)) {
    Serial.println ("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
  LoRa.receive ();
  Serial.println ("init ok");
  display.init ();
  display.flipScreenVertically ();
  display.setFont (ArialMT_Plain_10);
  
  delay (1500);
}

void loop () {
  int packetSize = LoRa.parsePacket ();
  if (packetSize) {cbk (packetSize); }
  delay (10);
}
```

```bash
echo "# Lora_Esp32" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/Joaosilgo/Lora_Esp32.git
git push -u origin main
```
