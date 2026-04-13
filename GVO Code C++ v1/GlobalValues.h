#pragma once
#include <string>
#include <cstdint>
#include "CommUtils.h"
#include "StateVar.h"

extern int ComPortNumber;
extern std::string OmsDllPath;
extern std::string TeleApiDllPath;

extern StateVar Xstate;
extern StateVar Ystate;

extern void* pndomem;

// Numeric globals
extern double TrkRate;
extern double RAFact, DECFACT;
extern double C_Lat, C_Long;
//extern double SidTimeFract; 
extern double SidTime;
extern double RANow, DECNow;
extern double RaTarget, DecTarget, Meridian, EastHor, WestHor;
extern double EastHA, WestHA, NorthHA, SouthHA, A, H;
extern double RaPos, decPos, tdecfact;

// Slew / velocities
extern std::string xvlslew, yvlslew, xvl5inch, yvl5inch;
extern std::string xvl, xac, xacmax, xvlmax, yvl, yac, yacmax, yvlmax;

// Flags & booleans
extern int DecBack;
extern int yPole;
extern bool LastDecNorth;
extern bool SlewSelect;
extern bool EastOfMeridian;
extern bool LookingEast;
extern bool TargetEastOfMeridian;

extern uint8_t keystroke, pcxdataout, status, HalfSecondCounter;
extern int quadrant, targetQuadrant;
extern bool movingRA, movingDEC;
extern uint8_t io;
extern bool EastPushed, WestPushed, NorthPushed, SouthPushed, ManualPushed;

extern int RAHr, RAMin, RASec;
extern int DECDeg, DECMin, DECSec;
extern int altdeg, altmin, altsec, azdeg, azmin, azsec;

extern bool Parkit, NorthofZenith, NoPassword;
extern char Response[256];

extern std::string name, commandBuffer;

// Mathematical constants
extern double pi;
extern double pi2;
extern double DToR;
extern double HToR;
extern double RToD;
extern double RToH;
extern double SToR;

// Astronomical constants
extern double EarthRadius;
extern double SolarParallax;

// Coordinate system identifiers
extern int FK4System;
extern int FK5System;

extern bool TimerEnabled;