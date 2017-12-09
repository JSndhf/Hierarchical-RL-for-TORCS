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
shared_ptr<Task> Task::getActionSelection(string featureValues, vector<shared_ptr<Task>>& allowedActions){
  return nullptr;
};
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
    _alphaStart(alphaStart),_gamma(gamma),_epsilon(epsilon),_totalActionCount(totalActionCount),
    _gen(chrono::high_resolution_clock::now().time_since_epoch().count()),_prob(0.0, 1.0){
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
    state and its best action.
********************************************************************/
double DynamicTask::getMaxQValue(string featureValues){
    // Loop through the map of cvals and check, if it contains the given feature
    double currBestValue = 0.0;
    int aCnt = 0;
    for(auto const &cval : this->cvals){
        // If all possible actions has been considered, the algorithm can stop savely
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
    shared_ptr<Task> aStar;
    shared_ptr<Task> aPi;
    char bestActionId = 0;
    double bestActionValue = 0.0;
    // Check if the given state was already visited and which action is to be preferred in that case
    int aCnt = 0;
    for(auto const &cval : this->cvals){
        // If all possible actions has been considered, the algorithm can stop savely
        if((cval.first.find(featureValues) != string::npos) && aCnt < this->_totalActionCount){
            // If the value of the examined action is better than the last one, update.
            if(cval.second[1] > bestActionValue){
                bestActionValue = cval.second[1];
                bestActionId = (char) strtoul(cval.first.substr(cval.first.length() - 3,2).c_str(), NULL, 16);
            }
            aCnt++;
        } else {
            break;
        }
    }
    // Check if the best actions id (or the default id 0 which will never be
    // found 'cause it's the root) is allowed
    for(unsigned int itm = 0; itm < allowedActions.size(); itm++){
        if(allowedActions[itm]->id == bestActionId) aStar = allowedActions[itm];
    }
    // Else select randomly
    aStar = allowedActions[(int)(this->_prob(this->_gen) * 100 ) % (allowedActions.size() - 1)];
    /****** Exploitation vs. exploration decision ****************************/
    // **** Current version uses epsilon-greedy exploration. ****
    //    The best action should be selected with a propability of 1-epsilon
    if(this->_prob(this->_gen) > this->_epsilon){
        aPi = aStar;
    } else {
        aPi = allowedActions[(int)(this->_prob(this->_gen) * 100 ) % (allowedActions.size() - 1)];
    }

    return aPi;
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
    // This implements the static gear shifting strategy as [Karavolos 2013] proposes it.
    // Expected feature value vector contains:
    //    One byte (2 hex digits) per:  rpm (DiscreteFeatures::rpm_t) gear (DiscreteFeatures::gear)
    // Extract features value from string
    shared_ptr<Task> selAction = nullptr;
    int rpm = strtoul(featureValues.substr(0,2).c_str(), NULL, 16);
    int gear = strtoul(featureValues.substr(2,2).c_str(), NULL, 16);
    char aID;  // Later finds the desired action in the vector of allowedActions
    // The following ID are set in HRLDriver.cpp:
    //    upshift - 7, neutral - 8, downshift - 9
    // Decision table:
    //    Gear | 1  | 2    | 3    | 4    | 5    | 6
    //    Up   | 8k | 9.5k | 9.5k | 9.5k | 9.5k | -
    //    Down | -  | 4k   | 6.3k | 7.3k | 7.3k | 7.3k
    switch(gear){
        case 1: if(rpm >= (int) DiscreteFeatures::rpm_t::RPM7) aID = 7;
                else aID = 8;
                break;
        case 2: if(rpm >= (int) DiscreteFeatures::rpm_t::RPM8) aID = 7;
                else if(rpm <= (int) DiscreteFeatures::rpm_t::RPM2) aID = 9;
                else aID = 8;
                break;
        case 3: if(rpm >= (int) DiscreteFeatures::rpm_t::RPM8) aID = 7;
                else if(rpm <= (int) DiscreteFeatures::rpm_t::RPM4) aID = 9;
                else aID = 8;
                break;
        case 4: if(rpm >= (int) DiscreteFeatures::rpm_t::RPM8) aID = 7;
                else if(rpm <= (int) DiscreteFeatures::rpm_t::RPM5) aID = 9;
                else aID = 8;
                break;
        case 5: if(rpm >= (int) DiscreteFeatures::rpm_t::RPM8) aID = 7;
                else if(rpm <= (int) DiscreteFeatures::rpm_t::RPM5) aID = 9;
                else aID = 8;
                break;
        case 6: if(rpm <= (int) DiscreteFeatures::rpm_t::RPM5) aID = 9;
                else aID = 8;
                break;
    }
    // If the desired action is allowed, return it.
    for(unsigned int itm = 0; itm < allowedActions.size(); itm++){
        if(allowedActions[itm]->id == aID) selAction = allowedActions[itm];
    }
    return selAction;
};

/*** Static root node ***/
StaticRoot::StaticRoot(char id):
    _gen(chrono::high_resolution_clock::now().time_since_epoch().count()),_prob(0, 100){
    this->id = id; this->name = "StaticRoot"; this->isPrimitive = false; this->isStatic = true;
    this->_lastAction = 0;
};
StaticRoot::~StaticRoot(){};
/* Implements the fixed strategy */
shared_ptr<Task> StaticRoot::getActionSelection(string featureValues, vector<shared_ptr<Task>>& allowedActions){
    // Cycle through the child nodes, executing one each time step
    // Leveraging the fact that the allowedActions will be in the same order each time,
    // check if the last executed action can be found and execute the next one.
    shared_ptr<Task> nextAction = nullptr;
    for(unsigned int itm = 0; itm < allowedActions.size(); itm++){
        if(allowedActions[itm]->id == this->_lastAction){
            itm++;
            // Make sure, the next action is before or equal the last element in the vector
            if(itm < allowedActions.size()) nextAction = allowedActions[itm];
            // Or take the first one instead
            else nextAction = allowedActions[0];
            break;
        }
    }
    // If the last executed action cannot be found, execute a random action of the allowed ones.
    if(nextAction == nullptr) nextAction = allowedActions[this->_prob(this->_gen) % (allowedActions.size() - 1)];

    return nextAction;
};
