#include <Arduino.h>

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
  Serial.begin(9600);
  // Other setup code
}

void loop()
{
  // Example payload data
  uint8_t payload[] = {0x01, 0x00, 0x00, 0x00};
  uint32_t payloadSize = sizeof(payload);

  sendData("INIT", payloadSize, payload);

  // Wait for acknowledgment
  while (!Serial.available())
  {
    // Wait for data to be available
  }

  // Read and process the acknowledgment message
  receiveMessage();

  // Wait for data message
  while (!Serial.available())
  {
    // Wait for data to be available
  }

  // Read and process the data message
  receiveMessage();

  // Other loop code
}
