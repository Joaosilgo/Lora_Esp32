/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *characteristicTX; //através desse objeto iremos enviar dados para o client
bool deviceConnected = false; //controle de dispositivo conectado

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "bccea50a-072b-491f-a74b-d6c041be8e97" // UART service UUID
#define CHARACTERISTIC_UUID_RX "cd888615-e1e3-42ef-9d71-a39215cbc97d"
#define CHARACTERISTIC_UUID_TX "6bc6a24c-580e-48f7-bdb8-15a52f3dec43"


//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//packet counter
int counter = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
/*

  hw_timer_t *timer = NULL; //faz o controle do temporizador (interrupção por tempo)

  //função que o temporizador irá chamar, para reiniciar o ESP32
  void IRAM_ATTR resetModule() {
  ets_printf("(watchdog) reiniciar\n"); //imprime no log
  esp_restart(); //reinicia o chip
  }

*/


//callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

//callback  para envendos das características
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      //retorna ponteiro para o registrador contendo o valor atual da caracteristica
      std::string rxValue = characteristic->getValue();
      //verifica se existe dados (tamanho maior que zero)
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();

        // Do stuff based on the command received


        Serial.println();
        Serial.println("*********");
      }
    }
};



void setup() {

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA SENDER ");
  display.display();

  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 10);
  display.print("LoRa Initializing OK!");
  display.display();


  //hw_timer_t * timerBegin(uint8_t num, uint16_t divider, bool countUp)
  /*
     num: é a ordem do temporizador. Podemos ter quatro temporizadores, então a ordem pode ser [0,1,2,3].
    divider: É um prescaler (reduz a frequencia por fator). Para fazer um agendador de um segundo,
    usaremos o divider como 80 (clock principal do ESP32 é 80MHz). Cada instante será T = 1/(80) = 1us
    countUp: True o contador será progressivo
  */
  /*
    timer = timerBegin(0, 80, true); //timerID 0, div 80
    //timer, callback, interrupção de borda
    timerAttachInterrupt(timer, &resetModule, true);
    //timer, tempo (us), repetição
    timerAlarmWrite(timer, 30000000, true);
    timerAlarmEnable(timer); //habilita a interrupção
  */

  // Create the BLE Device
  BLEDevice::init("ESP32-BLE"); // nome do dispositivo bluetooth
  // Create the BLE Server
  BLEServer *server = BLEDevice::createServer(); //cria um BLE server
  server->setCallbacks(new ServerCallbacks()); //seta o callback do server
  // Create the BLE Service
  BLEService *service = server->createService(SERVICE_UUID);
  // Create a BLE Characteristic para envio de dados
  characteristicTX = service->createCharacteristic(
                       CHARACTERISTIC_UUID_TX,
                       BLECharacteristic::PROPERTY_NOTIFY
                     );

  characteristicTX->addDescriptor(new BLE2902());

  // Create a BLE Characteristic para recebimento de dados
  BLECharacteristic *characteristic = service->createCharacteristic(
                                        CHARACTERISTIC_UUID_RX,
                                        BLECharacteristic::PROPERTY_WRITE
                                      );

  characteristic->setCallbacks(new CharacteristicCallbacks());
  // Start the service
  service->start();
  // Start advertising (descoberta do ESP32)
  server->getAdvertising()->start();

  delay(2000);
}

void loop() {

  /*
    timerWrite(timer, 0); //reseta o temporizador (alimenta o watchdog)
  */

  Serial.print("Sending packet: ");
  Serial.println(counter);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LORA SENDER");
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("LoRa packet sent.");
  display.setCursor(0, 30);
  display.print("Counter:");
  display.setCursor(50, 30);
  display.print(counter);
  display.setCursor(0, 40);
  display.print("Connected:");
  display.setCursor(60, 40);
  //display.print(String(deviceConnected));
  display.print( deviceConnected ? "True" : "False");
 
  display.display();

  //se existe algum dispositivo conectado
  if (deviceConnected) {
    // Let's convert the value to a char array:
    char txString[12]; // make sure this is big enuffz
    // dtostrf(tempAmbiente, 1, 2, txString); // float_val, min_width, digits_after_decimal, char_buffer
    sprintf(txString, "%d", counter);
    characteristicTX->setValue(txString); //seta o valor que a caracteristica notificará (enviar)
    characteristicTX->notify(); // Envia o valor para o smartphone

    Serial.print("*** Sent Value: ");
    Serial.print(txString);
    Serial.println(" ***");
  }

  counter++;

  delay(10000);
}
