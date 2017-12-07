#include "treeNodes.h"

void iTask::learn(string featureValues, string allowedActions){ return; }
double iTask::getMaxQValue(string featureValues){ return 0.0; }
void iTask::getActionSelection(string featureValues, string allowedActions, char& astarID, char& apiID){ return; }
string iTask::toString(){
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

primitiveAction::primitiveAction(char id, string name){
    this->id = id; this->name = name; this->isPrimitive = true; this->isStatic = false;
};
primitiveAction::~primitiveAction(){};



dynamicTask(char id, string name, double alphaStart, double gamma, double epsilon, int totalActionCount):
    _alphaStart(alphaStart),_gamma(gamma),_epsilon(epsilon),_totalActionCount(totalActionCount){
        this->id = id; this->name = name; this->isPrimitive = false; this->isStatic = false;
};

dynamicTask::~dynamicTask(){};

double dynamicTask::_getPseudoReward(string featureValues){

};

/*** learn *********************************************************
    Performs the actual learning algorithm, updating the internally
    stored Q-/Completion values, taking into account the actual reward
    gained and possibly a pseudo-reward enforcing the behavior towards
    reaching goal states.

    As rewards observed mirror the success/failure of an action taken
    in the last state, the update is performed backwards.
********************************************************************/
void dynamicTask::learn(string featureValues, char astarID, char apiID){
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
    _lastFeatActionPair = featureValues + apiID;
};

/*** getQValue *****************************************************
    Returns the externally visible Q-Value/Completion for a given
    state and it's best action.
********************************************************************/
double dynamicTask::getMaxQValue(string featureValues){
    // Loop through the map of cvals and check, if it contains the given feature
    double currBestValue = 0.0;
    int cnt, aCnt = 0;
    for(cnt = 0; cnt < this->cvals.size(); cnt++){
        // If all actions possible has been found, the algorithm can stop savely
        if(this->cvals[cnt].first.contains(featureValues) && aCnt < this->_totalActionCount){
            // If the value is better than the current one, update.
            currBestValue = this->cvals[cnt].second[0] > currBestValue ? this->cvals[cnt].second[0] : currBestValue;
            aCnt++;
        } else {
            break;
        }
    }
    return currBestValue;
};

void dynamicTask::getActionSelection(string featureValues, string allowedActions, char& astarID, char& apiID){

};
