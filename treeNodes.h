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

/* Interface classes */
class iTask {
  public:
    /* Constructor */
    iTask(){};
    /* Destructor */
    ~iTask(){};

    char id;
    string name;
    bool isPrimitive;
    bool isStatic;
    vector<shared_ptr<iTask>> children;
    /* ABSTRACT POLYMORPHIC METHODS */
    /* Evaluation -- QNode behavior */
    virtual void learn(string featureValues, string allowedActions);
    virtual double getMaxQValue(string featureValues);
    /* Strategy -- MAXNode behavior */
    virtual void getActionSelection(string featureValues, string allowedActions, char& astarID, char& apiID);
    /* Printing for debugging */
    virtual string toString();
};

class primitiveAction: public iTask {
  public:
    /* Constructor */
    primitiveAction(char id, string name);
    /* Destructor */
    ~primitiveAction();
};

class dynamicTask:public iTask {
  private:
    double _alphaStart;
    double _gamma;
    double _epsilon;
    int _totalActionCount;
    unsigned int _stepCount;
    string _lastFeatActionPair;
    double _getPseudoReward(string featureValues);
  public:
    /* Constructor */
    dynamicTask(char id, string name, double alphaStart, double gamma, double epsilon, int totalActionCount);
    /* Destructor */
    ~dynamicTask();
    /* Interface imlpementation */
    /* Evaluation -- QNode behavior */
    // Making the C-table accessible for the sake of less memory occupation
    map<string, array<double, 2> > cvals;
    void learn(string featureValues, char astarID, char apiID);
    double getMaxQValue(string featureValues);
    /* Strategy -- MAXNode behavior */
    void getActionSelection(string featureValues, string allowedActions, char& astarID, char& apiID);
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
