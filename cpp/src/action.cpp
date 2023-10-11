#include "action.h"
#include "car.h"
#include <memory>

Action::Action() = default;

void CruiseAction::apply(Car& ego, float dt) {

}

void HardBrakeAction::apply(Car& ego, float dt) {
    ego.hardBrake(dt);
}

void ToLeftLaneAction::apply(Car& ego, float dt) {
    ego.toLeftLane();
}

void ToRightLaneAction::apply(Car& ego, float dt) {
    ego.toRightLane();
}

void CompositeAction::apply(Car& ego, float dt) {
    for (auto &action : actions) {
        action->apply(ego, dt);
    }
}

CompositeAction::CompositeAction(std::vector<std::unique_ptr<Action>> actions) {
    this->actions = std::move(actions);
}


