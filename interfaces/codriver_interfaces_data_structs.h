// File for interfaces
// WARNING! AUTOMATICALLY GENERATED - DO NOT EDIT
// Origin file: VeDi2025_codriver_interfaces_v13.10.csv
// Origin CRC32: 2308865970

#ifndef codriver_interfaces_data_structs_h
#define codriver_interfaces_data_structs_h

#if defined(_DS1401)
#include "ds1401_defines.h"
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// interfaces version for printing
#define CODRIVERINTERFACESVER 1310

#if defined(MATLAB_MEX_FILE) || defined(_DS1401) || defined(_WIN32)
#pragma pack(push, 1)
typedef struct {
#else
typedef struct __attribute__((packed)) {
#endif
  int32_t ID; /* Enumeration 
01=Scenario message
11=Manoeuvre message, */
  int32_t Version; /* Value is: 1205 */
  int32_t CycleNumber; /* This is an increasing number */
  double ECUupTime; /* Means system up-time */
  double AVItime; /* From DATALOG PC, CANape Multimedia1 signal */
  double TimeStamp; /* UTC time difference after 1st January 1970, obtained from GPS time with leap seconds (Unix epoch) */
  int32_t Status; /* 0 = ACTIVE, 0 != Fail  (means working correctly or not) */
  int32_t DrivingStyle; /* 0 = NOT_DEFINED, 1 = ECO, 2 = NORMAL, 3 = SPORT */
  int32_t LowLevelLongControlType; /* This field is used to switch between different low level control. */
  int32_t LowLevelLatControlType; /* This field is used to switch between different low level control. */
  int32_t ConfigParamInt1;
  int32_t ConfigParamInt2;
  int32_t ConfigParamInt3;
  int32_t ConfigParamInt4;
  int32_t ConfigParamInt5;
  double ConfigParamDouble1;
  double ConfigParamDouble2;
  double ConfigParamDouble3;
  double ConfigParamDouble4;
  double ConfigParamDouble5;
  double HumanLongitudinalBias;
  double HumanLateralBias;
  double HumanModelParameters[20];
  double VLgtFild;
  double ALgtFild;
  double ALatFild;
  double YawRateFild; /* Speed module, not longitudinal speed */
  double SideSlipAngle;
  double SteerWhlAg; /* Positive when the car is turning left */
  double SteerWhlAgSpd; /* Derivative of steering wheel angle */
  double SteerTorque;
  double EngineSpeed;
  double MasterCylinderPressure;
  double BrakePedPos;
  double FuelConsumption;
  double GasPedPos;
  double EngineTorque;
  double EngineFrictionTorque;
  double MaxEngineTorque;
  double EngineTorqueDriverReq;
  double MaxEngineTorqueNorm;
  int32_t ExTemp;
  int32_t BrakePedalSwitchNCSts; /* 0 = RELEASED; 1 = PRESSED,  2 = UNKNOWN */
  int32_t ActGear;
  int32_t IndTurnComm; /* 0 = UNKNOWN; 1 = OFF; 2 = LEFT; 3 = RIGHT */
  int32_t VehicleID; /* 10 = CRF car, 20 = MIA, 30 = OpenDS, 40 = Carmaker */
  int32_t VehicleType; /* 1 = combustion engine, 2 = electric car */
  int32_t VehicleLightsStatus;
  double VehicleLen; /* Total length from front bumper to the rear bumper */
  double VehicleWidth;
  double VehicleMass;
  double VehicleBarLongPos; /* Distance from reference point to front bumper */
  double RequestedCruisingSpeed;
  int32_t AutomationLevel; /* 0 = NO AUTOMATION, 1 = ASSISTED, 2 = PARTIAL AUTOMATION, 3 = CONDITIONAL AUTOMATION, 4 = HIGH AUTOMATION, 5 = FULL AUTOMATION, 6 = UNKNOWN */
  int32_t SystemStatus;
  int32_t SystemMode;
  int32_t CurrentLane; /* Nomenclature from ADASIS: 0 = Unknown, 1 = Emergency lane, 2 = Single-lane road, 3 = Left-most lane, 4 = Right-most lane, 5 = One of middle lanes on road with three or more lanes */
  int32_t NrObjs; /* Limited to 20 max number of objects, selection needed (if more might be limited to nearest objects) */
  int32_t ObjID[20];
  int32_t ObjClass[20]; /* unknown(0), pedestrian(1), cyclist(2), moped(3), motorcycle(4), passengerCar(5), bus(6), lightTruck(7), heavyTruck(8), trailer(9), specialVehicles(10), tram(11), roadSideUnit(15) */
  int32_t ObjSensorInfo[20]; /* xxxxxxxD = LIDAR
xxxxxxDx = CAMERA
xxxxxDxx = RADAR
xxxxDxxx = V2V
xxxDxxxx = BLINDSPOT
xxDxxxxx = ULTRASOUND
xDxxxxxx = RACAM_VIDEO
Dxxxxxxx = RACAM_RADAR  */
  double ObjX[20]; /* Center of the object */
  double ObjY[20]; /* Centre of the object */
  double ObjLen[20]; /* Along object speed direction, along vehicle axis for stationary obstacles. 0 means unknown. */
  double ObjWidth[20]; /* Perpendicular to object speed direction, perpendicular to vehicle axis for stationary obstacles. 0 means unknown. */
  double ObjVel[20]; /* Speed module, not longitudinal speed */
  double ObjCourse[20]; /* In vehicle reference system, positive to the left */
  double ObjAcc[20]; /* Tangential acceleration */
  double ObjCourseRate[20];
  double ObjAdasisLongitudeValues[20]; /* For test purpose, only first point is necessary */
  double ObjAdasisLatitudeValues[20]; /* For test purpose, only first point is necessary */
  double ObjAdasisHeadingValues[20]; /* From East to North */
  int32_t ObjAdasisCurvatureNrs[20];
  double ObjAdasisCurvatureDist[400];
  double ObjAdasisCurvatureValues[400]; /* Positive for left curves */
  int32_t ObjAgentID[20]; /* 0 -> Non cooperative Objstacle 
10000 + X -> Codriver parametr. X  
(X from 1 to 50)
20000 + X -> MotionPlanner parametr. X
30000  -> Future Agents */
  double ObjT0[20]; /* ECU up time when the primitive starts (based on ECUs given by Scenario Messages) */
  double ObjLongitudinalMan1[400]; /* The type of coef depends on AgentID */
  double ObjLongitudinalMan2[400]; /* The type of coef depends on AgentID */
  double ObjLateralMan1[400]; /* The type of coef depends on AgentID */
  double ObjLateralMan2[400]; /* The type of coef depends on AgentID */
  double ObjCostMan1[20];
  double ObjCostMan2[20];
  double LaneWidth;
  double LatOffsLineR; /* positive to the left.  */
  double LatOffsLineL;
  double LaneHeading;
  double LaneCrvt; /* Positive for left curves, current curvature (at the cars position) */
  double DetectionRange;
  int32_t LeftLineConf;
  int32_t RightLineConf;
  int32_t LeftLineType; /* 0 = dashed, 
1 = solid, 
2 = undecided, 
3 = road edge, 
4 = double lane, 
5 = botts dots, 
6 = not visible, 
7 = invalid */
  int32_t RightLineType; /* 0 = dashed, 
1 = solid, 
2 = undecided, 
3 = road edge, 
4 = double lane, 
5 = botts dots, 
6 = not visible, 
7 = invalid */
  double GpsLongitude; /* As measured by GPS, East positive */
  double GpsLatitude; /* As measured by GPS, North positive */
  double GpsSpeed; /* As measured by GPS */
  double GpsCourse; /* With respect to North, clockwise, as measured by GPS */
  double GpsHdop; /* Diluition of precision as indicated by GPS */
  double EgoLongitude; /* After position filter, referred to barycentre */
  double EgoLatitude; /* After position filter, referred to barycentre */
  double EgoCourse; /* With respect to North, clockwise, after position filter */
  double EgoSemiMajorAxis; /* from V2X standard */
  double EgoSemiMinorAxis; /* from V2X standard */
  double EgoSemiMajorOrientation; /* from V2X standard */
  int32_t FreeLaneLeft; /* 0 = NOT AVAILABLE; 1 = FREE; 2 = Unknown */
  int32_t FreeLaneRight; /* 0 = NOT AVAILABLE; 1 = FREE; 2 = Unknown */
  int32_t SideObstacleLeft; /* 0 = NO OBSTACLE: 1 = OBSTACLE PRESENT; 2 = Unknown */
  int32_t SideObstacleRight; /* 0 = NO OBSTACLE; 1 = OBSTACLE PRESENT; 2 = Unknown */
  int32_t BlindSpotObstacleLeft; /* 0 = NO OBSTACLE; 1 = OBSTACLE PRESENT; 2 = Unknown */
  int32_t BlindSpotObstacleRight; /* 0 = NO OBSTACLE; 1 = OBSTACLE PRESENT; 2 = Unknown */
  int32_t LeftAdjacentLane; /* 0 = NOT DETECTED; 1 = DETECTED; 2 = Unknown */
  int32_t RightAdjacentLane; /* 0 = NOT DETECTED; 1 = DETECTED; 2 = Unknown */
  int32_t NrReferenceLines; /* Currently up to 5 paths, the first is the main path / reference line, others can be added at junctions (stubs) */
  int32_t NrLanesDrivingDirection; /* Considered at vehicle position */
  int32_t NrLanesOppositeDirection; /* Considered at vehicle position */
  double AdasisLongitudeValues[5]; /* For test purpose, only first point is necessary */
  double AdasisLatitudeValues[5]; /* For test purpose, only first point is necessary */
  double AdasisHeadingValues[5]; /* From East to North */
  int32_t AdasisCurvatureNrs[5];
  double AdasisCurvatureDist[100]; /* Curvature distance (60 points for ego, 10, 10, 10, 10 per gli altri path) or (40, 15,15,15,15) */
  double AdasisCurvatureValues[100]; /* Positive for left curves */
  int32_t AdasisSpeedLimitsNrs[5];
  double AdasisSpeedLimitDist[10];
  int32_t AdasisSpeedLimitValues[10]; /* 0 means unknown */
  int32_t AdasisSlopeNrs[5];
  double AdasisSlopeDist[100];
  double AdasisSlopeValues[100];
  int32_t AdasisLanesDistNrs[5];
  double AdasisLanesDistances[100];
  double AdasisRoadLeftEdge[100];
  double AdasisRoadRightEdge[100];
  double AdasisLeftLaneLeftEdge[100];
  double AdasisLeftLaneRightEdge[100];
  double AdasisCenterLaneLeftEdge[100];
  double AdasisCenterLaneRightEdge[100];
  double AdasisRightLaneLeftEdge[100];
  double AdasisRightLaneRightEdge[100];
  int32_t AdasisLeftLaneStopsNrs[4];
  int32_t AdasisCenterLaneStopsNrs[4];
  int32_t AdasisRightLaneStopsNrs[4];
  double AdasisLeftLaneStopsDist[4];
  double AdasisCenterLaneStopsDist[4];
  double AdasisRightLaneStopsDist[4];
  double AdasisLanesBiasDist[10];
  double AdasisLaneLeftBias[10];
  double AdasisLaneCenterBias[10];
  double AdasisLaneRightBias[10];
  int32_t PriorityLevelNrs[5];
  double PriorityLevelDist[10];
  int32_t PriorityLevelValues[10]; /* Not directly available from ADASIS, derived from other info */
  int32_t NrTrfLights; /* Only first traffic ligh is described if available */
  double TrfLightDist;
  int32_t TrfLightCurrState; /* 1 = Green, 2 = Yellow, 3 = Red, 0 = Flashing */
  double TrfLightFirstTimeToChange;
  int32_t TrfLightFirstNextState; /* 1 = Green, 2 = Yellow, 3 = Red, 0 = Flashing */
  double TrfLightSecondTimeToChange;
  int32_t TrfLightSecondNextState; /* 1 = Green, 2 = Yellow, 3 = Red, 0 = Flashing */
  double TrfLightThirdTimeToChange;
  int32_t NrPedCross; /* Only first pedestrian crossing is described if available */
  double PedCrossDist;
} input_data_str;
#if defined(MATLAB_MEX_FILE) || defined(_DS1401) || defined(_WIN32)
#pragma pack(pop)
#endif

#if defined(MATLAB_MEX_FILE) || defined(_DS1401) || defined(_WIN32)
#pragma pack(push, 1)
typedef struct {
#else
typedef struct __attribute__((packed)) {
#endif
  int32_t ID; /* Enumeration 
01=Scenario message
11=Manoeuvre message, */
  int32_t Version; /* Value is: 1205 */
  int32_t CycleNumber; /* This is an increasing number */
  double ECUupTime; /* Means system up-time */
  double AVItime; /* From DATALOG PC, CANape Multimedia1 signal */
  double TimeStamp; /* UTC time difference after 1st January 1970, obtained from GPS time with leap seconds (Unix epoch) */
  int32_t Status; /* 0 = ACTIVE, 0 != Fail  (means working correctly or not) */
  int32_t AgentID; /* 0 -> Non cooperative Objstacle 
10000 + X -> Codriver parametr. X  
(X from 1 to 50)
20000 + X -> MotionPlanner parametr. X
30000  -> Future Agents */
  int32_t CoDriverVersion;
  int32_t AgentLowLevelLatControlType; /* This field is used to identify the lowlevelcontrl used by codriver */
  int32_t CoDriverParamInt1;
  int32_t CoDriverParamInt2;
  int32_t CoDriverParamInt3;
  int32_t CoDriverParamInt4;
  int32_t CoDriverParamInt5;
  double CoDriverParamDouble1;
  double CoDriverParamDouble2;
  double CoDriverParamDouble3;
  double CoDriverParamDouble4;
  double CoDriverParamDouble5;
  double ComputationTime;
  int32_t ConfigChecksum;
  int32_t TimeHeadwayPolicy; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t LegalSpeedPolicy; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t LegalSpeedLimit;
  int32_t LandmarkPolicy; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t LandmarkType; /* 0 = NO LANDMARK
1 = STOP SIGN
2 = PEDESTRIAN CROSSING
3 = YIELD SIGN
4 = SEMAPHORE */
  int32_t AccelerationPolicyForCurve; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW LEFT
3 = YELLOW RIGHT
4 = RED LEFT
5 = RED RIGHT */
  int32_t RearTimeHeadwayPolicyLeft; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t LeftThreatType; /* 0 = SIDE OBSTACLE; 1 = RUN OFF ROAD */
  int32_t RearTimeHeadwayPolicyRight; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t RightThreatType; /* 0 = SIDE OBSTACLE; 1 = RUN OFF ROAD */
  int32_t LeftLanePolicy; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t RightLanePolicy; /* 0 = NOT COMPUTED
1 = GREEN
2 = YELLOW
3 = RED */
  int32_t TravelTimePolicy; /* 0 = NOT COMPUTED
1 = COMFORT
2 = NORMAL
3 = SPORT */
  int32_t RecommendedGear; /* 0 = NOT COMPUTED */
  int32_t NegotiationStatus; /* 0 = No Negotiation, 1 = Negotiation In Progress */
  int32_t TriggerV2XPrimitive1Status; /* 0 = NOT CHANGED
1 = CHANGED */
  int32_t TriggerV2XPrimitive2Status; /* 0 = NOT CHANGED
1 = CHANGED */
  int32_t TargetID;
  int32_t TargetClass; /* 0 = UNCLASSIFIED
1 = UNKNOWN SMALL
2 = UNKNOWN BIG
3 = PEDESTRIAN
4 = BIKE OR MOTORBIKE
5 = CAR
6 = TRUCK OR BUS */
  int32_t TargetSensorInformation; /* xxxxxxxD = LIDAR
xxxxxxDx = CAMERA
xxxxxDxx = RADAR
xxxxDxxx = V2V */
  double TargetX;
  double TargetY;
  double TargetDistance; /* Distance to intersection obtained from absolute positions of vehicle and target */
  double TargetLength;
  double TargetWidth;
  double TargetSpeed; /* 0 means stationary */
  double TargetCourse;
  double TargetAcceleration;
  double TargetCourseRate;
  int32_t TargetDrivingMode; /* 0 = NO AUTOMATION, 1 = ASSISTED, 2 = PARTIAL AUTOMATION, 3 = CONDITIONAL AUTOMATION, 4 = HIGH AUTOMATION, 5 = FULL AUTOMATION, 6 = UNKNOWN */
  int32_t NrArcs;
  double ArcStartAbscissa[30]; /* It is a vector of the longitudinal abcissa coordinates */
  double ArcCurvature[30];
  int32_t NTrajectoryPoints; /* Limited to 23 max number of trajectory points */
  double TrajectoryPointITime[23]; /* Unix epoch */
  double TrajectoryPointIX[23]; /* In vehicle reference system */
  double TrajectoryPointIY[23]; /* In vehicle reference system */
  double T0; /* ECU up time when the primitive starts (based on ECUs given by Scenario Messages) */
  double V0; /* Longitudinal speed at the time of generation of the motor primitive */
  double A0; /* Time derivative of speed, also valid for second trajectory */
  double T1; /* Relative to T0 */
  double J0; /* Time derivative of acceleration */
  double S0; /* Time derivative of jerk */
  double Cr0; /* Time derivative of snap */
  double T2; /* Relative to T0 */
  double J1; /* Time derivative of acceleration */
  double S1; /* Time derivative of jerk */
  double Cr1; /* Time derivative of snap */
  double Sn0; /* Also valid for second trajectory */
  double Alpha0; /* Also valid for second trajectory */
  double Delta0; /* Curvature of vehicle trajectory relative to lane curvature */
  double T1n;
  double Jdelta0; /* Currently, time derivative * v2 is given (m s-3) */
  double Sdelta0; /* Currently, second time derivative * v2 is given (m s-4) */
  double Crdelta0; /* Currently, third time derivative * v2 is given (m s-5) */
  double T2n;
  double Jdelta1; /* Currently, time derivative * v2 is given (m s-3) */
  double Sdelta1; /* Currently, second time derivative * v2 is given (m s-4) */
  double Crdelta1; /* Currently, third time derivative * v2 is given (m s-5) */
  double LongitudinalMan1[20]; /* The type of coef depends on AgentID */
  double LongitudinalMan2[20]; /* The type of coef depends on AgentID */
  double LateralMan1[20]; /* The type of coef depends on AgentID */
  double LateralMan2[20]; /* The type of coef depends on AgentID */
  double CostMan1;
  double CostMan2;
  int32_t FirstManoeuverTypeLong; /* E.g: follow object, free flow, stopping, etc. */
  int32_t FirstManoeuverTypeLat; /* E.g: lane keeping, lane change left, lane change right, etc. */
  double TargetEgoSpeed; /* Speed of the vehicle at the end of the manoeuvre */
  double TargetEgoLongitudinalAcceleration; /* Longitudinal acceleration required to perform the calculated manoeuvre */
  double TargetEgoDistanceToPreceedingVehicle; /* Distance from the preceding vehicle at the end of the manoeuvre */
  double SteeringWheelAngle[30];
  double LongitudinalAcceleration[30];
  int32_t LongitudinalControlActionType;
  double LongitudinalControlAction[30]; /* This signal meaning depends on the field LongitudinalControlActionType */
  int32_t LateralControlActionType;
  double LateralControlAction[30]; /* This signal meaning depends on the field LaterallControlActionType */
  double MotorCortexIn[1681];
  double MotorCortexOut[1681];
  double AgentAdasisLongitudeValues; /* For test purpose, only first point is necessary */
  double AgentAdasisLatitudeValues; /* For test purpose, only first point is necessary */
  double AgentAdasisHeadingValues; /* From East to North */
  int32_t AgentAdasisCurvatureNrs;
  double AgentAdasisCurvatureDist[20];
  double AgentAdasisCurvatureValues[20]; /* Positive for left curves */
} output_data_str;
#if defined(MATLAB_MEX_FILE) || defined(_DS1401) || defined(_WIN32)
#pragma pack(pop)
#endif


// Scenario message union
typedef union {
  input_data_str data_struct;
  char data_buffer[sizeof(input_data_str)];
} scenario_msg_t;

// Manoeuvre message union
typedef union {
  output_data_str data_struct;
  char data_buffer[sizeof(output_data_str)];
} manoeuvre_msg_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
