#include "HRLDriver.h"

HRLDriver::HRLDriver():
    _env(EnvControl(RL_MAX_EPISODES)),_data(DataHandler()),_lastState(DiscreteFeatures()){
    // Create the problem specific task tree
    shared_ptr<Task> root = make_shared<StaticRoot>(0);
    shared_ptr<Task> speedCtrl = make_shared<DynamicTask>(1, "speedCtrl", RL_ALPHA_START, RL_GAMMA, RL_EPSILON, 3);
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
    shared_ptr<Task> steeringCtrl = make_shared<DynamicTask>(3, "steeringCtrl", RL_ALPHA_START, RL_GAMMA, RL_EPSILON, 5);
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
};

CarControl HRLDriver::wDrive(CarState cs){
    /********* CURRENT STATE PROCESSING ***************************************/
    /**************************************************************************/
    // Extract the features from the CarState
    DiscreteFeatures newFullFeatures = this->_env.getFeatures(cs);
    // Get the overall reward
    double rt = this->_env.getAbstractReward(cs);
    // Do a tree search for the best action and build an hierarchical stack of actions
    // Begin with the root node
    shared_ptr<Task> actionOnPath = this->_rootTask;
    vector<shared_ptr<Task>> allowedActions;
    string taskFeatureValues;
    // Stack of actions/tasks along the best path
    vector<shared_ptr<Task>> currActionStack;
    do {
        // Get the task specific portion of the full feature set
        taskFeatureValues = this->_env.getTaskFeatureString(actionOnPath, newFullFeatures);
        // Get the allowed actions for the current task regarding the feature set
        allowedActions = this->_env.getAllowedActions(actionOnPath, newFullFeatures);
        // Determine the current nodes best and stategic actions (the latter accounts for exploration)
        actionOnPath = actionOnPath->getActionSelection(taskFeatureValues, allowedActions);
        // Push it to the stack
        currActionStack.push_back(actionOnPath);
        // Do as long as action is not primitive.
    } while (!actionOnPath->isPrimitive);
    // The primitive action now resides at the end of the stack as well as in
    // the actionOnPath variable

    /********* BACKWARDS LEARNING *********************************************/
    /**************************************************************************/
    // For each dynamic task in the stack of actions of the last state
        // Get the task specific portion of the last and the current feature vector

        // Let the given task learn (update its estimate on the last state and last action)

    /********* STORING AND EXECUTING ******************************************/
    /**************************************************************************/
    // Store the current state and action stack
    // Perform the primitive action
    return this->_env.getActions(actionOnPath);
};

void HRLDriver::onShutdown(){

};

void HRLDriver::onRestart(){

};
