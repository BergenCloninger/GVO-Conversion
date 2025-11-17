#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <functional> //this may or may not be required
#include <cstdint>

//variable definitions:
// record = struct
// real = double
struct Coord {
    double RA;
    double Dec;
    double RAGoto;
    double DecGoto;
    double RASync;
    double DecSync;
};

using TeleAPICallBack = std::function<void(int)>;

enum class Statevar {
    Off,
    Tracking,
    CorrectingE,
    CorrectingW,
    CorrectingN,
    CorrectingS,
    Slewing
};

class TelescopeControl {
public:
	void Initialize();
	void JogNorth();
	void JogSouth();
	void JogEast();
	void JogWest();
	void Stop();
	void Park();

private:
	void DLLWork(int i);
};

// (*
// //GVO
// //*******************************************************************************
// RA
// tracking rate -       SID=1.00273790935  solar time / sidereal time

// gear ration = 480/1
// 	1 rev of worm = 360/480 = .75 degrees of sky  = 2700  ArcSec  of sky

// 	stepper = 5,000 usteps /rev X 18 gear reduction =  90,000 usteps/rev of worm
// 	1 rev of worm  2700 arc sec sky
// 	1 ustep of worm  = .03  arc sec of sky(2700/90,000)  = 33.3333333   usteps/arc sec
// 	1 ustep of worm = .046875 arc sec sky       = 21.3333 usteps / arc sec
// 	33.3333333  usteps/arc sec   =  120,000 usteps/deg

// tracking = 14.959043494948 arcsec/sec = 498.635 usteps/sec  trackking rate

// 	RA FACT (steps/deg) = 120,000
// //********************************************************************************************
// DEC
	
// 	gear ration = 338/1
// 	1 rev of worm = 360/338 = 1.0650887573964497 degrees of sky  = 3834.31952  ArcSec  of sky

// 	stepper = 50,000 usteps/rev of worm
// 	1 rev of worm  3834.31952  arc sec sky
// 	1 ustep of worm  = .0766863905 arc sec of sky(3834.31952/50000)  = 13.040123456   usteps/arc sec
// 	13.040123456  usteps/arc sec   =  46944.4444 usteps/deg
// 	Dec fact (steps/deg) = 46944.4444


// *)

const bool EastOfMeridian = true;
const bool LookingEast = true;
const bool TargetEastOfMeridian = true;
const bool LastDecNorth = true;
// quadrant = 1 when east of meridian and above the pole
// quadrant = 2 when east of meridian and Below the pole
// quadrant = 3 when west of meridian and above the pole
// quadrant = 4 when west of meridian and Below the pole
//
// when below pole "EastofMeridian" is opposite of above the pole!!!!
//
//Also OMS board uses IRQ 5
const uint16_t Data_reg    = 0x300;
const uint16_t Done_reg    = 0x301;
const uint16_t Control_reg = 0x302;
const uint16_t Status_reg  = 0x303;

const uint8_t Cmderr_bit = 0x01;
const uint8_t Init_Bit   = 0x02;
const uint8_t Enc_bit    = 0x04;
const uint8_t Ovrt_bit   = 0x08;
const uint8_t Done_bit   = 0x10;
const uint8_t Ibf_bit    = 0x20;
const uint8_t Tbe_bit    = 0x40;
const uint8_t Irq_bit    = 0x80;

const uint8_t EastBit   = 0x04;
const uint8_t WestBit   = 0x08;
const uint8_t NorthBit  = 0x01;
const uint8_t SouthBit  = 0x02;
const uint8_t ManualBit = 0x01;

double TrkRate;
std::string xvlslew; //computer slew    
std::string yvlslew; //computer slew    
std::string xvl5inch; //5 inch auto guide 
std::string yvl5inch; //5 inc auto guide
std::string xvl; //tracking adjustment
std::string xac, xacmax, xvlmax; 
std::string yvl; //tracking adjustment
std::string yac, yacmax, yvlmax;
double RAFact;
double DECFACT;
int DecBack; //backlash
double C_Lat;
double C_Long;

uint8_t keystroke;
uint8_t pcxdataout;
uint8_t status;

std::string name;
std::string commandBuffer;

void* pndomem;
uint8_t halfSecondCounter;
int quadrant;
int targetQuadrant;
bool movingRA;
bool movingDEC;
int yPole; // neg when Object Below the Pole
