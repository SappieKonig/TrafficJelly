#include "action_manager.h"

ActionManager::~ActionManager()
{

}

BasicActionManager::BasicActionManager(std::shared_ptr<CarParameters> params)
    : action(std::make_unique<NullAction>(params))
{

}

void BasicActionManager::manage(std::unique_ptr<Action>&& newAction)
{
    action = std::move(newAction);
}

void BasicActionManager::step(float dt)
{
    action->apply(dt);
}
