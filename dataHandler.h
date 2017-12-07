/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef DATAHANDLER_H_
#define DATAHANDLER_H_

#include "lib/pugixml.hpp"
#include <map>
#include <vector>
#include <memory>
#include <array>

#include "treeNodes.h"

using namespace std;

class dataHandler {
  private:
    int _monitorFileHandle;
    vector<shared_ptr<iTask>> _getDynamicTasks(shared_ptr<iTask> rootTask);
    shared_ptr<iTask> _findTaskInTree(shared_ptr<iTask> rootTask);
  public:
    /* Constructor */
    dataHandler(){};
    /* Destructor */
    ~dataHandler(){};
    bool storeExperience(shared_ptr<iTask> taskTree);
    void loadExperience(string srcPath, shared_ptr<iTask> taskTree);

    void createStatistics();
    void updateStatistics(double totalEpisodeReward, double totalEpisodeDistance);
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
