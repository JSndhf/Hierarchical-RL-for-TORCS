#include "TreeNodes.h"

/*****************************************************************************/
/****** BASIC TASK IMPLEMENTATION ********************************************/
/*****************************************************************************/
Task::Task(){};
Task::~Task(){};
// Absolute minimalistic implementations for all methods, so derived classes
// like the PrimitiveAction do not need an own implementation.
void Task::learn(){ return; };
double Task::getMaxQValue(string featureValues){ return 0.0; };
shared_ptr<Task> Task::getActionSelection(string featureValues, vector<shared_ptr<Task>>& allowedActions){ return nullptr; };
// Basic stringify method
string Task::toString(){
    ostringstream s;
    if(this->isPrimitive){
        s << "<" << this->name << " | " << this->id << ">";
    } else if(this->isStatic){
        s << "[" << this->name << " | " << this->id << "]";
    } else {
        s << "[*" << this->name << " | " << this->id << "*]";
    }
    return s.str();
};

/*****************************************************************************/
/****** DYNAMIC TASK IMPLEMENTATION ******************************************/
/*****************************************************************************/
DynamicTask::DynamicTask(char id, string name, double alphaStart, double gamma, double epsilon, int totalActionCount):
    _alphaStart(alphaStart),_gamma(gamma),_epsilon(epsilon),_totalActionCount(totalActionCount){
        this->id = id; this->name = name; this->isPrimitive = false; this->isStatic = false;
};

DynamicTask::~DynamicTask(){};

double DynamicTask::_getPseudoReward(string featureValues){
    return 0.0; // No pseudo-reward atm
};

/*** learn *********************************************************
    Performs the actual learning algorithm, updating the internally
    stored Q-/Completion values, taking into account the actual reward
    gained and possibly a pseudo-reward enforcing the behavior towards
    reaching goal states.

    As rewards observed mirror the success/failure of an action taken
    in the last state, the update is performed backwards.
********************************************************************/
void DynamicTask::learn(){
    // Check if the state-action pairs used for calculation were already visited
    // and create them if not

    // Get the internal (pseudo-)reward for the last state-action pair

    // Check, if the last and the current chosen actions are primitive or subtasks,
    // and get the subtasks best action value

    // Update the C_i and C~_i values for the last state-action pair

    // External completion
    // C_currTask,lastState,new = (1-a) C_currTask,lastState,old + a [C_currTask,currState,bestAction,old + Reward/ Q_subTask,lastState,bestAction,old]

    // Internal completion including the pseudo-rewards:
    // C~_currTask,lastState,new = (1-a) C~_currTask,lastState,old + a [PseudoReward_currState + C~_currTask,currState,bestAction,old + Reward/ Q_subTask,lastState,bestAction,old]

    // Remember the current features and chosen action

};

/*** getQValue *****************************************************
    Returns the externally visible Q-Value/Completion for a given
    state and it's best action.
********************************************************************/
double DynamicTask::getMaxQValue(string featureValues){
    // Loop through the map of cvals and check, if it contains the given feature
    double currBestValue = 0.0;
    int aCnt = 0;
    for(auto const &cval : this->cvals){
        // If all actions possible has been found, the algorithm can stop savely
        if((cval.first.find(featureValues) != string::npos) && aCnt < this->_totalActionCount){
            // If the value is better than the current one, update.
            currBestValue = cval.second[0] > currBestValue ? cval.second[0] : currBestValue;
            aCnt++;
        } else {
            break;
        }
    }
    return currBestValue;
};

/*** getActionSelection *********************************************
    Implements the strategy to select actions either by finding the
    best state-action-value or deploying exploration
********************************************************************/
shared_ptr<Task> DynamicTask::getActionSelection(string featureValues, vector<shared_ptr<Task>>& allowedActions){
    return nullptr;
};

/*****************************************************************************/
/****** PRIMITIVE ACTION IMPLEMENTATION **************************************/
/*****************************************************************************/
PrimitiveAction::PrimitiveAction(char id, string name){
    this->id = id; this->name = name; this->isPrimitive = true; this->isStatic = false;
};
PrimitiveAction::~PrimitiveAction(){};

/*****************************************************************************/
/****** STATIC TASKS IMPLEMENTATION ******************************************/
/*****************************************************************************/
/*** Static gear control ***/
StaticGearControl::StaticGearControl(char id){
    this->id = id; this->name = "StaticGearControl"; this->isPrimitive = false; this->isStatic = true;
};
StaticGearControl::~StaticGearControl(){};
/* Implements the fixed strategy */
shared_ptr<Task> StaticGearControl::getActionSelection(string featureValues, vector<shared_ptr<Task>>& allowedActions){
    return nullptr;
};

/*** Static root node ***/
StaticRoot::StaticRoot(char id){
    this->id = id; this->name = "StaticRoot"; this->isPrimitive = false; this->isStatic = true;
};
StaticRoot::~StaticRoot(){};
/* Implements the fixed strategy */
shared_ptr<Task> StaticRoot::getActionSelection(string featureValues, vector<shared_ptr<Task>>& allowedActions){
    return nullptr;
};
