#ifndef NN_FIXED__POINT_ACTICATION_H
#define NN_FIXED__POINT_ACTICATION_H

#include <fixed_point.h>
#include <math.h>

class fixedPointActivation
{
public:
    static inline int32_t ReLUFixedPoint(int32_t x)
    {
        return x > 0 ? x : 0;
    }

    static inline int32_t sigmoidFixedPoint(int32_t x)
    {
        const int32_t LOWER_BOUND = -30 * (1 << FP_SHIFT);
        const int32_t UPPER_BOUND = 30 * (1 << FP_SHIFT);

        if (x < LOWER_BOUND)
            return 0;
        if (x > UPPER_BOUND)
            return (1 << FP_SHIFT);

        int32_t exp_neg_x = fixedPointExpo(-x);

        return ((1 << FP_SHIFT) << FP_SHIFT) / ((1 << FP_SHIFT) + exp_neg_x);
    }

    static inline int32_t fixedPointExpo(int32_t x)
    {
        int32_t result = (1 << FP_SHIFT);
        int32_t term = (1 << FP_SHIFT);
        for (int i = 1; i <= 10; ++i)
        {
            term = (term * x) / (i * (1 << FP_SHIFT));
            result += term;
            if (term < 1)
                break;
        }
        return result;
    }
};

#endif