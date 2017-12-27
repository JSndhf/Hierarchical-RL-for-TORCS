#include "TreeNodes.h"

/*****************************************************************************/
/****** BASIC TASK IMPLEMENTATION ********************************************/
/*****************************************************************************/
Task::Task(){};
Task::~Task(){};
/* Returns part of the full feature vector to be used in the particular task */
string Task::getTaskFeatureString(DiscreteFeatures& fullFeatures){
    // For each task only a portion of the full set of features may be relevant.
    // Also, to ease up further processing, convert the dicrete feature values
    // internally stored as ints in the enum to a hex string.
    stringstream hexFeatures;
    switch(this->id){
        // Root task gets the full feature set
        case 0: hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.speed;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.curvature;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.trackPos;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.rpm;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.gear;
                break;
        // speedCtrl
        case 1: hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.speed;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.curvature;
                break;
        // gearCtrl
        case 2: hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.rpm;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.gear;
                break;
        // steeringCtrl
        case 3: hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.speed;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.curvature;
                hexFeatures << std::setfill('0') << std::setw(2) << std::hex << fullFeatures.trackPos;
                break;
    }
    return hexFeatures.str();
};
// Absolute minimalistic implementations for all methods, so derived classes
// like the PrimitiveAction do not need an own implementation.
void Task::learn(DiscreteFeatures& currentFullFeatures, vector<shared_ptr<Task>>& allowedActions, double reward){ return; };
double Task::getExternalMaxCValue(DiscreteFeatures& fullFeatures){ return 0.0; };
shared_ptr<Task> Task::getActionSelection(DiscreteFeatures& fullFeatures, vector<shared_ptr<Task>>& allowedActions, bool exploitOnly){
  return nullptr;
};
// Basic stringify method
string Task::toString(int lvl){
    ostringstream s;
    // If it's the root, make no prefix, else indend it
    for(int indends = 0; indends < lvl; indends++) s << "    ";
    if(lvl != 0) s << "+-- ";
    // Print out node
    if(this->isPrimitive){
        s << "<" << this->name << " | " << (int) this->id << ">\n";
    } else if(this->isStatic){
        s << "[" << this->name << " | " << (int) this->id << "]\n";
    } else {
        s << "[*" << this->name << " | " << (int) this->id << "*]\n";
    }
    // Recusively call for children
    for(auto child : this->children){
        s << child->toString(lvl + 1);
    }
    return s.str();
};

/*****************************************************************************/
/****** DYNAMIC TASK IMPLEMENTATION ******************************************/
/*****************************************************************************/
DynamicTask::DynamicTask(char id, string name, double alphaStart, double gamma, double epsilon, int totalActionCount):
    _alpha(alphaStart),_gamma(gamma),_epsilon(epsilon),_totalActionCount(totalActionCount),
    _gen(chrono::high_resolution_clock::now().time_since_epoch().count()),_prob(0.0, 1.0){
        this->id = id; this->name = name; this->isPrimitive = false; this->isStatic = false;
};

DynamicTask::~DynamicTask(){};

double DynamicTask::_getPseudoReward(DiscreteFeatures& fullFeatures){
    #ifdef __HRL_PR_ENABLED__
        double reward = 0.0;
        switch (this->id) {
          // speedCtrl
          case 1:
              /* Extra punish driving too slow (<50kmh => <V6) */
              if(fullFeatures.speed < DiscreteFeatures::speed_t::V6) reward = HRL_PR_SPEED_SLOW;
              /* Reward driving fast (>120kmh => >V8) */
              else if(fullFeatures.speed > DiscreteFeatures::speed_t::V8) reward = HRL_PR_SPEED_FAST;
          // steeringCtrl
          case 3:
            /* Force avoiding the sides of the track */
            if(fullFeatures.trackPos == DiscreteFeatures::trackPos_t::PL4 ||
               fullFeatures.trackPos == DiscreteFeatures::trackPos_t::PR4)
                reward = HRL_PR_STEER_NEAROUT;
            /* Extra punish driving out of the track */
            else if(fullFeatures.trackPos == DiscreteFeatures::trackPos_t::PL5 ||
                    fullFeatures.trackPos == DiscreteFeatures::trackPos_t::PR5)
                reward = HRL_PR_STEER_OUT;
        }
        return reward;
    #else
        return 0.0; // No pseudo-rewards
    #endif
};

/*** _getInternalMaxCValue ******************************************
    Returns the best action and its value in the given state regarding
    the INTERNAL completion function (including pseudo-rewards)
********************************************************************/
pair<shared_ptr<Task>, double> DynamicTask::_getInternalMaxCValue(string featureValues, vector<shared_ptr<Task>>& allowedActions){
    // Loop though the C~ values (!) and find the best value and action for the given state
    int aCnt = 0;
    double bestValue = 0.0;
    shared_ptr<Task> bestAction = nullptr;
    for(auto const &cval : this->cvals){
        // If all possible actions has been considered, the algorithm can stop savely
        if(aCnt == this->_totalActionCount) break;
        // Else loop further through the cvals and check if a better value can be found
        if(cval.first.find(featureValues) != std::string::npos){
            // If the value of the examined action is better than the last one, take this.
            if(cval.second[HRL_CTILDEVAL_POS] > bestValue){
                char tmpActionID = (char)stoi(cval.first.substr(cval.first.size() - 3), nullptr, 16);
                // Only consider allowed actions
                for(unsigned int itm = 0; itm < allowedActions.size(); itm++){
                    if(allowedActions[itm]->id == tmpActionID){
                        bestValue = cval.second[HRL_CTILDEVAL_POS];
                        bestAction = allowedActions[itm];
                        break; // only the loop over allowedActions
                    }
                }
            }
            aCnt++;
        } else {
            continue; // next cval
        }
    }
    // If no best action could be found, choose a random one
    if(bestAction == nullptr)
        bestAction = allowedActions[(int)(this->_prob(this->_gen) * 100 ) % (int)allowedActions.size()];
    // Return the best action and it's value
    return make_pair( bestAction, bestValue );
}

/*** _getInternalCValue *********************************************
    Returns the value of a feature-action pair regarding the INTERNAL
    completion function
********************************************************************/
double DynamicTask::_getInternalCValue(string featActionPair){
    // Find the value for the given feature-action pair in the C~ values (!)
    map<string, array<double, 2>>::iterator it;
    it = this->cvals.find(featActionPair);
    if(it != this->cvals.end()){
        return it->second[HRL_CTILDEVAL_POS];
    // If the entry doesn't exist until now, return 0.0
    } else {
        return 0.0;
    }
}

/*** _getExternalCValue *********************************************
    Returns the value of a feature-action pair (will be the best action
    determined by the INTERNAL completion function) regarding
    the EXTERNAL completion function (no pseudo-rewards)
********************************************************************/
double DynamicTask::_getExternalCValue(string featActionPair){
    // Find the value for the given feature-action pair in the C values (!)
    map<string, array<double, 2>>::iterator it;
    it = this->cvals.find(featActionPair);
    if(it != this->cvals.end()){
        return it->second[HRL_CVAL_POS];
    // If the entry doesn't exist until now, return 0.0
    } else {
        return 0.0;
    }
}

/*** learn *********************************************************
    Performs the actual learning algorithm, updating the internally
    stored Q-/Completion values, taking into account the actual reward
    gained and possibly a pseudo-reward enforcing the behavior towards
    reaching goal states.

    As rewards observed mirror the success/failure of an action taken
    in the last state, the update is performed backwards.
********************************************************************/
void DynamicTask::learn(DiscreteFeatures& currentFullFeatures, vector<shared_ptr<Task>>& allowedActions,
             double reward){
    // Get this task's portion of the full feature set.
    string currentFeatures = this->getTaskFeatureString(currentFullFeatures);
    // Determine the Q-learning like best action and its value regarding the current state
    pair<shared_ptr<Task>, double> maxIntCActionValue = this->_getInternalMaxCValue(currentFeatures, allowedActions);
    // Build the current feature-action pair
    stringstream aStarId;
    aStarId << std::setfill('0') << std::setw(2) << std::hex << (int)maxIntCActionValue.first->id;
    string currentFeatureAStarPair = currentFeatures + aStarId.str();
    // Remember the current feature-best action pair
    this->_featureAStarPair = currentFeatureAStarPair;

    /****** GATHER DATA ******************************************************/
    /*************************************************************************/
    /*** Learning the INTERNAL completion function ***/
    double Ctilde_sLast_aPiLast = this->_getInternalCValue(this->_lastFeatureAPiPair);
    // Get the internal (pseudo-)reward for the current feature vector
    double Rtilde_sNow = this->_getPseudoReward(currentFullFeatures);
    double Ctilde_sNow_aStar = maxIntCActionValue.second;
    double V_aStar_sNow;

    // Check if aStar is a primitive or composite action
    if(!(maxIntCActionValue.first->isPrimitive)){
        V_aStar_sNow = maxIntCActionValue.first->getExternalMaxCValue(currentFullFeatures);
    } else {
        // aStar is primitive, so take the reward as V_aStar_s1
        V_aStar_sNow = reward;
    }

    #ifdef HRL_DEBUG
        cout << "\tC~ = (1-" << this->_alpha << ") * " << Ctilde_sLast_aPiLast;
        cout << " + " << this->_alpha << " * (" << Rtilde_sNow << " + " << Ctilde_sNow_aStar << " + " << V_aStar_sNow << ")";
    #endif
    // Update the internal completion value for the last feature-action pair
    Ctilde_sLast_aPiLast = ROUND_DBL5((1.0 - this->_alpha) * Ctilde_sLast_aPiLast + this->_alpha * (Rtilde_sNow + Ctilde_sNow_aStar + V_aStar_sNow));
    #ifdef HRL_DEBUG
        cout << " = " << Ctilde_sLast_aPiLast << endl;
    #endif
    /*** Learning the INTERNAL completion function ***/
    double C_sLast_aPiLast = this->_getExternalCValue(this->_lastFeatureAPiPair);
    double C_sNow_aStar = this->_getExternalCValue(currentFeatureAStarPair);
    // As V_aStar_now is just the same as above, no additional data is needed.
    #ifdef HRL_DEBUG
        cout << "\tC = (1-" << this->_alpha << ") * " << C_sLast_aPiLast;
        cout << " + " << this->_alpha << " * (" << C_sNow_aStar << " + " << V_aStar_sNow << ")";
    #endif
    C_sLast_aPiLast = ROUND_DBL5((1.0 - this->_alpha) * C_sLast_aPiLast + this->_alpha * (C_sNow_aStar + V_aStar_sNow));
    #ifdef HRL_DEBUG
        cout << " = " << C_sLast_aPiLast << endl;
    #endif

    /****** UPDATE ***********************************************************/
    /*************************************************************************/
    array<double, 2> cvalUpdate;
    cvalUpdate[HRL_CVAL_POS] = C_sLast_aPiLast;
    cvalUpdate[HRL_CTILDEVAL_POS] = Ctilde_sLast_aPiLast;
    this->cvals[this->_lastFeatureAPiPair] = cvalUpdate;
};

/*** getQValue *****************************************************
    Returns the externally visible Q-Value/Completion for a given
    state and its best action.
********************************************************************/
double DynamicTask::getExternalMaxCValue(DiscreteFeatures& fullFeatures){
    // Get this tasks portion of the feature vector
    string featureValues = this->getTaskFeatureString(fullFeatures);
    // Loop through the map of cvals and check, if it contains the given feature
    double currBestValue = 0.0;
    int aCnt = 0;
    for(auto const &cval : this->cvals){
        // If all possible actions has been considered, the algorithm can stop savely
        if(aCnt == this->_totalActionCount) break;
        // Check if the current features are part of the feat-action string
        if(cval.first.find(featureValues) != string::npos){
            // If the value is better than the current one, update.
            currBestValue = cval.second[HRL_CVAL_POS] > currBestValue ? cval.second[HRL_CVAL_POS] : currBestValue;
            aCnt++;
        } else {
            continue; // next cval
        }
    }
    return currBestValue;
};

/*** getActionSelection *********************************************
    Implements the strategy to select actions either by finding the
    best state-action-value or deploying exploration
********************************************************************/
shared_ptr<Task> DynamicTask::getActionSelection(DiscreteFeatures& fullFeatures, vector<shared_ptr<Task>>& allowedActions, bool exploitOnly){
    shared_ptr<Task> aStar = nullptr;
    shared_ptr<Task> aPi = nullptr;
    char actionId = 0;
    // Get this tasks portion of the feature vector
    string featureValues = this->getTaskFeatureString(fullFeatures);
    // Check if the internally computed best action for the current features
    // has already been set or another search through the cvals has to be done
    if(this->_featureAStarPair.find(featureValues) == string::npos){
        // The current featureValues are not represented so get another aStar
        pair<shared_ptr<Task>, double> maxIntCActionValue = this->_getInternalMaxCValue(featureValues, allowedActions);
        actionId = maxIntCActionValue.first->id;
    } else {
        // Get the aStar action from the string
        actionId = (char)stoi(this->_featureAStarPair.substr(this->_featureAStarPair.size() - 3), nullptr, 16);
    }
    // Check if the best actions id is allowed
    for(unsigned int itm = 0; itm < allowedActions.size(); itm++){
        if(allowedActions[itm]->id == actionId) aStar = allowedActions[itm];
    }
    // Select randomly if no action could be found before
    if(aStar == nullptr)
        aStar = allowedActions[(int)(this->_prob(this->_gen) * 100 ) % (int)allowedActions.size()];
    /****** Exploitation vs. exploration decision ****************************/
    // **** Current version uses epsilon-greedy exploration. ****
    //    The best action should be selected with a propability of 1-epsilon
    if(exploitOnly){
        aPi = aStar;
    } else {
        if(this->_prob(this->_gen) > this->_epsilon){
            aPi = aStar;
        } else {
            // Get a random action NOT including the best action
            do {
                aPi = allowedActions[(int)(this->_prob(this->_gen) * 100 ) % (int)allowedActions.size()];
            } while(aPi->id == aStar->id);
        }
    }
    // Store the feature-action pair for usage in next learning cycle
    stringstream aPiId;
    aPiId << std::setfill('0') << std::setw(2) << std::hex << (int) aPi->id;
    this->_lastFeatureAPiPair = featureValues + aPiId.str();

    return aPi;
};
/*** uodateParams **************************************************
    Implements the decay of alpha/epsilon over the whole experiment
********************************************************************/
void DynamicTask::updateParams(){
    this->_alpha -= HRL_ALPHA_DECAY;
    this->_epsilon -= HRL_EPSILON_DECAY;
}

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
/*** Static root node ***/
StaticRoot::StaticRoot(char id):
    _gen(chrono::high_resolution_clock::now().time_since_epoch().count()),_prob(0, 100){
    this->id = id; this->name = "StaticRoot"; this->isPrimitive = false; this->isStatic = true;
    this->_lastAction = 0;
};
StaticRoot::~StaticRoot(){};
/* Implements the fixed strategy */
shared_ptr<Task> StaticRoot::getActionSelection(DiscreteFeatures& fullFeatures, vector<shared_ptr<Task>>& allowedActions, bool exploitOnly){
    // Cycle through the child nodes, executing one each time step
    // Leveraging the fact that the allowedActions will be in the same order each time,
    // check if the last executed action can be found and execute the next one.
    // Note: This behavior is independent of the fullFeature state discription.
    shared_ptr<Task> nextAction = nullptr;
    for(unsigned int tsk = 0; tsk < allowedActions.size(); tsk++){
        if(allowedActions[tsk]->id == this->_lastAction){
            if(allowedActions[tsk] == allowedActions.back()){
                nextAction = allowedActions[0];
            } else {
                nextAction = allowedActions[tsk + 1];
            }
        }
    }
    // If the last executed action cannot be found, execute a random action of the allowed ones.
    if(nextAction == nullptr) nextAction = allowedActions[this->_prob(this->_gen) % allowedActions.size()];

    this->_lastAction = nextAction->id;
    return nextAction;
};
/*** Static speed control ***/
StaticSpeedControl::StaticSpeedControl(char id){
    this->id = id; this->name = "StaticSpeedControl"; this->isPrimitive = false; this->isStatic = true;
}
StaticSpeedControl::~StaticSpeedControl(){};
/* Implements the fixed strategy */
shared_ptr<Task> StaticSpeedControl::getActionSelection(DiscreteFeatures& fullFeatures, vector<shared_ptr<Task>>& allowedActions, bool exploitOnly){
    // Accelerate until 30kmh and hold this speed constant.
    shared_ptr<Task> nextAction = nullptr;
    char actionId;
    if((int)fullFeatures.speed < (int)DiscreteFeatures::speed_t::V6){
        // Speed under 30kmh --> accelerate
        actionId = 4;
    } else {
        actionId = 5;
    }


    for(unsigned int tsk = 0; tsk < allowedActions.size(); tsk++){
        if(allowedActions[tsk]->id == actionId) nextAction = allowedActions[tsk];
    }
    // If the last executed action cannot be found, execute a random action of the allowed ones.
    if(nextAction == nullptr) nextAction = allowedActions[0];

    return nextAction;
};

/*** Static gear control ***/
StaticGearControl::StaticGearControl(char id){
    this->id = id; this->name = "StaticGearControl"; this->isPrimitive = false; this->isStatic = true;
};
StaticGearControl::~StaticGearControl(){};
/* Implements the fixed strategy */
shared_ptr<Task> StaticGearControl::getActionSelection(DiscreteFeatures& fullFeatures, vector<shared_ptr<Task>>& allowedActions, bool exploitOnly){
    // This implements the static gear shifting strategy as [Karavolos 2013] proposes it.
    // Expected feature value vector contains:
    //    One byte (2 hex digits) per:  rpm (DiscreteFeatures::rpm_t) gear (DiscreteFeatures::gear)
    // Get this tasks portion of the feature vector
    string featureValues = this->getTaskFeatureString(fullFeatures);
    // Extract features value from string
    shared_ptr<Task> nextAction = nullptr;
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
        case 0: aID = 7;
                break;
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
        default: aID = 8;
    }
    // If the desired action is allowed, return it.
    for(unsigned int itm = 0; itm < allowedActions.size(); itm++){
        if(allowedActions[itm]->id == aID) nextAction = allowedActions[itm];
    }
    return nextAction;
};
