#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "utils.h"
#include "action.h"

class ActionManager
{
public:
    virtual ~ActionManager() = default;
    virtual void record(Action& action) = 0;
    virtual void step() = 0;
};

class BasicActionManager : public ActionManager
{
private:
    Action& action;

public:
    BasicActionManager();
    void step() override;
};


#endif
