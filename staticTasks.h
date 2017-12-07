/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef STATICTASKS_H_
#define STATICTASKS_H_

#include "treeNodes.h"

using namespace std;

class staticGearControl: public iTask {
  public:
    /* Constructor */
    staticGearControl(char id);
    /* Destructor */
    ~staticGearControl();
    void getActionSelection(string featureValues, string allowedActions, char& astarID, char& apiID);
};

class parallelRoot: public iTask {
  public:
    /* Constructor */
    parallelRoot(char id);
    /* Destructor */
    ~parallelRoot();
    void getActionSelection(string featureValues, string allowedActions, char& astarID, char& apiID);
};

#endif /*STATICTASKS_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
