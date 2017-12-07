#include "envControl.h"

void envControl::_checkConditions(CarState cs){

};

/* Calculates the feature values given the current CarState */
void envControl::buildFeatures(CarState cs){

};

/* Returns part of the full feature vector to be used in the particular task */
string envControl::getTaskFeatures(string taskID){

};
/* Returns the actions available (based on the feature values) for the task */
string envControl::getAllowedActions(string taskID, string featureValues){

};
/* Returns the overall MDP reward */
double envControl::getAbstractReward(CarState cs){
    // As in previous works on this problem, the overall rewards is calculated
    // based on the distance traveled since the last call. Additionally, negative
    // rewards for leaving the track and for standing still seek to avoid
    // undesired states.
    double ds = cs.getDistFromStart() - this->_lastState.getDistFromStart();
    double dv = cs.getSpeedX() - this->_lastState.getSpeedX();
    double pos = cs.getTrackPos();
    // Allow small crossings of the sidelines but punish the leaving of track
    if(this->_isStuck){
       return -3.0;
    } else if(pos < -1.2 || pos > 1.2){
        return -2.0;
    } else if(dv < 0.1){
        return -1.0;
    } else {
        return ds;
    }
};

/* Returns the control actions based on the root task decision or termination */
CarControl envControl::getActions(){
    CarControl cc;
    // If the current episode is terminated, send a request for restart.
    if(this->_isTerminated){
        cc.setMeta(1);
    }
    
};
