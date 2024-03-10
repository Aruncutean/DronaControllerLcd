#include <iostream>
#include <string>
#include <sstream>
#include <RadioLib.h>
#include "PiHal.h"

PiHal *hal = new PiHal(0);

// NSS pin:   25
// DIO1 pin:  4
// NRST pin:  17
// BUSY pin:  27 //nu se foloseste nu merge rcv-ul pentru RFM95
RFM95 radio = new Module(hal, 25, 4, 17);

// NSS pin:   12
// DIO1 pin:  5
// NRST pin:  13
// BUSY pin:  6
SX1280 radio1 = new Module(hal, 12, 5, 13, 6);

template <typename T>
void send(T radio, std::string message);

template <typename T>
void recv(T radio);

int main(int argc, char **argv)
{
  if (gpioInitialise() < 0)
  {
    fprintf(stderr, "Nu s-a putut inițializa pigpio\n");
    return 1;
  }
  printf("[RFM95] Initializing ... ");
  int state = radio.begin(868.0);
  if (state != RADIOLIB_ERR_NONE)
  {
    printf("failed, code %d\n", state);
    return (1);
  }
  printf("success!\n");
  radio.setOutputPower(20);

  printf("[SX1280] Initializing ... ");
  state = radio1.begin();
  if (state != RADIOLIB_ERR_NONE)
  {
    printf("failed, code %d\n", state);
    return (1);
  }
  printf("success!\n");
  uint8_t c = 0;

  int messageNumber = 1;
  std::string messageContent = "Hello World!";

  for (;;)
  {

    std::stringstream ss;
    ss << " [# " << messageNumber << "]"
       << ": " << messageContent;

    std::string combinedMessage = ss.str();
    send(radio, combinedMessage);
    messageNumber = messageNumber + 1;
  }

  return (0);
}

template <typename T>
void send(T radio, std::string message)
{
  const char *transmitBuffer = message.c_str();
  // send a packet
  printf("[SX1262] Transmitting packet ... ");
  std::cout << transmitBuffer;
  int state = radio.transmit(transmitBuffer);
  if (state == RADIOLIB_ERR_NONE)
  {
    printf("success!\n");
  }
  else
  {
    printf("failed, code %d\n", state);
  }
}

template <typename T>
void recv(T radio)
{
  uint8_t rfm95_buf[256];
  memset(rfm95_buf, 0, sizeof(rfm95_buf));
  int state = radio.receive(rfm95_buf, sizeof(rfm95_buf) - 1);

  if (state == RADIOLIB_ERR_NONE)
  {
    rfm95_buf[sizeof(rfm95_buf) - 1] = '\0';
    std::cout << "Received: " << (char *)rfm95_buf << std::endl;
    std::cout << "[LoRaWAN] RSSI:\t\t";
    std::cout << radio.getRSSI();
    std::cout << " dBm" << std::endl;

    // print SNR (Signal-to-Noise Ratio)
    std::cout << "[LoRaWAN] SNR:\t\t";
    std::cout << radio.getSNR();
    std::cout << " dB" << std::endl;

    // print frequency error
    std::cout << "[LoRaWAN] Frequency error:\t";
    std::cout << radio.getFrequencyError();
    std::cout << " Hz" << std::endl;
  }
  else if (state == RADIOLIB_ERR_RX_TIMEOUT)
  {
    // std::cout << "Reception timed out" << std::endl;
  }
  else
  {
    std::cout << "Receiving error: " << state << std::endl;
  }
}