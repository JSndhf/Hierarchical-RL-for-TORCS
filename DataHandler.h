/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef DATAHANDLER_H_
#define DATAHANDLER_H_

#include "pugixml.hpp"
#include <map>
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <fstream>

#include "TreeNodes.h"

using namespace std;

class DataHandler {
  private:
    int _episodeCnt;
    long long _episodeActionCnt;
    double _episodeReward;
    vector<shared_ptr<DynamicTask>> _getDynamicTasks(shared_ptr<Task>);
    shared_ptr<Task> _findTaskInTree(shared_ptr<Task>, char);
  public:
    /* Constructor */
    DataHandler();
    /* Destructor */
    ~DataHandler();
    bool storeExperience(shared_ptr<Task>);
    bool loadExperience(string, shared_ptr<Task>);
    void printExperience(shared_ptr<Task>);
    void updateParams(shared_ptr<Task>);

    void updateStats(double);
    void writeStats();
};

#endif /*DATAHANDLER_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
