#include <Arduino.h>
#include "nn_fixed_point.h"

void setup()
{
  Serial.begin(115200);
  nnFixedPoint network;
  uint16_t hiddenNeurons[1] = {4};
  network.initialize(2, 1, hiddenNeurons, 1);

  float weights[12] = {-1.0458469, 1.0628817, 1.153841, -1.125687, 0.20513678, 0.01769366, -0.07436187, 0.24505346, 0.88013214, 0.97230804, -0.406285, 0.33086073};
  float biases[5] = {0.00439999, -0.00202769, 0.29493982, 0.5703486, -0.07985158};

  network.initializeWeights(weights, 12);
  network.initializeBiases(biases, 5);
  network.getNetworkDetail();

  float inputs1[2] = {1, 1};
  float inputs2[2] = {0, 0};
  float inputs3[2] = {1, 0};
  float inputs4[2] = {0, 1};

  int32_t *output1 = network.inference(inputs1);
  int32_t *output2 = network.inference(inputs2);
  int32_t *output3 = network.inference(inputs3);
  int32_t *output4 = network.inference(inputs4);

  float *output1_float = fixedPointArrayToFloatArray(output1, 1);
  Serial.println(output1_float[0]);
}

void loop()
{
}