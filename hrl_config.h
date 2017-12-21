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
#define HRL_EPSILON                 0.1
#define HRL_GAMMA                   0.99
#define HRL_MAX_EPISODES            10000
#define HRL_ALPHA_DECAY             (0.7 / HRL_MAX_EPISODES)      /*((HRL_ALPHA_START-HRL_ALPHA_END)/HRL_MAX_EPISODES) */
#define HRL_EPSILON_DECAY           (0.09 / HRL_MAX_EPISODES)     /* ((HRL_EPSILON-HRL_EPSILON_END)/HRL_MAX_EPISODES) */

/***** Task config ****/
#define HRL_STATIC_ROOT
#define HRL_STATIC_GEAR
#define HRL_STATIC_SPEED

/***** Rewarding ****/
#define HRL_DSFACTOR                1
#define HRL_TERMINATE_NEGREWARD     -10.0
/* Pseudo-rewards */
//#define HRL_PR_ENABLED
#define HRL_PR_STEER_NEAROUT        -10.0
#define HRL_PR_STEER_OUT            -100.0
#define HRL_PR_SPEED_SLOW           -10.0
#define HRL_PR_SPEED_FAST           10.0


/***** Data handling ****/
#define HRL_BACKUP_EPISODE_CNT      500

/***** MAXQ-Q specifics ****/
#define HRL_CVAL_POS       0
#define HRL_CTILDEVAL_POS  1

/***** Debugging & Output ****/
//#define HRL_DEBUG
#define HRL_EPISODE_MARKER          100 /* Display a marker to visualize progress each ... ep. */
#define HRL_EPISODE_COUNTER         1000 /* Display the completed episode count each ... ep. */

#endif /*HRLCONFIG_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
