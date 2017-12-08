#include "rlDriver.h"

HRLDriver::HRLDriver():_env(RL_MAX_EPISODES){
    // Create the problem specific task tree
    //parallelRoot root;
    /*shared_ptr<iTask> root(new parallelRoot(0));
    shared_ptr<iTask> speedCtrl(new dynamicTask(1, "speedCtrl", RL_ALPHA_START, RL_GAMMA, RL_EPSILON, 3));
    shared_ptr<iTask> accel(new primitiveAction(4, "accelerate"));
    shared_ptr<iTask> idle(new primitiveAction(5, "idle"));
    shared_ptr<iTask> breakk(new primitiveAction(6, "break"));
    speedCtrl->children.push_back(accel);
    speedCtrl->children.push_back(idle);
    speedCtrl->children.push_back(breakk);
    root->children.push_back(speedCtrl);
    shared_ptr<iTask> gearCtrl(new staticGearControl(2));
    shared_ptr<iTask> upshift(new primitiveAction(7, "shift up"));
    shared_ptr<iTask> neutral(new primitiveAction(8, "neural"));
    shared_ptr<iTask> downshift(new primitiveAction(9, "shift down"));
    gearCtrl->children.push_back(upshift);
    gearCtrl->children.push_back(neutral);
    gearCtrl->children.push_back(downshift);
    root->children.push_back(gearCtrl);
    shared_ptr<iTask> steeringCtrl(new dynamicTask(3, "steeringCtrl", RL_ALPHA_START, RL_GAMMA, RL_EPSILON, 5));
    shared_ptr<iTask> s1(new primitiveAction(10, "left 0.5"));
    shared_ptr<iTask> s2(new primitiveAction(11, "left 0.1"));
    shared_ptr<iTask> s3(new primitiveAction(12, "neutral"));
    shared_ptr<iTask> s4(new primitiveAction(13, "right 0.1"));
    shared_ptr<iTask> s5(new primitiveAction(14, "right 0.5"));
    steeringCtrl->children.push_back(s1);
    steeringCtrl->children.push_back(s2);
    steeringCtrl->children.push_back(s3);
    steeringCtrl->children.push_back(s4);
    steeringCtrl->children.push_back(s5);
    root->children.push_back(steeringCtrl);
    // Attach the task tree to the driver object
    this->_taskTree = root;*/
};

void HRLDriver::init(float *angles, string expFilePath){
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
        // Determine the current nodes best/exploring action
        actionOnPath = actionOnPath->getActionSelection(taskFeatureValues, allowedActions);
        // Push it to the stack
        currActionStack.push_back(actionOnPath);
        // Do as long as action is not primitive.
    } while (!actionOnPath->_isPrimitive);
    // The primitive action now resides at the end of the stack

    /********* BACKWARDS LEARNING *********************************************/
    /**************************************************************************/
    // For each dynamic task in the stack of actions of the last state
        // Get the task specific portion of the last and the current feature vector

        // Let the given task learn (update its estimate on the last state and last action)

    /********* STORING AND EXECUTING ******************************************/
    /**************************************************************************/
    // Store the current
    // Perform the primitive action
};

void HRLDriver::onShutdown(){

};

void HRLDriver::onRestart(){

};
