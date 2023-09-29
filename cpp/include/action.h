#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>

#include "car_parameters.h"
#include "utils.h"

/*
 * This is an action abstract base class for cars to take.
 * Actions are applied to cars and may affect internal parameters.
 * In that case, the derivation must be befriended by Car.
 */
class Action
{
protected:
    std::shared_ptr<CarParameters> params;

public:
    Action(std::shared_ptr<CarParameters> params);
    virtual ~Action();
    virtual void apply(float dt) = 0;
};

/*
 * This action does nothing.
 */
class NullAction : public Action
{
public:
    NullAction(std::shared_ptr<CarParameters> params);
    void apply(float dt) override;
};

/*
 * This action moves the car by its velocity over a time step.
 */
class DriveAction : public Action
{
public:
    DriveAction(std::shared_ptr<CarParameters> params);
    void apply(float dt) override;
};

/*
 * This action lets the car wait for a given duration.
 */
class WaitAction : public Action
{
private:
    float duration;
    float age = 0;

public:
    WaitAction(std::shared_ptr<CarParameters> params);
    void apply(float dt) override;
};

/*
 * This action carries out multiple actions in a row.
 */
class CompositeAction : public Action
{
private:
    std::vector<std::reference_wrapper<Action>> actions;

public:
    CompositeAction(std::shared_ptr<CarParameters> params, std::vector<std::reference_wrapper<Action>> actions);
    void apply(float dt) override;
};

#endif
