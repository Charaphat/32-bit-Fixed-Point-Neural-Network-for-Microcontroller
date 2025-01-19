#include <Arduino.h>
#include "nn_esp32.h"

void setup()
{
    Serial.begin(115200);
    nn_network network;
    uint16_t hiddenNeurons[2] = {3, 4};
    network.initialize(2, 2, hiddenNeurons, 1);
    network.getNetworkDetail();
}

void loop()
{
}