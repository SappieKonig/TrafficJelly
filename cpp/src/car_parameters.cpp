#include "car_parameters.h"

CarParametersInitializer::CarParametersInitializer(float x_mean, float x_std, float v_mean, float v_std)
    : x_distribution(x_mean, x_std), v_distribution(v_mean, v_std)
{

}

void CarParametersInitializer::operator()(std::shared_ptr<CarParameters>& carParameters, std::default_random_engine& generator)
{
    carParameters->x = x_distribution(generator);
    carParameters->v = v_distribution(generator);
}

