/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef TREENODES_H_
#define TREENODES_H_

// Decaying learning and exploration over the long term
#ifdef RL_LIN_ALPHA_DECAY
    #define RL_MIN_ALPHA    0.1
#endif
#ifdef RL_LIN_EPSILON_DECAY
    #define RL_MIN_EPSILON  0.05
#endif

#include <map>
#include <array>
#include <vector>
#include <utility> // pair
#include <memory> // shared_ptr
#include <string>
#include <sstream> // stringstream
#include <iomanip> // stringstream manipulations setfill, setw
#include <chrono> // For high resolution seeding
#include <random> // More advanced normal distribution pseudo-random generators

#include "DiscreteFeatures.h"

#define RL_CVAL_POS       0
#define RL_CTILDEVAL_POS  1

using namespace std;

/*****************************************************************************/
/****** BASIC TASK CLASS used for inheritance and polymorphism ***************/
/*****************************************************************************/
class Task {
  public:
    /* Constructor */
    Task();
    /* Destructor */
    virtual ~Task();

    char id;
    string name;
    bool isPrimitive;
    bool isStatic;
    vector<shared_ptr<Task>> children;
    /* Returns part of the full feature vector to be used in the particular task */
    virtual string getTaskFeatureString(DiscreteFeatures&);
    /*** POLYMORPHIC METHODS ***/
    /* Evaluation -- QNode behavior */
    virtual void learn(DiscreteFeatures&, vector<shared_ptr<Task>>&, double);
    virtual double getExternalMaxCValue(DiscreteFeatures&);
    /* Strategy -- MAXNode behavior */
    virtual shared_ptr<Task> getActionSelection(DiscreteFeatures&, vector<shared_ptr<Task>>&);
    /* Printing for debugging */
    virtual string toString();
};

/*****************************************************************************/
/****** DYNAMIC (LEARNING) TASK **********************************************/
/*****************************************************************************/
class DynamicTask:public Task {
  private:
    double _alpha;
    double _gamma;
    double _epsilon;
    int _totalActionCount;
    unsigned int _stepCount;
    /* Variables used for learning */
    string _featureAStarPair;
    string _lastFeatureAPiPair;
    /* Randomness of exploration */
    mt19937 _gen;
    uniform_real_distribution<double> _prob;
    // Private methods
    double _getPseudoReward(string);
    /* Returns the best action and its value in the given state regarding
       the INTERNAL completion function (including pseudo-rewards) */
    pair<shared_ptr<Task>, double> _getInternalMaxCValue(string, vector<shared_ptr<Task>>&);
    /* Returns the value of a feature-action pair regarding the INTERNAL
       completion function */
    double _getInternalCValue(string);
    /* Returns the value of a feature-action pair regarding
       the EXTERNAL completion function (no pseudo-rewards) */
    double _getExternalCValue(string);
  public:
    /* Constructor */
    DynamicTask(char, string, double, double, double, int);
    /* Destructor */
    virtual ~DynamicTask();
    /* Evaluation -- QNode behavior */
    // Making the C-table accessible for the sake of less memory occupation
    map<string, array<double, 2> > cvals;
    virtual void learn(DiscreteFeatures&, vector<shared_ptr<Task>>&, double);
    virtual double getExternalMaxCValue(DiscreteFeatures&);
    /* Strategy -- MAXNode behavior */
    virtual shared_ptr<Task> getActionSelection(DiscreteFeatures&, vector<shared_ptr<Task>>&);
};

/*****************************************************************************/
/****** PRIMITIVE ACTION (more like a dummy) *********************************/
/*****************************************************************************/
class PrimitiveAction: public Task {
  public:
    /* Constructor */
    PrimitiveAction(char id, string name);
    /* Destructor */
    virtual ~PrimitiveAction();

};

/*****************************************************************************/
/****** STATIC (FIXED CONTROL) TASKS *****************************************/
/* These tasks do not learn and pick subtasks/primitive actions attached to  */
/* them with a fixed strategy.                                               */
/*****************************************************************************/
/*** Static gear control ***/
class StaticGearControl: public Task {
  public:
    /* Constructor */
    StaticGearControl(char id);
    /* Destructor */
    virtual ~StaticGearControl();
    virtual shared_ptr<Task> getActionSelection(DiscreteFeatures&, vector<shared_ptr<Task>>&);
};
/*** Static root node ***/
class StaticRoot: public Task {
    char _lastAction;
    mt19937 _gen;
    uniform_int_distribution<int> _prob;
  public:
    /* Constructor */
    StaticRoot(char);
    /* Destructor */
    virtual ~StaticRoot();
    virtual shared_ptr<Task> getActionSelection(DiscreteFeatures&, vector<shared_ptr<Task>>&);
};

#endif /*TREENODES_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
