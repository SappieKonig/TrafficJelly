#ifndef ACTION_GENERATOR_H
#define ACTION_GENERATOR_H

#include "utils.h"
#include "route.h"
#include "action.h"

/*
 * This is an action generator abstract base class to generate actions for a car to take.
 * This is required when cars are out of actions, or must process observations.
 */
class ActionGenerator
{
private:
    optional_const_reference<Checkpoint> checkpoint;

public:
    ActionGenerator(optional_const_reference<Checkpoint> checkpoint);
    virtual ~ActionGenerator() = default;
    virtual Action generateAction() = 0;
    void setCheckpoint(optional_const_reference<Checkpoint> checkpoint);
};

class BasicActionGenerator : public ActionGenerator
{
public:
    BasicActionGenerator(optional_const_reference<Checkpoint> checkpoint);
    Action generateAction() override;
};


#endif
