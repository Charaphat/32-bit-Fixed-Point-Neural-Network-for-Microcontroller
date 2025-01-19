#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>
#include <stdlib.h>

#define FP_SHIFT 12

static inline int32_t floatToFixedPoint(float x)
{
    return (int32_t)(x * (1 << FP_SHIFT));
}

static inline float fixedPointToFloat(int32_t x)
{
    return (float)x / (1 << FP_SHIFT);
}

static inline int32_t *floatArrayToFixedPointArray(float *floatArray, size_t size)
{
    int32_t *fixedPointArray = static_cast<int32_t *>(malloc(sizeof(int32_t) * size));

    if (!fixedPointArray)
    {
        Serial.println("Memory allocation failed for fixedPointArray");
        return nullptr;
    }

    for (size_t i = 0; i < size; i++)
    {
        fixedPointArray[i] = static_cast<int32_t>(floatArray[i] * (1 << FP_SHIFT));
    }

    return fixedPointArray;
}

static inline float *fixedPointArrayToFloatArray(int32_t *fixedPointArray, size_t size)
{
    float *floatArray = static_cast<float *>(malloc(sizeof(float) * size));

    if (!floatArray)
    {
        Serial.println("Memory allocation failed for floatArray");
        return nullptr;
    }

    for (size_t i = 0; i < size; i++)
    {
        floatArray[i] = static_cast<float>(fixedPointArray[i]) / (1 << FP_SHIFT);
    }

    return floatArray;
}

static inline int32_t fixedPointAdd(int32_t a, int32_t b)
{
    return a + b;
}

static inline int32_t fixedPointSubtract(int32_t a, int32_t b)
{
    return a - b;
}

static inline float fixedPointMultiply(int32_t a, int32_t b)
{
    int32_t buffer = (int32_t)a * (int32_t)b;
    return (int32_t)(buffer >> FP_SHIFT);
}

static inline float fixedPointDivide(int32_t a, int32_t b)
{
    int32_t buffer = ((int32_t)a << FP_SHIFT) / b;
    return (int32_t)buffer;
}

#endif