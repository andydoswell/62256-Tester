/* 62256 SRAM Tester

Tests every pattern in every location */

#include "Arduino.h"

const char DATA_BUS[] = {A0, A1, A2, A3, A4, A5, 9, 10};
const char NOT_WE = 6;
const char NOT_OE = 5;
const char NOT_CS = 4;
const char ADDRESS_RST = 3;
const char ADDRESS_INC = 2;
const char PASS_LED = 8;
const char FAIL_LED = 7;
const unsigned int MEMORY_SIZE = 32768;
unsigned int address;
bool fail = false;

void setDataPinsIn()
{
  for (int i = 0; i < 8; i++)
  {
    pinMode(DATA_BUS[i], INPUT_PULLUP);
  }
}

void setDataPinsOut()
{
  for (int i = 0; i < 8; i++)
  {
    pinMode(DATA_BUS[i], OUTPUT);
  }
}

void resetAddress()
{
  digitalWrite(ADDRESS_RST, LOW);
  delay(1);
  digitalWrite(ADDRESS_RST, HIGH);
}

void resetRAM()
{
  digitalWrite(NOT_WE, HIGH);
  digitalWrite(NOT_OE, HIGH);
  digitalWrite(NOT_CS, HIGH);
}

void incrementAddress()
{
  digitalWrite(ADDRESS_INC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ADDRESS_INC, LOW);
  delayMicroseconds(10);
}

byte readData()
{
  byte result = 0;
  for (int i = 0; i < 8; i++)
  {
    bitWrite(result, i, digitalRead(DATA_BUS[i]));
  }
  return result;
}

// Print binary value from the 8 bit data
void printBinary(byte data)
{
  for (int b = 7; b >= 0; b--)
  {
    Serial.print(bitRead(data, b));
  }
}

void setup()
{
  pinMode(NOT_WE, OUTPUT);
  pinMode(NOT_OE, OUTPUT);
  pinMode(NOT_CS, OUTPUT);
  pinMode(PASS_LED, OUTPUT);
  pinMode(FAIL_LED, OUTPUT);
  pinMode(ADDRESS_INC, OUTPUT);
  pinMode(ADDRESS_RST, OUTPUT);
  resetRAM();
  Serial.begin(57600);
  Serial.println("62256 SRAM Tester by Doz.");
}

void loop()
{
  digitalWrite(NOT_CS, LOW);
  resetAddress();
  digitalWrite (PASS_LED, HIGH); // both LEDS on for test in progress.
  digitalWrite (FAIL_LED,HIGH);
  // Write pattern to all locations
  for (byte pattern = 0; pattern < 255; pattern++)
  {
    Serial.print("Running test pattern ");
    printBinary(pattern);
    Serial.println();

    for (address = 0; address < MEMORY_SIZE+1; address++)
    {
      setDataPinsOut(); // set data bus to output (on arduino)
      digitalWrite(NOT_OE, HIGH); // disable output
      digitalWrite(NOT_WE, LOW);  // Enable write
       for (int i = 0; i < 8; i++)
      {
        if (bitRead(pattern, i))
        {
          digitalWrite(DATA_BUS[i], HIGH);
        }
        else
        {
          digitalWrite(DATA_BUS[i], LOW);
        }
      }
      delayMicroseconds(1);
      digitalWrite(NOT_WE, HIGH); // disable write
      setDataPinsIn(); // set data bus to receive (on arduino)
      digitalWrite(NOT_OE, LOW); // output enable

      // Read data back in

      byte data = readData();

      if (data != pattern)
      {
        fail = true;
        Serial.print("ERROR at ");
        Serial.print("0x");
        Serial.print(address, HEX);
        Serial.print(" - Got: ");
        printBinary(data);
        Serial.print(" Expected: ");
        printBinary(pattern);
        Serial.println();
        digitalWrite(FAIL_LED, HIGH);
        digitalWrite(PASS_LED,LOW);
      }
       incrementAddress();
    }
  }
  
  Serial.println("All tests complete");
  
  if (!fail)
  {
    Serial.println("PASS");
    digitalWrite(PASS_LED, HIGH);
    digitalWrite(FAIL_LED,LOW);
  }
  else {
    Serial.println("FAIL");
  }
  Serial.println("Hit reset to re-run");
  while (1);
}
