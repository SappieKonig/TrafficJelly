#ifndef ACTION_GENERATOR_H
#define ACTION_GENERATOR_H

#include "utils.h"
#include "route.h"
#include "action.h"
#include "observation.h"
#include "car_parameters.h"

#include <memory>

/*
 * This is an action generator interface to generate actions for a car to take.
 * This is required every time step in the processing of observations.
 * The action decision is based only on the car parameters (including the target checkpoint) and the observation.
 */
class ActionGenerator
{
public:
    ActionGenerator();
    virtual ~ActionGenerator();
    virtual std::unique_ptr<Action> generateAction(Observation const& observation) = 0;
};

class BasicActionGenerator : public ActionGenerator
{
private:
    std::shared_ptr<CarParameters> params;

public:
    BasicActionGenerator(std::shared_ptr<CarParameters> params);
    std::unique_ptr<Action> generateAction(Observation const& observation) override;
};

#endif
