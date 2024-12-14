// #include <SPI.h>
// #include <MFRC522.h>
// #include <Wire.h>
// #include <Adafruit_AHTX0.h>

// // RFID Reader Pins
// #define RST_PIN 2    // Reset pin for RFID
// #define SS_PIN 15    // SDA/SS pin for RFID
// #define SCK_PIN 27   // SPI Clock pin
// #define MOSI_PIN 26  // SPI MOSI pin
// #define MISO_PIN 25  // SPI MISO pin

// // Create an instance of the MFRC522 class
// MFRC522 rfid(SS_PIN, RST_PIN);

// // DHT20 Sensor Pins
// #define SDA_PIN 33  // SDA pin for I2C
// #define SCL_PIN 32  // SCL pin for I2C

// // Create an instance of the DHT20 sensor
// Adafruit_AHTX0 aht;

// void setup() {
//   // Initialize Serial Monitor
//   Serial.begin(9600);

//   // Initialize SPI for RFID
//   SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
//   rfid.PCD_Init();
//   Serial.println("RFID reader initialized!");

//   // Initialize I2C for DHT20
//   Wire.begin(SDA_PIN, SCL_PIN);
//   if (!aht.begin()) {
//     Serial.println("Could not find AHT20 sensor!");
//     while (1); // Halt execution if sensor is not found
//   }
//   Serial.println("AHT20 sensor initialized!");
// }

// void loop() {
//   // --- Read Temperature and Humidity from DHT20 ---
//   sensors_event_t humidity, temp;
//   aht.getEvent(&humidity, &temp); // Read data from the sensor

//   // Print temperature and humidity to Serial Monitor
//   Serial.print("Temperature: ");
//   Serial.print(temp.temperature);
//   Serial.println(" °C");

//   Serial.print("Humidity: ");
//   Serial.print(humidity.relative_humidity);
//   Serial.println(" %");

//   // --- Check for RFID Tag ---
//   if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
//     Serial.print("RFID UID: ");
//     for (byte i = 0; i < rfid.uid.size; i++) {
//       Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
//       Serial.print(rfid.uid.uidByte[i], HEX);
//     }
//     Serial.println();
//     rfid.PICC_HaltA(); // Stop reading the card
//   }

//   // Wait 2 seconds before the next loop
//   delay(2000);
// }

// #include <SPI.h>
// #include <MFRC522.h>

// #define RST_PIN 22 // Configurable, see typical pin layout above
// #define SS_PIN 21 // Configurable, see typical pin layout above

// MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance


// void setup() {
// Serial.begin(9600); // Initialize serial communications with the PC
// while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

// pinMode(RST_PIN, OUTPUT);
// digitalWrite(RST_PIN, HIGH);
// Serial.println("HELLO");
// Serial.println(digitalRead(RST_PIN));

// SPI.begin(25,27,26);            // Init SPI bus CLK, MISO, MOSI
// mfrc522.PCD_Init();     // Init MFRC522
// delay(4);               // Optional delay. Some board do need more time after init to be ready, see Readme
// mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
// Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
// }

// void loop() {
// // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
// if ( ! mfrc522.PICC_IsNewCardPresent()) {
// return;
// }

// // Select one of the cards
// if ( ! mfrc522.PICC_ReadCardSerial()) {
//     return;
// }

// // Dump debug info about the card; PICC_HaltA() is automatically called
// mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
// }

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

// RFID Reader Pins
#define RST_PIN 22    // Reset pin for RFID
#define SS_PIN 21     // SDA/SS pin for RFID
#define CLK_PIN 25    // SPI Clock
#define MISO_PIN 27   // SPI MISO
#define MOSI_PIN 26   // SPI MOSI

// DHT20 Sensor Pins
#define SDA_PIN 32    // SDA pin for I2C
#define SCL_PIN 33    // SCL pin for I2C

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID reader instance
Adafruit_AHTX0 aht;               // DHT20 sensor instance

// Item mapping (Assign RFID UIDs to specific items)
struct Item {
  String uid;
  String name;
  bool present;
};

// Define items with placeholders for RFID UIDs
Item fridgeItems[] = {
  {"", "Milk", false},
  {"", "Vegetables", false},
  {"", "Eggs", false}
};
const int itemCount = sizeof(fridgeItems) / sizeof(fridgeItems[0]);


void checkRFID() {
  // Check for a new RFID card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Read the UID of the RFID tag
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("Scanned RFID UID: ");
  Serial.println(uid);

  // Check if this UID is already assigned to an item
  for (int i = 0; i < itemCount; i++) {
    if (fridgeItems[i].uid == uid) {
      // Toggle presence status
      fridgeItems[i].present = !fridgeItems[i].present;
      Serial.print(fridgeItems[i].name);
      Serial.println(fridgeItems[i].present ? " is now IN the fridge." : " is now OUT of the fridge.");
      mfrc522.PICC_HaltA();
      return;
    }
  }

  // Assign new UID to an unassigned item
  for (int i = 0; i < itemCount; i++) {
    if (fridgeItems[i].uid == "") {
      fridgeItems[i].uid = uid;
      fridgeItems[i].present = true;
      Serial.print("Assigned RFID UID to ");
      Serial.println(fridgeItems[i].name);
      mfrc522.PICC_HaltA();
      return;
    }
  }

  // If all items are already assigned, notify the user
  Serial.println("No unassigned items available for this RFID UID.");
  mfrc522.PICC_HaltA();
}

void readEnvironment() {
  // Read temperature and humidity
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  // Print temperature and humidity
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" %"); 

  // Print fridge item statuses
  Serial.println("Fridge Inventory Status:");
  for (int i = 0; i < itemCount; i++) {
    Serial.print(fridgeItems[i].name);
    Serial.print(": ");
    Serial.println(fridgeItems[i].present ? "Present" : "Absent");
  }
  Serial.println("----------------------------");
}


void setup() {
  Serial.begin(9600);

  // Initialize RFID reader
  SPI.begin(CLK_PIN, MISO_PIN, MOSI_PIN);
  mfrc522.PCD_Init();
  Serial.println("RFID reader initialized!");
  mfrc522.PCD_DumpVersionToSerial();

  // Initialize DHT20 sensor
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!aht.begin()) {
    Serial.println("Could not find DHT20 sensor!");
    while (1); // Halt execution if sensor is not found
  }
  Serial.println("DHT20 sensor initialized!");

  Serial.println("Scan RFID cards to assign items...");
}

void loop() {
  // Check for RFID tag and handle item presence
  checkRFID();

  // Read temperature and humidity from DHT20
  readEnvironment();

  delay(2000); // Wait 2 seconds before the next loop
}
