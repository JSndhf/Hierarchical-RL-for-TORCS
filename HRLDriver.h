/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef HRLDRIVER_H_
#define HRLDRIVER_H_

#include "BaseDriver.h"
#include "CarState.h"
#include "CarControl.h"
#include "SimpleParser.h"
#include "WrapperBaseDriver.h"

#include "DiscreteFeatures.h"
#include "TreeNodes.h"
#include "EnvControl.h"
#include "DataHandler.h"

#include <vector>
#include <memory> // shared_ptr
#include <string>
#include <iostream>
#include <unistd.h> // access()

/***** Experiment setup ****/
#define RL_ALPHA_START      0.8
#define RL_EPSILON          0.2
#define RL_GAMMA            0.99
#define RL_MAX_EPISODES     1500

using namespace std;

class HRLDriver:public WrapperBaseDriver {
  private:
    shared_ptr<Task> _rootTask;
    EnvControl _env;
    DataHandler _data;
    DiscreteFeatures _lastState;
    vector<shared_ptr<Task>> _lastActionsStack;
    bool _isLearning;
  public:
    /* Constructor - includes creating the envControl and taskTree,
       as well as preparing file handling and logging.*/
    HRLDriver();
    /* Destructor */
    ~HRLDriver();
    // Initialization with optional experience import
  	virtual void init(float *, unsigned int, string);

    virtual CarControl wDrive(CarState);

  	// Print a shutdown message
  	virtual void onShutdown();

  	// Print a restart message
  	virtual void onRestart();
};

#endif /*HRLDRIVER_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
