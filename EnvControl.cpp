#include "EnvControl.h"

EnvControl::EnvControl(unsigned int maxEpisodes):
    _episodeCnt(0),_maxEpisodes(maxEpisodes),_lastState(CarState()),
    _lastActions(CarControl()),_stuckWatchdog(0),_isStuck(false),
    _isTerminated(false){
        CarControl dummy2(0.0, 0.0, 0, 0.0, 0.0);
        this->_lastActions = dummy2;
    };

EnvControl::~EnvControl(){};

void EnvControl::updateStatus(CarState& cs){
    // Check if the car is stuck right now (no movement and/or an angle greater
    // 45 degrees (pi/4) as in [Karavolos 2013])
    double angle = fabs(cs.getAngle());
    double speed = cs.getSpeedX() + cs.getSpeedY();

    if(speed < 0.1 || angle > 0.9) this->_stuckWatchdog++;
    else this->_stuckWatchdog = 0;
    // If the car is stuck for too long, set the _isStuck flag.
    if(this->_stuckWatchdog >= HRL_STUCK_MAX_GAMETICKS){
        this->_isStuck = true;
    }
    // Check if a termination condition is reached (stucked or out of track)
    double pos = cs.getTrackPos();
    if(this->_isStuck || (pos < HRL_TRACKLEAVE_RIGHT || pos > HRL_TRACKLEAVE_LEFT)){
        this->_isTerminated = true;
    }
};

void EnvControl::resetStatus(){
    this->_isStuck = false;
    this->_isTerminated = false;
    this->_stuckWatchdog = 0;
    CarState dummy1;
    this->_lastState = dummy1;
    CarControl dummy2(0.0, 0.0, 0, 0.0, 0.0);
    this->_lastActions = dummy2;
};

/* Calculates the feature values given the current CarState */
/* ----- Ideas for improvement:
          - sliding window over cs to avoid glitches
   -----*/
DiscreteFeatures EnvControl::getFeatures(CarState& cs){
    DiscreteFeatures dPhi;
    /***** Speed *************************************************************/
    double speedx = cs.getSpeedX();
    double speed = cs.getSpeedY();
    speed = sqrt(speedx * speedx + speed * speed);
    // Discretization table:
    // speed  | 0  | 0-10 | 10-20 | 20-30 | 30-40 | 40-50 | 50-75 | 75-100 | 100-125 | 125-150 | 150-175 | 175-200 | 200-240 | 240-280 | >280
    // state  | V0 | V1   | V2    | V3    | V4    | V5    | V6    | V7     | V8      | V9      | V10     | V11     | V12     | V13     | V14

    // Leverage the implementation of enum as integer and bool true as 1
    dPhi.speed = (DiscreteFeatures::speed_t) ( (speed > 0) + (speed > 10) + (speed > 20)
                    + (speed > 30) + (speed > 40) + (speed > 50) + (speed > 75) + (speed > 100)
                    + (speed > 125) + (speed > 150) + (speed > 175) + (speed > 200)
                    + (speed > 240) + (speed > 80));
    /***** Curvature *********************************************************/
    // For now, a weighted average over the angles and distanes is used to
    // calculate the curvature which is discretized afterwards.
    double sensorAngles[] = {-60,-50,-40,-30,-20,-15,-10,-6,-3,0,3,6,10,15,20,30,40,50,60};
    double res = 0;
    double angleDistanceSum = 0;
    double distanceSum = 0;
    for(int sens = 0; sens < 19; sens++){
        res = cs.getTrack(sens);
        angleDistanceSum += res * sensorAngles[sens];
        distanceSum += res;
    }
    res = angleDistanceSum / distanceSum;
    // Discretization table:
    // curvature | (-60)-(-45) | (-45)-(-30) | (-30)-(-22.5) | (-22.5)-(-15) | (-15)-(-10) | (-10)-(-6) | (-6)-(-3) | (-3)-3 | 3-6 | 6-10 | 10-15 | 15-22.5 | 22.5-30 | 30-45 | 45-60 |
    // state     | CL7         | CL6         | CL5           | CL4           | CL3         | CL2        | CL1       | C0     | CR1 | CR2  | CR3   | CR4     | CR5     | CR6   | CR7   |
    dPhi.curvature = (DiscreteFeatures::curvature_t) ( (res > -45.0) + (res > -30.0)
                        + (res > -22.5) + (res > -15.0) + (res > -10.0) + (res > -6)
                        + (res > -3.0) + (res > 3.0) + (res > 6.0) + (res > 10.0)
                        + (res > 15.0) + (res > 22.5) + (res > 30.0) + (res > 45.0) );
    #ifdef HRL_DEBUG
        cout << "Curvature: " << res << endl;
        cout << "|";
        cout << setfill(' ') << setw((int)dPhi.curvature) << " ";
        cout << "o";
        cout << setfill(' ') << setw(14 - (int)dPhi.curvature) << " ";
        cout << "|" << endl;
    #endif
    /***** TrackPos **********************************************************/
    double pos = cs.getTrackPos();
    // Discretization table:
    // pos   | >1.0 | 1.0-0.8 | 0.8-0.6 | 0.6-0.4 | 0.4-0.2 | 0.2-(-0.2) | (-0.2)-(-0.4) | (-0.4)-(-0.6) | (-0.6)-(-0.8) | (-0.8)-(-1.0) | <-1.0
    // state | PL5  | PL4     | PL3     | PL2     | PL1     | P0         | PR1           | PR2           | PR3           | PR4           | PR5
    dPhi.trackPos = (DiscreteFeatures::trackPos_t) ( (pos < 1.0) + (pos < 0.8)
                      + (pos < 0.6) + (pos < 0.4) + (pos < 0.2) + (pos < -0.2)
                      + (pos < -0.4) + (pos < -0.6) + (pos < -0.8) + (pos < -1.0));
    /***** RPM ***************************************************************/
    int rrpm = cs.getRpm();
    // Discretization table (following [Karavolos 2013]):
    // rpm   | 0-2k | 2-3k | 3-4k | 4-5k | 5-6.3k | 6.3-7.3k | 7.3-8k | 8-9.5k | >9.5k
    // state | RPM0 | RPM1 | RPM2 | RPM3 | RPM4   | RPM5     | RPM6   | RPM7   | RPM8
    dPhi.rpm = (DiscreteFeatures::rpm_t) ( (rrpm > 2000) + (rrpm > 3000) + (rrpm > 4000)
                + (rrpm > 5000) + (rrpm > 6300) + (rrpm > 7300) + (rrpm > 8000) + (rrpm > 9300));
    /***** Gear **************************************************************/
    dPhi.gear = cs.getGear();

    return dPhi;
};

/* Returns the actions available (based on the feature values) for the task */
vector<shared_ptr<Task>> EnvControl::getAllowedActions(shared_ptr<Task> task, DiscreteFeatures& fullFeatures){
    // Basically the constraints on each task can be enforced here.
    // If no constraints can be made to shrink the state-action space, all actions are returned.
    vector<shared_ptr<Task>> aAllowed;
    // Loop through the tasks attached actions and add only valid ones to aAllowed
    switch(task->id){
        // root constraints:
        case 0:
            for(shared_ptr<Task> a : task->children){
                aAllowed.push_back(a);
            }
            break;
        // speedCtrl constraints:
        case 1:
            for(shared_ptr<Task> a : task->children){
                // No accelerating if at maximum speed
                if(fullFeatures.speed == DiscreteFeatures::speed_t::V14 && a->id == 4) continue;
                // No breaking if already standing still
                if(fullFeatures.speed == DiscreteFeatures::speed_t::V0 && a->id == 6) continue;
                // else
                aAllowed.push_back(a);
            }
            break;
        // gearCtrl constraints
        case 2:
            for(shared_ptr<Task> a : task->children){
                aAllowed.push_back(a);
            }
            break;
        // steeringCtrl constraints:
        case 3:
            for(shared_ptr<Task> a : task->children){
                // Force steering in sharp curves
                if((fullFeatures.curvature <= DiscreteFeatures::curvature_t::CL4
                    || fullFeatures.curvature >= DiscreteFeatures::curvature_t::CR4) && a->id == 12) continue;
                // else
                aAllowed.push_back(a);
            }
            break;
    }
    return aAllowed;
};
/* Returns the overall MDP reward */
double EnvControl::getAbstractReward(CarState& cs){
    // As in previous works on this problem, the overall rewards is calculated
    // based on the distance traveled since the last call. Additionally, negative
    // rewards for leaving the track and for standing still seek to avoid
    // undesired states.
    double reward;
    double ds = fabs((cs.getDistFromStart() - this->_lastState.getDistFromStart()) * HRL_DSFACTOR);

    // Starting race negative distFromStart hack, make rewards smaller
    ds = (ds > 10.0) ? 0 : ds;
    // Positive rewards for good actions, negative ones for bad termination
    if(this->_isTerminated) reward = HRL_TERMINATE_NEGREWARD;
    else reward = ds;
    // Save the current state
    this->_lastState = cs;
    return reward;
};

/* Returns the control actions based on the root task decision or termination */
CarControl EnvControl::getActions(shared_ptr<Task> a){
    CarControl cc;
    // Take the last actions as basis - the agent needs to explicitly adjust an
    // action to change behavior. This eases up the sequential decision making for
    // a parallel problem.
    cc = this->_lastActions;
    // If the current episode is terminated, send a request for restart.
    if(this->_isTerminated){
        cc.setMeta(1);
        // Count up the epsiodes
        this->_episodeCnt++;
    } else {
        // Mapping of action ids to concrete action values
        switch(a->id){
            // SpeedCtrl
            case 4: cc.setAccel(1.0); break;
            case 5: cc.setAccel(0.0); break;
            case 6: cc.setAccel(0.0); cc.setBrake(1.0); break;
            // GearCtrl
            case 7: cc.setGear(this->_lastActions.getGear() + 1); break;
            case 8: cc.setGear(this->_lastActions.getGear()); break;
            case 9: cc.setGear(this->_lastActions.getGear() - 1); break;
            // SteeringCtrl
            case 10: cc.setSteer(0.5); break;
            case 11: cc.setSteer(0.1); break;
            case 12: cc.setSteer(0.0); break;
            case 13: cc.setSteer(-0.1); break;
            case 14: cc.setSteer(-0.5); break;
        }
    }
    this->_lastActions = cc;
    return cc;
};
