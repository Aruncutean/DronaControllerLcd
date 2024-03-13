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
  // radio.setOutputPower(20);

  LoRaWANNode node(&radio, &EU868);

  uint64_t joinEUI = 0x3EA63435A61D7B5A;

  uint64_t devEUI = 0x3EA63435A61D7B5B;

  uint8_t nwkKey[] = {0xB6, 0xD7, 0x37, 0xEE, 0xFA, 0x73, 0x77, 0x93, 0x11, 0x05, 0x32, 0xF5, 0x7F, 0xBE, 0xF8, 0x71};

  uint8_t appKey[] = {0x40, 0x9F, 0x0B, 0xA2, 0x17, 0xFC, 0x67, 0x7B, 0x64, 0x15, 0xB5, 0x1D, 0x28, 0xD1, 0xF6, 0xD1};

  printf("[LoRaWAN] Attempting over-the-air activation ... ");
  while (true)
  {
    state = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
    if (state >= RADIOLIB_ERR_NONE)
    {
      printf("success!\n");
      break;
    }
    else
    {
      printf("failed, code ");
      std::cout << state << std::endl;
    }
  }

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
    // Creează un mesaj pentru a trimite
    uint8_t message[] = "Hello LoRaWAN!";
    uint8_t downlinkData[64];
    size_t downlinkSize = sizeof(downlinkData);
    printf("[LoRaWAN] Transmitting packet...\n");
    state = node.sendReceive(message, sizeof(message), 10, downlinkData, &downlinkSize, false);
    if (state == RADIOLIB_ERR_NONE) {
      printf("Packet transmitted successfully!\n");
      if (downlinkSize > 0) {
        // Procesează datele primite
        printf("Received downlink: ");
        for (size_t i = 0; i < downlinkSize; i++) {
          printf("%02X ", downlinkData[i]);
        }
        printf("\n");
      }
    } else {
      printf("Packet transmission failed, code %d\n", state);
    }

    // Așteaptă un interval adecvat între transmisii, respectând regulile de duty cycle
    hal->delay(5000);

    // std::stringstream ss;
    // ss << " [# " << messageNumber << "]"
    //    << ": " << messageContent;

    // std::string combinedMessage = ss.str();
    // send(radio, combinedMessage);
    // messageNumber = messageNumber + 1;
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
