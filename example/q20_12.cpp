#include <Arduino.h>
#include "q20_12.h"

void setup()
{
    Serial.begin(115200);
    float floatArray[3] = {32.54, 1.34, 0.25};
    Serial.println("Original float array:");
    for (size_t i = 0; i < 3; i++)
    {
        Serial.println(floatArray[i]);
    }
    q20_12 *fixedPointArray = floatArrayToQ20_12Array(floatArray);
    Serial.println("\nConverted to q20_12:");
    for (size_t i = 0; i < 3; i++)
    {
        Serial.println(fixedPointArray[i]);
    }

    q20_12 addResult = q20_12_add(fixedPointArray[0], fixedPointArray[1]);
    q20_12 subResult = q20_12_subtract(fixedPointArray[0], fixedPointArray[1]);
    q20_12 mulResult = q20_12_multiply(fixedPointArray[0], fixedPointArray[2]);
    q20_12 divResult = q20_12_divide(fixedPointArray[0], fixedPointArray[2]);

    Serial.println("\nArithmetic results (converted back to float):");
    Serial.print("Addition: ");
    Serial.println(q20_12_to_float(addResult));
    Serial.print("Subtraction: ");
    Serial.println(q20_12_to_float(subResult));
    Serial.print("Multiplication: ");
    Serial.println(q20_12_to_float(mulResult));
    Serial.print("Division: ");
    Serial.println(q20_12_to_float(divResult));

    free(fixedPointArray);
}

void loop()
{
}