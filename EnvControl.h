/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef ENVCONTROL_H_
#define ENVCONTROL_H_

#include "CarState.h"
#include "CarControl.h"

#include "DiscreteFeatures.h"
#include "TreeNodes.h"

#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <cmath>

// Parameters for termination control
#define STUCK_MAX_GAMETICKS     25
#define TRACKLEAVE_RIGHT        -1.2
#define TRACKLEAVE_LEFT         1.2

using namespace std;

/* Discretizations for feature values */

class EnvControl {
  private:
    unsigned int _episodeCnt;
    unsigned int _maxEpisodes;
    CarState _lastState;
    CarControl _lastActions;
    /* Check for termination conditions */
    unsigned int _stuckWatchdog;
    bool _isStuck;
    bool _isTerminated;
    void _checkConditions(CarState&);
  public:
    EnvControl(unsigned int);
    ~EnvControl();
    /* Calculates the feature values given the current CarState */
    DiscreteFeatures getFeatures(CarState&);
    /* Returns the actions available (based on the feature values) for the task */
    vector<shared_ptr<Task>> getAllowedActions(shared_ptr<Task>, DiscreteFeatures&);
    /* Returns the overall MDP reward */
    double getAbstractReward(CarState&);

    /* Returns the control actions based on the root task decision or termination */
    CarControl getActions(shared_ptr<Task>);
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
