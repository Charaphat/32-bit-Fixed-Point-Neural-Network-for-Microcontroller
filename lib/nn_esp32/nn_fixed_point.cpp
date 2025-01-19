#include <nn_fixed_point.h>
#include <nn_fixed_point_acitivations.h>

nnFixedPoint::nnFixedPoint() : hiddenNeurons(nullptr), weights(nullptr), outputs(nullptr), biases(nullptr), hiddenLayers(0)
{
    this->activation = fixedPointActivation::ReLUFixedPoint;
    this->initialize(0, 0, nullptr, 0);
}

nnFixedPoint::~nnFixedPoint()
{
    if (this->weights)
        free(this->weights);
    if (this->biases)
        free(this->biases);
    if (this->outputs)
        free(this->outputs);
    if (this->hiddenNeurons)
        free(this->hiddenNeurons);
}

void nnFixedPoint::initialize(uint16_t inputNeurons, uint16_t hiddenLayers, const uint16_t hiddenNeurons[], uint16_t outputNeurons)
{
    this->inputNeurons = inputNeurons;
    this->hiddenLayers = hiddenLayers;
    this->outputNeurons = outputNeurons;

    if (this->hiddenLayers > 0)
    {
        this->hiddenNeurons = static_cast<uint16_t *>(malloc(sizeof(uint16_t) * this->hiddenLayers));
        if (!this->hiddenNeurons)
        {
            Serial.println("Memory allocation failed for hiddenNeurons");
            return;
        }

        for (uint16_t i = 0; i < this->hiddenLayers; i++)
        {
            this->hiddenNeurons[i] = hiddenNeurons[i];
        }
    }
    else
    {
        this->hiddenNeurons = nullptr;
    }

    this->totalNeurons = this->inputNeurons + this->outputNeurons;
    for (uint16_t i = 0; i < this->hiddenLayers; i++)
    {
        this->totalNeurons += this->hiddenNeurons[i];
    }

    this->totalWeights = 0;
    if (this->hiddenLayers > 0)
    {
        this->totalWeights += this->inputNeurons * this->hiddenNeurons[0];
        for (uint16_t i = 1; i < this->hiddenLayers; i++)
        {
            this->totalWeights += this->hiddenNeurons[i - 1] * this->hiddenNeurons[i];
        }
        this->totalWeights += this->hiddenNeurons[this->hiddenLayers - 1] * this->outputNeurons;
    }
    else
    {
        this->totalWeights += this->inputNeurons * this->outputNeurons;
    }

    this->totalBiases = 0;
    this->totalBiases += this->outputNeurons;
    if (this->hiddenLayers > 0)
    {
        for (uint16_t i = 0; i < this->hiddenLayers; i++)
        {
            this->totalBiases += this->hiddenNeurons[i];
        }
    }

    this->weights = static_cast<int32_t *>(malloc(sizeof(int32_t) * this->totalWeights));
    this->biases = static_cast<int32_t *>(malloc(sizeof(int32_t) * this->totalBiases));

    if (!this->weights || !this->biases)
    {
        Serial.println("Memory allocation failed for weights or biases");
        if (this->weights)
            free(this->weights);
        if (this->biases)
            free(this->biases);
        if (this->hiddenNeurons)
            free(this->hiddenNeurons);
        this->weights = nullptr;
        this->biases = nullptr;
        this->hiddenNeurons = nullptr;
        return;
    }
}

void nnFixedPoint::getNetworkDetail() const
{
#if defined(ARDUINO) && defined(ARDUINO_ARCH_ESP32)
    Serial.println("Network Structure:");
    Serial.print("Input Neurons: ");
    Serial.print(inputNeurons);
    Serial.println(" neurons");

    for (uint16_t i = 0; i < hiddenLayers; i++)
    {
        Serial.print("Hidden Layer ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(hiddenNeurons[i]);
        Serial.println(" neurons");
    }

    Serial.print("Outout Neurons: ");
    Serial.print(outputNeurons);
    Serial.println(" neurons");

    Serial.println("Weights:");
    if (weights)
    {
        for (size_t i = 0; i < totalWeights; i++)
        {
            Serial.print("Weight[");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(weights[i]);
        }
    }

    Serial.println("Biases:");
    if (biases)
    {
        for (size_t i = 0; i < totalBiases; ++i)
        {
            Serial.print("Bias[");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(biases[i]);
        }
    }

    Serial.println();
#endif
}

void nnFixedPoint::initializeWeights(float *weightsArray, size_t size)
{
    if (!weightsArray)
    {
        Serial.println("Invalid weights array provided");
        return;
    }

    int32_t *fixedPointWeightsArray = floatArrayToFixedPointArray(weightsArray, size);

    for (uint16_t i = 0; i < this->totalWeights; i++)
    {
        this->weights[i] = fixedPointWeightsArray[i];
    }

    free(fixedPointWeightsArray);
}

void nnFixedPoint::initializeBiases(float *biasesArray, size_t size)
{
    if (!biasesArray)
    {
        Serial.println("Invalid biases array provided");
        return;
    }

    int32_t *fixedPointBiasesArray = floatArrayToFixedPointArray(biasesArray, size);

    for (uint16_t i = 0; i < this->totalBiases; i++)
    {
        this->biases[i] = fixedPointBiasesArray[i];
    }

    free(fixedPointBiasesArray);
}

int32_t *nnFixedPoint::inference(float *inputs)
{
    int32_t *weights = this->weights;
    int32_t *biases = this->biases;

    int32_t *inputsFixedPoint = floatArrayToFixedPointArray(inputs, this->inputNeurons);
    if (!inputsFixedPoint)
    {
        Serial.println("Memory allocation failed for inputsFixedPoint");
        return nullptr;
    }

    if (this->outputs)
    {
        free(this->outputs);
    }
    this->outputs = static_cast<int32_t *>(malloc(sizeof(int32_t) * this->outputNeurons));
    if (!this->outputs)
    {
        Serial.println("Memory allocation failed for outputs");
        free(inputsFixedPoint);
        return nullptr;
    }

    int32_t totalIntermediateNeurons = 0;
    for (uint16_t i = 0; i < this->hiddenLayers; i++)
    {
        totalIntermediateNeurons += this->hiddenNeurons[i];
    }
    int32_t *outputBuffer = static_cast<int32_t *>(malloc(sizeof(int32_t) * totalIntermediateNeurons));
    if (!outputBuffer)
    {
        Serial.println("Memory allocation failed for outputBuffer");
        free(inputsFixedPoint);
        return nullptr;
    }

    int32_t *currentInputs = inputsFixedPoint;
    int32_t *currentOutputs = outputBuffer;
    int32_t *layerOutputs = currentOutputs;
    int32_t sum = 0;

    for (uint16_t i = 0; i < this->hiddenNeurons[0]; ++i)
    {
        sum = 0;
        for (uint16_t j = 0; j < this->inputNeurons; ++j)
        {
            sum += fixedPointMultiply(currentInputs[j], *weights++);
        }
        sum += *biases++;
        *currentOutputs++ = activation(sum);
    }

    currentInputs = layerOutputs;           // Update currentInputs to point to the outputs of the first hidden layer
    layerOutputs += this->hiddenNeurons[0]; // Advance layerOutputs for the next layer

    // **2. Hidden Layers**
    for (uint16_t h = 1; h < this->hiddenLayers; ++h)
    {
        for (uint16_t i = 0; i < this->hiddenNeurons[h]; ++i)
        {
            sum = 0; // Reset sum for each neuron
            for (uint16_t j = 0; j < this->hiddenNeurons[h - 1]; ++j)
            {
                sum += fixedPointMultiply(currentInputs[j], *weights++); // Weighted sum
            }
            sum += *biases++;                    // Add bias
            *currentOutputs++ = activation(sum); // Apply activation function and store result
        }

        currentInputs = layerOutputs;           // Update currentInputs to point to the outputs of the current hidden layer
        layerOutputs += this->hiddenNeurons[h]; // Advance layerOutputs for the next layer
    }

    // **3. Hidden Layer to Output Layer**
    for (uint16_t i = 0; i < this->outputNeurons; ++i)
    {
        sum = 0; // Reset sum for each neuron
        for (uint16_t j = 0; j < this->hiddenNeurons[this->hiddenLayers - 1]; ++j)
        {
            sum += fixedPointMultiply(currentInputs[j], *weights++); // Weighted sum
        }
        sum += *biases++;                   // Add bias
        this->outputs[i] = activation(sum); // Apply activation function and store result
    }

    for (uint16_t i = 0; i < outputNeurons; i++)
    {
        Serial.println(outputs[i]);
    }
    // Free temporary buffers
    free(inputsFixedPoint);
    free(outputBuffer);

    return this->outputs;
}