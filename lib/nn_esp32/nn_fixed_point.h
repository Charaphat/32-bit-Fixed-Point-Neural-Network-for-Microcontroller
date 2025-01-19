#ifndef NN_int32_t_H
#define NN_int32_t_H

#include <Arduino.h>
#include <fixed_point.h>


class nnFixedPoint
{
private:
    uint16_t inputNeurons;
    uint16_t *hiddenNeurons;
    uint16_t hiddenLayers;
    uint16_t outputNeurons;

    uint16_t totalWeights;
    uint16_t totalBiases;
    uint16_t totalNeurons;

    int32_t *weights;
    int32_t *outputs;
    int32_t *biases;

    int32_t (*activation)(int32_t);

public:
    nnFixedPoint();
    ~nnFixedPoint();

    void initialize(uint16_t inputNeurons, uint16_t hiddenLayers, const uint16_t hiddenNeurons[], uint16_t outputNeurons);

    void getNetworkDetail() const;

    void initializeWeights(float *weightsArray, size_t size);

    void initializeBiases(float *biasesArray, size_t size);

    int32_t* inference(float *inputs);
};

#endif