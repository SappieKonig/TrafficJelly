#ifndef ACTION_GENERATOR_H
#define ACTION_GENERATOR_H

#include "utils.h"
#include "route.h"
#include "action.h"
#include "observation.h"

#include <memory>

/*
 * This is an action generator abstract base class to generate actions for a car to take.
 * This is required when cars are out of actions, or must process observations.
 */
class ActionGenerator
{
protected:
    optional_const_reference<Checkpoint> checkpoint;
    Car& actor;

public:
    ActionGenerator(Car& actor, optional_const_reference<Checkpoint> checkpoint);
    virtual ~ActionGenerator() = default;
    virtual std::unique_ptr<Action> generateAction(Observation& observation) = 0;
    void setCheckpoint(optional_const_reference<Checkpoint> checkpoint);
};

class BasicActionGenerator : public ActionGenerator
{
public:
    BasicActionGenerator(Car& actor, optional_const_reference<Checkpoint> checkpoint);
    std::unique_ptr<Action> generateAction(Observation& observation) override;
};


#endif
