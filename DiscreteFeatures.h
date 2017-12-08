/***************************************************************************
    Description:

    Author: J.Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/

#ifndef DISCRETEFEATURES_H_
#define DISCRETEFEATURES_H_

class DiscreteFeatures {
  public:
    typedef enum : int {V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14} speed_t;
    typedef enum : int {CL6, CL5, CL4, CL3, CL2, CL1, C0, CR1, CR2, CR3, CR4, CR5, CR6} curvature_t;
    typedef enum : int {PL5, PL4, PL3, PL2, PL1, P0, PR1, PR2, PR3, PR4, PR5} trackPos_t;
    typedef enum : int {RPM0, RPM1, RPM2, RPM3, RPM4, RPM5, RPM6, RPM7, RPM8} rpm_t;
    /* Members */
    speed_t speed;
    curvature_t curvature;
    trackPos_t trackPos;
    rpm_t rpm;
    int gear;
};

#endif /*DISCRETEFEATURES_H_*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
