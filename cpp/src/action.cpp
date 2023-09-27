#include "action.h"
#include "car.h"

Action::Action(Car& actor)
    : actor(actor)
{

}

NullAction::NullAction(Car& actor)
    : Action(actor)
{

}

void NullAction::apply(float dt)
{

}

DriveAction::DriveAction(Car& actor)
    : Action(actor)
{

}

void DriveAction::apply(float dt)
{
    actor.x += actor.v * dt;
}

WaitAction::WaitAction(Car& actor)
    : Action(actor)
{

}

void WaitAction::apply(float dt)
{
    age += dt;
    if (age >= duration)
    {
        delete this;
    }
}

EdgeChoiceAction::EdgeChoiceAction(Car& actor)
    : Action(actor)
{

}

void EdgeChoiceAction::apply(float dt)
{

}

CompositeAction::CompositeAction(Car& actor, std::vector<std::reference_wrapper<Action>> actions)
    : Action(actor), actions(actions)
{
    
}

void CompositeAction::apply(float dt)
{
    for (Action& action : actions)
    {
        action.apply(dt);
    }
}
