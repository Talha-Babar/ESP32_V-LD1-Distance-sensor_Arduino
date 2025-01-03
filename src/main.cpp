#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define RX_PIN 13 // GPIO13
#define TX_PIN 14 // GPIO14

double binaryToDouble(uint64_t binaryValue)
{
  // Copy the binary value into a double variable
  double doubleValue;
  memcpy(&doubleValue, &binaryValue, sizeof(double));

  return doubleValue;
}

void sendData(const char *alphabet, uint32_t payloadSize, const uint8_t *payloadData)
{

  // Send the alphabet
  for (size_t i = 0; i < strlen(alphabet); i++)
  {
    Serial2.write((uint8_t)alphabet[i]);
  }

  // Send the payload size
  Serial2.write((const uint8_t *)&payloadSize, sizeof(uint32_t));

  // Send the payload data
  Serial2.write(payloadData, payloadSize);
}

uint64_t receiveMessage()
{
  char alphabet[5];
  uint32_t receivedPayloadSize;

  // Read the alphabet
  Serial2.readBytes(alphabet, 4);
  alphabet[4] = '\0'; // Null-terminate the string

  // Read the payload size
  Serial2.readBytes((char *)&receivedPayloadSize, sizeof(uint32_t));
  uint32_t payloadSize = receivedPayloadSize; // Convert back to host byte order

  // Read the payload data
  uint8_t payload[payloadSize];
  // uint64_t payloaddata;
  Serial2.readBytes(payload, payloadSize);

  // Process the received message
  Serial.print("Received Alphabet: ");
  Serial.println(alphabet);

  Serial.print("Received Payload Size: ");
  Serial.println(payloadSize);

  Serial.print("Received Payload Data: ");
  for (uint32_t i = 0; i < payloadSize; i++)
  {
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  uint64_t payloaddata = 0;
  for (uint32_t i = 0; i < payloadSize; i++)
  {
    payloaddata <<= 8 * i;
    payloaddata |= payload[i]; // OR with the new byte
  }

  return payloaddata;
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  uint8_t payload[] = {0x01};
  uint32_t payloadSize = sizeof(payload);
  uint64_t receivedata;

  sendData("INIT", payloadSize, payload);
  while (!Serial2.available())
  {
    Serial.println("Waiting for data: ");
  }

  // Read and process the acknowledgment message
  receiveMessage();

  // Wait for data message
  while (!Serial2.available())
  {
    Serial.println("Waiting for data: ");
  }

  // Read and process the data message
  receiveMessage();
}

void loop()
{
  uint8_t payload[] = {0x01};
  uint32_t payloadSize = sizeof(payload);
  uint64_t receivedata;
  float result;

  sendData("GNFD", payloadSize, payload);
  while (!Serial2.available())
  {
    Serial.println("Waiting for data: ");
  }

  // Read and process the acknowledgment message
  receiveMessage();

  // Wait for data message
  while (!Serial2.available())
  {
    Serial.println("Waiting for data: ");
  }

  // Read and process the data message
  receivedata = receiveMessage();
  result = binaryToDouble(receivedata);

  // Print the result
  Serial.print("Binary: ");
  Serial.println(receivedata, BIN);
  Serial.print("Distance: ");
  Serial.println(result, 4);

  // Other loop code
}
