#include "rlDriver.h"

rlDriver::rlDriver():_env(RL_MAX_EPISODES){
    // Create the problem specific task tree
    parallelRoot root;
    /*shared_ptr<iTask> root(new parallelRoot(0));
    shared_ptr<iTask> speedCtrl(new dynamicTask(1, "speedCtrl", RL_ALPHA_START, RL_GAMMA, RL_EPSILON, 3));
    shared_ptr<iTask> accel(new primitiveAction(4, "accellerate"));
    shared_ptr<iTask> idle(new primitiveAction(5, "idle"));
    shared_ptr<iTask> breakk(new primitiveAction(6, "break"));
    speedCtrl->children.push_back(accel);
    speedCtrl->children.push_back(idle);
    speedCtrl->children.push_back(breakk);
    root->children.push_back(speedCtrl);
    shared_ptr<iTask> gearCtrl(new staticGearControl(2));
    root->children.push_back(gearCtrl);
    shared_ptr<iTask> steeringCtrl(new dynamicTask(3, "steeringCtrl", RL_ALPHA_START, RL_GAMMA, RL_EPSILON, 5));
    shared_ptr<iTask> s1(new primitiveAction(7, "left 0.5"));
    shared_ptr<iTask> s2(new primitiveAction(8, "left 0.1"));
    shared_ptr<iTask> s3(new primitiveAction(9, "neutral"));
    shared_ptr<iTask> s4(new primitiveAction(10, "right 0.1"));
    shared_ptr<iTask> s5(new primitiveAction(11, "right 0.5"));
    steeringCtrl->children.push_back(s1);
    steeringCtrl->children.push_back(s2);
    steeringCtrl->children.push_back(s3);
    steeringCtrl->children.push_back(s4);
    steeringCtrl->children.push_back(s5);
    root->children.push_back(steeringCtrl);
    // Attach the task tree to the driver object
    this->_taskTree = root;*/
};

void rlDriver::init(float *angles, string expFilePath){

};

CarControl rlDriver::wDrive(CarState cs){
    // Extract the features from the CarState

    // Get the overall reward

    // Do a tree search for the best action and build an hierarchical stack of actions
        // Determine the root's best/exploring action
        // For this action (i.e. subtask) get the best/exploring action, too.
        // Do as long as action is not primitive.
        // When the primitive action is reached, add it to the top of the stack
        // and store the stack of (current) actions.

    // For each task in the stack of actions of the last state
        // Get the task specific portion of the last and the current feature vector

        // Let the given task learn (update its estimate on the last state and last action)

    // Perform the primitive action
};

void rlDriver::onShutdown(){

};

void rlDriver::onRestart(){

};
