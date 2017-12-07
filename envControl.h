/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef ENVCONTROL_H_
#define ENVCONTROL_H_

#include "CarState.h"

#include "treeNodes.h"

#include <string>

using namespace std;

/* Discretizations for feature values */

class envControl {
  private:
    unsigned int _episodeCnt;
    unsigned int _maxEpisodes;
    string _currentFeatureSpace;
    CarState _lastState;
    /* Check for termination conditions */
    unsigned int _stuckWatchdog;
    bool _isStuck;
    bool _isTerminated;
    void _checkConditions(CarState cs);
  public:
    /* Constructor */
    envControl(unsigned int maxEpisodes):
        _episodeCnt(0),_maxEpisodes(maxEpisodes),_stuckWatchdog(0){};
    /* Destructor */
    ~envControl(){};
    /* Calculates the feature values given the current CarState */
    void buildFeatures(CarState cs);

    /* Returns part of the full feature vector to be used in the particular task */
    string getTaskFeatures(string taskID);
    /* Returns the actions available (based on the feature values) for the task */
    string getAllowedActions(string taskID, string featureValues);
    /* Returns the overall MDP reward */
    double getAbstractReward(CarState cs);

    /* Returns the control actions based on the root task decision or termination */
    CarControl getActions();
};


#endif /*ENVCONTROL_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
