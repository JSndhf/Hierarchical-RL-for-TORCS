/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef HRLCONFIG_H_
#define HRLCONFIG_H_

/***** Server configuration ****/
#define HRL_DEFAULT_SERVERPORT      3001
#define HRL_MAX_SERVERPORT          3009
#define HRL_RACECONFIG_BASE         "/home/josch/Schreibtisch/HReinforcementLearning/hrlAgent/raceconfig/practice"

/***** Parameters for termination control ****/
#define HRL_STUCK_MAX_GAMETICKS     50
#define HRL_TRACKLEAVE_RIGHT        -1.2
#define HRL_TRACKLEAVE_LEFT         1.2

/***** Experiment setup ****/
#define HRL_ALPHA_START             0.8
#define HRL_EPSILON                 0.2
#define HRL_GAMMA                   0.99
#define HRL_MAX_EPISODES            1000
#define HRL_ALPHA_DECAY             ((0.8-0.1)/HRL_MAX_EPISODES)
#define HRL_EPSILON_DECAY           ((0.2-0.05)/HRL_MAX_EPISODES)

/***** Rewarding ****/
#define HRL_DSFACTOR                1
#define HRL_TERMINATE_NEGREWARD     -3.0


/***** Data handling ****/
#define HRL_BACKUP_EPISODE_CNT      500

/***** MAXQ-Q specifics ****/
#define HRL_CVAL_POS       0
#define HRL_CTILDEVAL_POS  1

/***** Debugging ****/
//#define HRL_DEBUG

#endif /*HRLCONFIG_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
