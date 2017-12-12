#include "HRLDriver.h"

HRLDriver::HRLDriver():
    _env(EnvControl(HRL_MAX_EPISODES)),_data(DataHandler()),_lastState(DiscreteFeatures()){
    // Create the problem specific task tree
    shared_ptr<Task> root = make_shared<StaticRoot>(0);
    shared_ptr<Task> speedCtrl = make_shared<DynamicTask>(1, "speedCtrl", HRL_ALPHA_START, HRL_GAMMA, HRL_EPSILON, 3);
    shared_ptr<Task> accel = make_shared<PrimitiveAction>(4, "accelerate");
    shared_ptr<Task> idle = make_shared<PrimitiveAction>(5, "idle");
    shared_ptr<Task> breakk = make_shared<PrimitiveAction>(6, "break");
    speedCtrl->children.push_back(accel);
    speedCtrl->children.push_back(idle);
    speedCtrl->children.push_back(breakk);
    root->children.push_back(speedCtrl);
    shared_ptr<Task> gearCtrl = make_shared<StaticGearControl>(2);
    shared_ptr<Task> upshift = make_shared<PrimitiveAction>(7, "shift up");
    shared_ptr<Task> neutral = make_shared<PrimitiveAction>(8, "neural");
    shared_ptr<Task> downshift = make_shared<PrimitiveAction>(9, "shift down");
    gearCtrl->children.push_back(upshift);
    gearCtrl->children.push_back(neutral);
    gearCtrl->children.push_back(downshift);
    root->children.push_back(gearCtrl);
    shared_ptr<Task> steeringCtrl = make_shared<DynamicTask>(3, "steeringCtrl", HRL_ALPHA_START, HRL_GAMMA, HRL_EPSILON, 5);
    shared_ptr<Task> s1 = make_shared<PrimitiveAction>(10, "left 0.5");
    shared_ptr<Task> s2 = make_shared<PrimitiveAction>(11, "left 0.1");
    shared_ptr<Task> s3 = make_shared<PrimitiveAction>(12, "neutral");
    shared_ptr<Task> s4 = make_shared<PrimitiveAction>(13, "right 0.1");
    shared_ptr<Task> s5 = make_shared<PrimitiveAction>(14, "right 0.5");
    steeringCtrl->children.push_back(s1);
    steeringCtrl->children.push_back(s2);
    steeringCtrl->children.push_back(s3);
    steeringCtrl->children.push_back(s4);
    steeringCtrl->children.push_back(s5);
    root->children.push_back(steeringCtrl);
    // Attach the task tree to the driver object
    this->_rootTask = root;
    #ifdef HRL_DEBUG
        cout << "*** Task configuration: ***" << endl;
        cout << this->_rootTask->toString(0);
    #endif
    this->_episodeCnt = 0;
};

HRLDriver::~HRLDriver(){};

void HRLDriver::init(float *angles, unsigned int mode, string expFilePath){
    // Use the standard angle splitting as in SimpleDriver although not all of
    // the values might be used.

    // Set angles as {-90,-75,-60,-45,-30,-20,-15,-10,-5,0,5,10,15,20,30,45,60,75,90}
    for(int i = 0; i < 5; i++){
        angles[i] = -90 + i * 15;
        angles[18 - i] = 90 - i * 15;
    }
    for(int i = 5; i < 9; i++){
      	angles[i] = -20 + (i - 5) * 5;
      	angles[18 - i] = 20 - (i - 5) * 5;
    }
    angles[9] = 0;
    // Set the mode
    this->_isLearning = (bool) mode;
    this->_episodeCnt = 0;
    // Check whether the dynamic tasks should be fed with stored experience
    if(expFilePath.size() != 0){
        // Check if the file can be found
        if(access( expFilePath.c_str(), F_OK ) != -1){
            // Load the experience
            this->_data.loadExperience(expFilePath, this->_rootTask);
        }
    }
};

CarControl HRLDriver::wDrive(CarState cs){
    CarControl primActions(0.0, 0.0, 0, 0.0, 0.0);
    // Wait until the race begins
    if(cs.getCurLapTime() >= 0.0){
        // Update the environment status
        this->_env.updateStatus(cs);
        // Extract the features from the CarState
        DiscreteFeatures fullFeatures = this->_env.getFeatures(cs);
        /*** Learn first, as then the internally computed best action choices in
             the current state can be reused for strategy finding               ***/

        // Get the overall reward
        double rt = this->_env.getAbstractReward(cs);

        /********* BACKWARDS LEARNING *********************************************/
        /**************************************************************************/
        // For each dynamic task in the stack of actions of the last state
        for(shared_ptr<Task> task : this->_lastActionsStack){
            if(!task->isPrimitive && !task->isStatic){
                // Get the currently allowed actions for the specific task
                vector<shared_ptr<Task>> allowedActions = this->_env.getAllowedActions(task, fullFeatures);
                // Call the learning method
                task->learn(fullFeatures, allowedActions, rt);
            }
        }

        /********* CURRENT STATE PROCESSING ***************************************/
        /**************************************************************************/
        // Do a tree search for the best action and build an hierarchical stack of actions
        // Begin with the root node
        shared_ptr<Task> actionOnPath = this->_rootTask;
        vector<shared_ptr<Task>> allowedActions;
        string taskFeatureValues;
        // Stack of actions/tasks along the best path
        vector<shared_ptr<Task>> currActionStack;
        do {
            // Get the allowed actions for the current task regarding the feature set
            allowedActions = this->_env.getAllowedActions(actionOnPath, fullFeatures);
            // Determine the current nodes best and stategic actions (the latter accounts for exploration)
            actionOnPath = actionOnPath->getActionSelection(fullFeatures, allowedActions);
            // Push it to the stack
            currActionStack.push_back(actionOnPath);
            // Do as long as action is not primitive.
        } while (!actionOnPath->isPrimitive);
        // The primitive action now resides at the end of the stack as well as in
        // the actionOnPath variable

        /********* STORING AND EXECUTING ******************************************/
        /**************************************************************************/
        // Store the current action stack to learn from its outcome in the next iteration
        this->_lastActionsStack = currActionStack;
        // Perform the primitive action
        primActions = this->_env.getActions(actionOnPath);
        // Update the statistics
        this->_data.updateStats(rt);
        // If the reset is called, do some backup
        if(primActions.getMeta()){
            this->_episodeCnt++;
            // Store the experience once every 500 episodes
            if((this->_episodeCnt % HRL_BACKUP_EPISODE_CNT) == 0) this->_data.storeExperience(this->_rootTask);
            // Write out stats
            this->_data.writeStats();
            // Output to visualize episodes
            cout << "." << flush;
            if((this->_episodeCnt % 50) == 0) cout << "[" << this->_episodeCnt << "]";
            // Reset the environment for the next episode
            this->_env.resetStatus();
        }
    }
    return primActions;
};

void HRLDriver::onShutdown(){
    // Store the experience in a file
    this->_data.storeExperience(this->_rootTask);
};

void HRLDriver::onRestart(){

};
