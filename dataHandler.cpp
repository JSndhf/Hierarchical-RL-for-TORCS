#include "dataHandler.h"

/*** getDynamicTasks *************************************************
    Loops through the given taskTree, returning those children who
    themself are dynamic tasks. This is meant to be used recursively
    to create a flat vector of pointers of all dynamic tasks in a whole
    tree. ATTENTION: As this solution uses pointers to objects, it
    must be assured, these objects exist longer than the usage of the
    returned vector.
********************************************************************/
vector<shared_ptr<iTask>> dataHandler::_getDynamicTasks(shared_ptr<iTask> rootTask){
    vector<shared_ptr<iTask>> dynChildren;
    // The first node is the root node of the task tree.
    // First, get its children and call getDynamicTasks recursively
    // to take their children into account, too.
    int childCnt = rootTask->children.size();
    int ccnt;
    for(ccnt = 0; ccnt < childCnt; ccnt++){
        shared_ptr<iTask> child = rootTask->children[ccnt];
        if (!child->isStatic && !child->isPrimitive){
            // Add the child to the overall vector
            dynChildren.push_back(child);
            // Now do a recursive call to get childs children and append the
            vector<shared_ptr<iTask>> dynGrandchildren = dataHandler::_getDynamicTasks(child);
            dynChildren.insert(dynChildren.end(), dynGrandchildren.begin(), dynGrandchildren.end());
        }
    }
    // When all children are found and have been examined if they're dynamic,
    // return the resulting vector.
    return dynChildren;
};

/*** findTaskInTree *****************************************
    Search for a task with the given id.
    As the task tree is kind of a linked list, a recursive algorithm
    is used here too. The same risk appears as above: It has to be
    assured outside of the function, that the objects pointed to still
    exist.
********************************************************************/
shared_ptr<iTask> _findTaskInTree(shared_ptr<iTask> rootTask, char id){
    int cnt;
    shared_ptr<iTask> childMatch;
    // Check if the current task matches the searched id
    if(rootTask->id == id){
        return rootTask;
    } else {
        // Check if the rootTask has children and try to find the id in them
        if(rootTask->children.size()){
            for(cnt = 0; cnt < rootTask->children.size(); cnt++){
                childMatch = dataHandler::_findTaskInTree(&rootTask->children[cnt], id);
                if(childMatch != NULL) break;
            }
            return childMatch;
        } else {
            return NULL;
        }
    }
}

/*** storeExperience ************************************************
    Creates a pugixml document in memory and stores it in the default
    location in the application folder.
********************************************************************/
bool dataHandler::storeExperience(shared_ptr<iTask> taskTree){
    // Create a new xml document to contain the experience
    pugi::xml_document xmlDoc;
    // Generate XML declaration
    auto declarationNode = xmlDoc.append_child(pugi::node_declaration);
    declarationNode.append_attribute("version") = "1.0";
    declarationNode.append_attribute("encoding") = "UTF-8";
    declarationNode.append_attribute("standalone") = "yes";
    // A valid XML doc must contain a single root node of any name
    auto xmlRoot = xmlDoc.append_child("dynTasks");
    // Get a vector of all dynamic tasks in the taskTree
    vector<shared_ptr<iTask>> dynTasks = dataHandler::_getDynamicTasks(&taskTree);
    // Iterate through the tasks and store their experience one after another
    int tCnt, expCnt;
    map<string, array<double, 2> > cvals;
    for(tCnt = 0; tCnt < dynTasks.size(); tCnt++){
        auto taskNode = xmlRoot.append_child("taskNode");
        taskNode.append_attribute("id") = dynTasks[tCnt]->id;
        // For each experience create a xml node and store the C and C~ values in it
        for(expCnt = 0; expCnt < cvals.size(); expCnt++){
            auto expNode = taskNode.append_child("cValNode");
            expNode.append_attribute("sa") = dynTasks[tCnt]->cvals[expCnt].first.c_str();
            expNode.append_child(pugi::node_pcdata).set_value(
                    to_string(dynTasks[tCnt]->cvals[expCnt].second[0]).c_str()
                );
            expNode.append_child(pugi::node_pcdata).set_value(
                    to_string(dynTasks[tCnt]->cvals[expCnt].second[1]).c_str()
                );
        }
    }
    // Now xmlDoc and namely the root node contains all dynamic tasks data
    // Write out the data structure.
    return xmlDoc.save_file("data/rlDriverData.xml");
};

/*** loadExperience ************************************************
    Loads a pugixml document in memory, loops through all nodes and
    copy the C-values in the given taskTree
********************************************************************/
bool dataHandler::loadExperience(string srcPath, shared_ptr<iTask> taskTree){
    string searchStr;
    // Load xml file
    pugi::xml_document xmlDoc;
    pugi::xml_parse_result result = doc.load_file(srcPath.c_str(),
        pugi::parse_default|pugi::parse_declaration);
    // Do a rudimentary compliance check
        // Is the file correct xml?
        if (!result){
            cout << "Failed to load experience due to XML parsing error: " << result.description() << endl;
            return false;
        }
        pugi::xml_node xmlRoot = xmlDoc.document_element();
        // Is there at least one taskNode in the structure?
        searchStr = "taskNode";
        pugi::xpath_node xpathRootNode = xmlRoot.select_single_node(searchStr.c_str());
        if (!xpathNode){
            cout << "Failed to load experience - no tasks found in the XML." << endl;
            return false;
        }

    // For each taskNode in the file, find the corresponding task in the
    // taskTree. Then loop through the cValNodes and merge their values
    // into the tasks cvals vector.
    for (pugi::xpath_node n: xmlRoot.select_nodes("taskNode")){
        pugi::xml_node xmlTask = n.node();
        // Find the task in the task tree with the same id
        iTask* task = dataHandler::_findTaskInTree(&taskTree, (char) xmlNode.attribute("intVal").as_int());
        if(task != NULL){
            // For all child nodes (representing the cvals) insert their values into
            // the tasks cval vector.
            for(pugi::xpath_node c: xmlTask.select_nodes("cValNode")){
                pugi::xml_node xmlCValNode = c.node();
                pugi::xpath_node_set cvalSet = xmlCValNode.select_nodes("text()");
                task->cvals.insert(
                        xmlCValNode.attribute("as").value(),
                        {{ cvalSet[0].node().as_double(), cvalSet[1].node().as_double() }}
                    );
            }
        }
    }
    return true;

}

void dataHandler::createStatistics(){

};

void dataHandler::updateStatistics(double totalEpisodeReward, double totalEpisodeDistance){

};
