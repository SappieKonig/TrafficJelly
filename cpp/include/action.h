#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>

#include "utils.h"

/*
 * This is an action abstract base class for cars to take.
 * Actions are applied to cars and may affect internal parameters.
 * In that case, the derivation must be befriended by Car.
 */
class Action
{
public:
    Action();
    virtual void apply(Car& ego, float dt) = 0;
    virtual ~Action() = default;
};

class CruiseAction : public Action
{
public:
    void apply(Car& ego, float dt) override;
};

class HardBrakeAction : public Action
{
public:
    void apply(Car& ego, float dt) override;
};

class ToLeftLaneAction : public Action
{
public:
    void apply(Car& ego, float dt) override;
};

class ToRightLaneAction : public Action
{
public:
    void apply(Car& ego, float dt) override;
};

/*
 * This action carries out multiple actions in a row.
 */
class CompositeAction : public Action
{
private:
    std::vector<std::unique_ptr<Action>> actions;

public:
    CompositeAction(std::vector<std::unique_ptr<Action>> actions);
    void apply(Car& ego, float dt) override;
};

#endif
