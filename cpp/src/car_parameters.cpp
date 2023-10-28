#include "car_parameters.h"

CarParametersInitializer::CarParametersInitializer(float v_mean, float v_std)
    : v_distribution(v_mean, v_std)
{

}

void CarParametersInitializer::operator()(CarParameters& carParameters, std::default_random_engine& generator)
{
    carParameters.v = v_distribution(generator);
}
