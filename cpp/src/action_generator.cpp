#include "action_generator.h"
#include <memory>

ActionGenerator::ActionGenerator()
{

}

ActionGenerator::~ActionGenerator()
{

}

BasicActionGenerator::BasicActionGenerator(std::shared_ptr<CarParameters> params)
    : params(params)
{

}

std::unique_ptr<Action> BasicActionGenerator::generateAction(Observation const& observation)
{
    return std::make_unique<DriveAction>(params);
}
