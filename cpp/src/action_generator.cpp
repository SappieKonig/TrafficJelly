#include "action_generator.h"

ActionGenerator::ActionGenerator(Car& actor, optional_const_reference<Checkpoint> checkpoint)
    : actor(actor), checkpoint(checkpoint)
{

}

void ActionGenerator::setCheckpoint(optional_const_reference<Checkpoint> newCheckpoint)
{
    checkpoint = newCheckpoint;
}

BasicActionGenerator::BasicActionGenerator(Car& actor, optional_const_reference<Checkpoint> checkpoint)
    : ActionGenerator(actor, checkpoint)
{

}

std::unique_ptr<Action> BasicActionGenerator::generateAction(Observation& observation)
{
    return std::unique_ptr<Action>(new DriveAction(actor));
}
