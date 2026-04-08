#pragma once
#include <windows.h>
#include <string>

// Error codes
constexpr long SUCCESS = 0;
constexpr long INVALID_PORT = 1;
constexpr long INVALID_BAUD_RATE = 2;
constexpr long PORT_NOT_AVAILABLE = 3;
constexpr long COMM_TIMEOUT_ERROR = 4;
constexpr long GET_COMM_STATE_ERROR = 5;
constexpr long SET_COMM_STATE_ERROR = 6;
constexpr long COMM_WRITE_ERROR = 7;
constexpr long COMM_READ_ERROR = 8;
constexpr long SERVO_READ_ERROR = 9;
constexpr long COMM_IO_TIMEOUT = 10;
constexpr long INVALID_AXIS_SELECTION = 11;
constexpr long MOVE_TIME_OUT = 12;
constexpr long INVALID_PARAMETER = 13;
constexpr long INVALID_BIT_NUMBER = 14;

// Axis status
constexpr long AXIS_DIRECTION = 0x01;
constexpr long AXIS_DONE = 0x02;
constexpr long AXIS_OVERTRAVEL = 0x04;
constexpr long AXIS_HOME_SWITCH = 0x08;

// Encoder status
constexpr long SLIP_DETECT_ENABLED = 0x01;
constexpr long POSITION_MAINTENANCE_ENABLED = 0x02;
constexpr long AXIS_SLIPPED = 0x04;
constexpr long AXIS_WITHIN_DEADBAND = 0x08;
constexpr long ENCODER_AT_HOME = 0x10;

// Function arguments
constexpr long POSITIVE = 0;
constexpr long NEGATIVE = 1;
constexpr long MODE_OFF = 0;
constexpr long MODE_ON = 1;
constexpr long LOW = 0;
constexpr long HIGH = 1;

// Axis selection codes
constexpr long OMS_X_AXIS = 0x01;
constexpr long OMS_Y_AXIS = 0x02;
constexpr long OMS_Z_AXIS = 0x04;
constexpr long OMS_T_AXIS = 0x08;
constexpr long OMS_U_AXIS = 0x10;
constexpr long OMS_V_AXIS = 0x20;
constexpr long OMS_R_AXIS = 0x40;
constexpr long OMS_S_AXIS = 0x80;
constexpr long OMS_ALL_4 = 0x0;
constexpr long OMS_ALL_6 = 0x3;
constexpr long OMS_ALL_8 = 0xFF;
constexpr long OMS_ALL_AXES = 0xFF;

// I/O bit selection codes
constexpr long BIT0 = 0x01;
constexpr long BIT1 = 0x02;
constexpr long BIT2 = 0x04;
constexpr long BIT3 = 0x08;
constexpr long BIT4 = 0x10;
constexpr long BIT5 = 0x20;
constexpr long BIT6 = 0x40;
constexpr long BIT7 = 0x80;
constexpr long BIT8 = 0x100;
constexpr long BIT9 = 0x200;
constexpr long BIT10 = 0x400;
constexpr long BIT11 = 0x800;
constexpr long BIT12 = 0x1000;
constexpr long BIT13 = 0x2000;

// Absolute position limits for controller
constexpr long MIN_POSITION = -33554431;
constexpr long MAX_POSITION = 33554431;
constexpr long MIN_VELOCITY = 1;
constexpr long MAX_VELOCITY = 1044000;
constexpr long MIN_ACCELERATION = 1;
constexpr long MAX_ACCELERATION = 8000000;

// Data structures
struct AXES_DATA {
    long X;
    long Y;
    long Z;
    long T;
    long U;
    long V;
    long R;
    long S;
};

struct SCOMM_STRUCT {
    HANDLE hComm;
    int CommPortNumber;
    int BaudRate;
    int AxisDoneFlags;
    bool GlobalDone;
    bool Overtravel;
    bool CmdError;
    bool Slip;
    int Mode;
    int TimeLimit;
    int LF_Count;
    int Timer;
};

struct SCOMM_STRUCT;

typedef long(__stdcall* InitOmsCommPort_t)(SCOMM_STRUCT*);
typedef long(__stdcall* SendString_t)(SCOMM_STRUCT*, char*);
typedef long(__stdcall* SendAndGetString_t)(SCOMM_STRUCT*, char*, char*);

extern InitOmsCommPort_t pInitOmsCommPort;
extern SendString_t pSendString;
extern SendAndGetString_t pSendAndGetString;

extern SCOMM_STRUCT CommRecord;
extern HMODULE DLLHandle;

//load/unload dll functions included
bool LoadDLL(const std::string& dllPath);
void UnloadDLL();

// ---------------------------------------------------------------------------
// DLL function declarations
extern "C" {
    // Comm port functions
    __declspec(dllimport) long __stdcall InitOmsCommPort(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall CloseOmsCommHandle(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall FlushOmsCommBuff(SCOMM_STRUCT* comm);

    // Controller event functions
    __declspec(dllimport) long __stdcall CmdErrEvent(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall GlobalDoneEvent(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall OvertravelEvent(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall SlipEvent(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall ClrAxisDoneFlags(SCOMM_STRUCT* comm, long axis);
    __declspec(dllimport) long __stdcall GetAllAxisDoneFlags(SCOMM_STRUCT* comm, long* flags);

    // Reset and query
    __declspec(dllimport) long __stdcall ResetOmsController(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall SendAndGetString(SCOMM_STRUCT* comm, char* cmd, char* response);
    __declspec(dllimport) long __stdcall SendOmsQueryCommand(SCOMM_STRUCT* comm, char* cmd, char* response);
    __declspec(dllimport) long __stdcall SendString(SCOMM_STRUCT* comm, char* cmd);
    __declspec(dllimport) long __stdcall SendOmsTextCommand(SCOMM_STRUCT* comm, char* cmd);

    // Axis and encoder queries
    __declspec(dllimport) long __stdcall GetAxisSlipFlags(SCOMM_STRUCT* comm, long* flags);
    __declspec(dllimport) long __stdcall GetOmsAxisAcceleration(SCOMM_STRUCT* comm, long axis, long* accel);
    __declspec(dllimport) long __stdcall GetOmsAxisDoneFlag(SCOMM_STRUCT* comm, long axis, bool* done);
    __declspec(dllimport) long __stdcall GetOmsAxisEncoderPosition(SCOMM_STRUCT* comm, long axis, long* pos);
    __declspec(dllimport) long __stdcall GetOmsAxisFlags(SCOMM_STRUCT* comm, long axis, long* flags);
    __declspec(dllimport) long __stdcall GetOmsAxisMotorPosition(SCOMM_STRUCT* comm, long axis, long* pos);
    __declspec(dllimport) long __stdcall GetOmsAxisVelocity(SCOMM_STRUCT* comm, long axis, long* vel);
    __declspec(dllimport) long __stdcall GetOmsControllerDescription(SCOMM_STRUCT* comm, char* desc);
    __declspec(dllimport) long __stdcall GetOmsEncoderFlags(SCOMM_STRUCT* comm, long axis, long* flags);
    __declspec(dllimport) long __stdcall GetSelectedOmsAccelerations(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall GetSelectedOmsMotorPositions(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall GetSelectedOmsVelocities(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);

    // Acceleration ramps
    __declspec(dllimport) long __stdcall SelectOmsCosineRamp(SCOMM_STRUCT* comm, long axis);
    __declspec(dllimport) long __stdcall SelectOmsLinearRamp(SCOMM_STRUCT* comm, long axis);
    __declspec(dllimport) long __stdcall SelectOmsParabolicRamp(SCOMM_STRUCT* comm, long axis, long param);

    // Set velocity & acceleration
    __declspec(dllimport) long __stdcall SetOmsAxisAcceleration(SCOMM_STRUCT* comm, long axis, long accel);
    __declspec(dllimport) long __stdcall SetOmsAxisBaseVelocity(SCOMM_STRUCT* comm, long axis, long vel);
    __declspec(dllimport) long __stdcall SetOmsAxisPosition(SCOMM_STRUCT* comm, long axis, long pos);
    __declspec(dllimport) long __stdcall SetOmsAxisVelocity(SCOMM_STRUCT* comm, long axis, long vel);

    // Stop motion
    __declspec(dllimport) long __stdcall KillAllOmsMotion(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall StopAllOmsAxes(SCOMM_STRUCT* comm);
    __declspec(dllimport) long __stdcall StopOmsAxis(SCOMM_STRUCT* comm, long axis);

    // Axis movement
    __declspec(dllimport) long __stdcall MoveOmsAxisAbs(SCOMM_STRUCT* comm, long axis, long pos);
    __declspec(dllimport) long __stdcall MoveOmsAxisAbsWait(SCOMM_STRUCT* comm, long axis, long pos, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsAxisFractional(SCOMM_STRUCT* comm, long axis, double fraction);
    __declspec(dllimport) long __stdcall MoveOmsAxisIndefinite(SCOMM_STRUCT* comm, long axis, long pos);
    __declspec(dllimport) long __stdcall MoveOmsAxisOneStep(SCOMM_STRUCT* comm, long axis, long step);
    __declspec(dllimport) long __stdcall MoveOmsAxisRel(SCOMM_STRUCT* comm, long axis, long delta);
    __declspec(dllimport) long __stdcall MoveOmsAxisRelWait(SCOMM_STRUCT* comm, long axis, long delta, long timeout);

    // Multi-axis independent moves
    __declspec(dllimport) long __stdcall MoveOmsIndependentAbs(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsIndependentAbsWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsIndependentRel(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsIndependentRelWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsIndependentAbsMt(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsIndependentAbsMtWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsIndependentRelMt(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsIndependentRelMtWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);

    // Multi-axis linear moves 
    __declspec(dllimport) long __stdcall MoveOmsLinearAbs(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsLinearAbsWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsLinearRel(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsLinearRelWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsLinearAbsMt(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsLinearAbsMtWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);
    __declspec(dllimport) long __stdcall MoveOmsLinearRelMt(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data);
    __declspec(dllimport) long __stdcall MoveOmsLinearRelMtWait(SCOMM_STRUCT* comm, long axisMask, AXES_DATA* data, long timeout);

    // Homing
    __declspec(dllimport) long __stdcall HomeOmsAxisFwdUseEncoder(SCOMM_STRUCT* comm, long axis, long vel);
    __declspec(dllimport) long __stdcall HomeOmsAxisFwdUseEncoderWait(SCOMM_STRUCT* comm, long axis, long vel, long timeout);
    __declspec(dllimport) long __stdcall HomeOmsAxisRevUseEncoder(SCOMM_STRUCT* comm, long axis, long vel);
    __declspec(dllimport) long __stdcall HomeOmsAxisRevUseEncoderWait(SCOMM_STRUCT* comm, long axis, long vel, long timeout);
    __declspec(dllimport) long __stdcall HomeOmsAxisFwdUseSwitch(SCOMM_STRUCT* comm, long axis, long vel);
    __declspec(dllimport) long __stdcall HomeOmsAxisFwdUseSwitchWait(SCOMM_STRUCT* comm, long axis, long vel, long timeout);
    __declspec(dllimport) long __stdcall HomeOmsAxisRevUseSwitch(SCOMM_STRUCT* comm, long axis, long vel);
    __declspec(dllimport) long __stdcall HomeOmsAxisRevUseSwitchWait(SCOMM_STRUCT* comm, long axis, long vel, long timeout);
    __declspec(dllimport) long __stdcall DefineOmsHomeAsSwitchClosed(SCOMM_STRUCT* comm, long axis);
    __declspec(dllimport) long __stdcall DefineOmsHomeAsSwitchOpen(SCOMM_STRUCT* comm, long axis);

    // Encoder functions
    __declspec(dllimport) long __stdcall EnableOmsSlipDetection(SCOMM_STRUCT* comm, long axis);
    __declspec(dllimport) long __stdcall SetOmsEncoderRatio(SCOMM_STRUCT* comm, long axis, long numerator, long denominator);
    __declspec(dllimport) long __stdcall SetOmsEncoderSlipTolerance(SCOMM_STRUCT* comm, long axis, long tolerance);
    __declspec(dllimport) long __stdcall SetOmsEncoderHoldMode(SCOMM_STRUCT* comm, long axis, long mode);
    __declspec(dllimport) long __stdcall SetOmsHoldDeadBand(SCOMM_STRUCT* comm, long axis, long band);
    __declspec(dllimport) long __stdcall SetOmsHoldGain(SCOMM_STRUCT* comm, long axis, long gain);
    __declspec(dllimport) long __stdcall SetOmsHoldVelocity(SCOMM_STRUCT* comm, long axis, long vel);

    // Overtravel
    __declspec(dllimport) long __stdcall SetOmsAxisOvertravelDetect(SCOMM_STRUCT* comm, long axis, long mode);
    __declspec(dllimport) long __stdcall SetOmsSoftLimitsMode(SCOMM_STRUCT* comm, long axis, long mode);

    // I/O bits
    __declspec(dllimport) long __stdcall GetAllOmsIOBits(SCOMM_STRUCT* comm, long* bits);
    __declspec(dllimport) long __stdcall GetOmsIOBit(SCOMM_STRUCT* comm, long bit, long* value);
    __declspec(dllimport) long __stdcall GetOmsIOBitConfig(SCOMM_STRUCT* comm, long* config);
    __declspec(dllimport) long __stdcall SetOmsIOBit(SCOMM_STRUCT* comm, long bit, long value);

    // Extended I/O
    __declspec(dllimport) long __stdcall GetOmsXIOBit(SCOMM_STRUCT* comm, long bit, long* value);
    __declspec(dllimport) long __stdcall GetOmsXIOBitConfig(SCOMM_STRUCT* comm, long bit, long* config);
    __declspec(dllimport) long __stdcall SetOmsXIOBit(SCOMM_STRUCT* comm, long bit, long value);
    __declspec(dllimport) long __stdcall ReadOmsXIOPort(SCOMM_STRUCT* comm, long port, long* value);
    __declspec(dllimport) long __stdcall GetOmsXIOPortConfig(SCOMM_STRUCT* comm, long port, long* config);
    __declspec(dllimport) long __stdcall WriteOmsXIOPort(SCOMM_STRUCT* comm, long port, long value);

    // Auxiliary outputs
    __declspec(dllimport) long __stdcall EnableOmsAxisAuxOutAutoMode(SCOMM_STRUCT* comm, long axis, long mode);
    __declspec(dllimport) long __stdcall SetOmsAxisAuxOutBit(SCOMM_STRUCT* comm, long axis, long bit);
    __declspec(dllimport) long __stdcall SetOmsAxisAuxOutSettleTime(SCOMM_STRUCT* comm, long axis, long time);
    __declspec(dllimport) long __stdcall SetSelectedOmsAuxOutBits(SCOMM_STRUCT* comm, long axisMask, long bits);

    // Wait
    __declspec(dllimport) long __stdcall OmsWait(long ms);
}
