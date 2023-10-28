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
    float x = 0; // in m
    float baseTarget = 0; // in m/s
    float v; // in m/s (starts at speed limit of current edge (or maybe not, idfk))
    float offset = 0; // in m/s (to be added to v, with respect to the speed limit)
    int lane; // in number of lanes (starts at 0) (lane 0 is the rightmost lane)

    std::shared_ptr<Checkpoint> targetCheckpoint = nullptr;
    bool isWaiting = false;
};

struct CarParametersInitializer
{
private:
    std::normal_distribution<float> v_distribution;

public:
    CarParametersInitializer(float v_mean, float v_std);
    void operator()(CarParameters& carParameters, std::default_random_engine& generator);
};

#endif
