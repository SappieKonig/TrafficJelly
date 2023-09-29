#ifndef CAR_PARAMETERS_H
#define CAR_PARAMETERS_H

#include "utils.h"
#include "route.h"
#include <random>

/*
 * All car paramters are captured in this for the actions to manipulate, rather than the car itself.
 * Might benefit from being abstract. Already taken into account by using smart pointers.
 */
struct CarParameters
{
    float x = 0; // in km
    float v = 1; // in km/h

    std::shared_ptr<Checkpoint> targetCheckpoint = nullptr;
    bool isWaiting = false;
};

struct CarParametersInitializer
{
private:
    std::normal_distribution<float> x_distribution;
    std::normal_distribution<float> v_distribution;

public:
    CarParametersInitializer(float x_mean, float x_std, float v_mean, float v_std);
    void operator()(std::shared_ptr<CarParameters>& carParameters, std::default_random_engine& generator);
};

#endif
