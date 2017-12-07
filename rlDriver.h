/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef RLDRIVER_H_
#define RLDRIVER_H_

#include "BaseDriver.h"
#include "CarState.h"
#include "CarControl.h"
#include "SimpleParser.h"
#include "WrapperBaseDriver.h"

#include "treeNodes.h"
#include "staticTasks.h"
#include "envControl.h"
#include "dataHandler.h"

#include <vector>
#include <memory>
#include <string>
#include <iostream>

/***** Experiment setup ****/
#define RL_ALPHA_DECAY
#define RL_ALPHA_START      0.8
#define RL_ALPHA_MIN        0.1
#define RL_EPSILON          0.9
#define RL_GAMMA            0.99
#define RL_MAX_EPISODES     1500

using namespace std;

class rlDriver:public WrapperBaseDriver {
  private:
    shared_ptr<iTask> _taskTree;
    envControl _env;
    dataHandler _data;
  public:
    /* Constructor - includes creating the envControl and taskTree,
       as well as preparing file handling and logging.*/
    rlDriver();
    /* Destructor */
    ~rlDriver(){};
    // Initialization with optional experience import
  	virtual void init(float *angles, string expFilePath);

    virtual CarControl wDrive(CarState cs);

  	// Print a shutdown message
  	virtual void onShutdown();

  	// Print a restart message
  	virtual void onRestart();
};

#endif /*RLDRIVER_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
