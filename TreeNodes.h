/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef TREENODES_H_
#define TREENODES_H_

// Preprocessor routines to speed calculation up
/* Decaying alpha */
#ifdef __LIN_ALPHA_DECAY__
    #define MIN_ALPHA     0.1
    #define MAX_EPISODES  1500
    #define GET_ALPHA(x)  (x-(x-MIN_ALPHA)/MAX_EPISODES)
#else
    #define GET_ALPHA(x)  (x)
#endif

#include <map>
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

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
    /*** POLYMORPHIC METHODS ***/
    /* Evaluation -- QNode behavior */
    virtual void learn();
    virtual double getMaxQValue(string);
    /* Strategy -- MAXNode behavior */
    virtual shared_ptr<Task> getActionSelection(string, vector<shared_ptr<Task>>&);
    /* Printing for debugging */
    virtual string toString();
};

/*****************************************************************************/
/****** DYNAMIC (LEARNING) TASK **********************************************/
/*****************************************************************************/
class DynamicTask:public Task {
  private:
    double _alphaStart;
    double _gamma;
    double _epsilon;
    int _totalActionCount;
    unsigned int _stepCount;
    string _lastFeatActionPair;
    double _getPseudoReward(string);
  public:
    /* Constructor */
    DynamicTask(char, string, double, double, double, int);
    /* Destructor */
    virtual ~DynamicTask();
    /* Evaluation -- QNode behavior */
    // Making the C-table accessible for the sake of less memory occupation
    map<string, array<double, 2> > cvals;
    virtual void learn();
    virtual double getMaxQValue(string);
    /* Strategy -- MAXNode behavior */
    virtual shared_ptr<Task> getActionSelection(string, vector<shared_ptr<Task>>&);
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
    virtual shared_ptr<Task> getActionSelection(string, vector<shared_ptr<Task>>&);
};
/*** Static root node ***/
class StaticRoot: public Task {
  public:
    /* Constructor */
    StaticRoot(char);
    /* Destructor */
    virtual ~StaticRoot();
    virtual shared_ptr<Task> getActionSelection(string, vector<shared_ptr<Task>>&);
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
