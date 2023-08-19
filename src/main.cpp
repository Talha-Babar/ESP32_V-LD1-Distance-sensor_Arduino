#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

double binaryToDouble(uint64_t binaryValue)
{
  // Copy the binary value into a double variable
  double doubleValue;
  memcpy(&doubleValue, &binaryValue, sizeof(double));

  return doubleValue;
}

void sendData(const char *alphabet, uint32_t payloadSize, const uint8_t *payloadData)
{
  // Calculate the payload size in network byte order (big-endian)
  uint32_t payloadSizeNetwork = payloadSize;

  // Send the alphabet
  Serial.write((const uint8_t *)alphabet, 4);

  // Send the payload size
  Serial.write((const uint8_t *)&payloadSizeNetwork, sizeof(uint32_t));

  // Send the payload data
  Serial.write(payloadData, payloadSize);
}

uint64_t receiveMessage()
{
  char alphabet[5];
  uint32_t receivedPayloadSize;

  // Read the alphabet
  Serial.readBytes(alphabet, 4);
  alphabet[4] = '\0'; // Null-terminate the string

  // Read the payload size
  Serial.readBytes((char *)&receivedPayloadSize, sizeof(uint32_t));
  uint32_t payloadSize = receivedPayloadSize; // Convert back to host byte order

  // Read the payload data
  uint8_t payload[payloadSize];
  // uint64_t payloaddata;
  Serial.readBytes(payload, payloadSize);

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
  uint8_t payload[] = {0x01};
  uint32_t payloadSize = sizeof(payload);
  uint64_t receivedata;

  sendData("INIT", payloadSize, payload);
  while (!Serial.available())
  {
    Serial.println("Waiting for data: ");
  }

  // Read and process the acknowledgment message
  receiveMessage();

  // Wait for data message
  while (!Serial.available())
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
  while (!Serial.available())
  {
    Serial.println("Waiting for data: ");
  }

  // Read and process the acknowledgment message
  receiveMessage();

  // Wait for data message
  while (!Serial.available())
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
