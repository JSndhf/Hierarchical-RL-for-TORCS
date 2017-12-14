#include "DataHandler.h"

DataHandler::DataHandler():_episodeCnt(0),_episodeActionCnt(0),_episodeReward(0.0){};
DataHandler::~DataHandler(){};

/*** getDynamicTasks *************************************************
    Loops through the given taskTree, returning those children who
    themself are dynamic tasks. This is meant to be used recursively
    to create a flat vector of pointers of all dynamic tasks in a whole
    tree. ATTENTION: As this solution uses pointers to objects, it
    must be assured, these objects exist longer than the usage of the
    returned vector.
********************************************************************/
vector<shared_ptr<DynamicTask>> DataHandler::_getDynamicTasks(shared_ptr<Task> rootTask){
    vector<shared_ptr<DynamicTask>> dynChildren;
    // The first node is the root node of the task tree.
    // First, get its children and call getDynamicTasks recursively
    // to take their children into account, too.
    for(unsigned int ccnt = 0; ccnt < rootTask->children.size(); ccnt++){
        shared_ptr<Task> child = rootTask->children[ccnt];
        if (!child->isStatic && !child->isPrimitive){
            // Add the child to the overall vector
            dynChildren.push_back(dynamic_pointer_cast<DynamicTask>(child));
            // Now do a recursive call to get childs children and append the
            vector<shared_ptr<DynamicTask>> dynGrandchildren = DataHandler::_getDynamicTasks(child);
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
shared_ptr<Task> DataHandler::_findTaskInTree(shared_ptr<Task> rootTask, char id){
    shared_ptr<Task> childMatch;
    // Check if the current task matches the searched id
    if(rootTask->id == id){
        return rootTask;
    } else {
        // Check if the rootTask has children and try to find the id in them
        if(rootTask->children.size()){
            for(unsigned int cnt = 0; cnt < rootTask->children.size(); cnt++){
                childMatch = DataHandler::_findTaskInTree(rootTask->children[cnt], id);
                if(childMatch != nullptr) break;
            }
            return childMatch;
        } else {
            return nullptr;
        }
    }
}

/*** storeExperience ************************************************
    Creates a pugixml document in memory and stores it in the default
    location in the application folder.
********************************************************************/
bool DataHandler::storeExperience(shared_ptr<Task> taskTree){
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
    vector<shared_ptr<DynamicTask>> dynTasks = DataHandler::_getDynamicTasks(taskTree);
    // Iterate through the tasks and store their experience one after another
    map<string, array<double, 2> > cvals;
    for(unsigned int tCnt = 0; tCnt < dynTasks.size(); tCnt++){
        auto taskNode = xmlRoot.append_child("taskNode");
        taskNode.append_attribute("id") = dynTasks[tCnt]->id;
        // For each experience create a xml node and store the C and C~ values in it
        for(auto const &cval : dynTasks[tCnt]->cvals){
            auto expNode = taskNode.append_child("cValNode");
            expNode.append_attribute("sa") = cval.first.c_str();
            expNode.append_child(pugi::node_cdata).set_value(
                    to_string(cval.second[HRL_CVAL_POS]).c_str()
                );
            expNode.append_child(pugi::node_cdata).set_value(
                    to_string(cval.second[HRL_CTILDEVAL_POS]).c_str()
                );
        }
    }
    // Now xmlDoc and namely the root node contains all dynamic tasks data
    // Write out the data structure.
    return xmlDoc.save_file("data/HRLDriverData.xml");
};

/*** loadExperience ************************************************
    Loads a pugixml document in memory, loops through all nodes and
    copy the C-values in the given taskTree
********************************************************************/
bool DataHandler::loadExperience(string srcPath, shared_ptr<Task> taskTree){
    string searchStr;
    // Load xml file
    pugi::xml_document xmlDoc;
    pugi::xml_parse_result result = xmlDoc.load_file(srcPath.c_str(),
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
        if (!xpathRootNode){
            cout << "Failed to load experience - no tasks found in the XML." << endl;
            return false;
        }

    // For each taskNode in the file, find the corresponding task in the
    // taskTree. Then loop through the cValNodes and merge their values
    // into the tasks cvals vector.
    for (pugi::xpath_node n: xmlRoot.select_nodes("taskNode")){
        pugi::xml_node xmlTask = n.node();
        // Find the task in the task tree with the same id
        shared_ptr<DynamicTask> task = dynamic_pointer_cast<DynamicTask>(
              DataHandler::_findTaskInTree(taskTree, (char) xmlTask.attribute("intVal").as_int()));
        if(task != nullptr){
            // For all child nodes (representing the cvals) insert their values into
            // the tasks cval vector.
            for(pugi::xpath_node c: xmlTask.select_nodes("cValNode")){
                pugi::xml_node xmlCValNode = c.node();
                pugi::xpath_node_set cvalSet = xmlCValNode.select_nodes("text()");
                task->cvals.insert(pair<string, array<double, 2>>(
                        (string) xmlCValNode.attribute("as").value(),
                        array<double, 2>
                          {{ cvalSet[HRL_CVAL_POS].node().text().as_double(), cvalSet[HRL_CTILDEVAL_POS].node().text().as_double() }}
                        )
                    );
            }
        }
    }
    return true;

}

void DataHandler::updateStats(double addReward){
    this->_episodeActionCnt++;
    this->_episodeReward += addReward;
};

void DataHandler::writeStats(){
    ofstream statFile;
    statFile.open("data/stats.txt", ios::app);
    statFile << this->_episodeCnt << "; " << this->_episodeActionCnt << "; " << this->_episodeReward << ";\n";
    statFile.close();
    this->_episodeActionCnt = 0;
    this->_episodeReward = 0.0;
    this->_episodeCnt++;
}
