#include "action.h"
#include "car.h"
#include <memory>

Action::Action(std::shared_ptr<CarParameters> params)
    : params(params)
{

}

Action::~Action()
{

}

NullAction::NullAction(std::shared_ptr<CarParameters> params)
    : Action(params)
{

}

void NullAction::apply(float dt)
{

}

DriveAction::DriveAction(std::shared_ptr<CarParameters> params)
    : Action(params)
{

}

void DriveAction::apply(float dt)
{
    params->x += params->v * dt;
}

WaitAction::WaitAction(std::shared_ptr<CarParameters> params)
    : Action(params)
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

CompositeAction::CompositeAction(std::shared_ptr<CarParameters> params, std::vector<std::reference_wrapper<Action>> actions)
    : Action(params), actions(actions)
{
    
}

void CompositeAction::apply(float dt)
{
    for (Action& action : actions)
    {
        action.apply(dt);
    }
}
