# GVO Telescope Control Documentation

This document describes the internal design of the GVO Telescope Control C++ project.

The project is a Windows-only, 32-bit C++ telescope controller that communicates with an OMS motion controller through `68SERMC.DLL`, communicates with TheSky through `teleapi.dll`, and uses a timer-driven loop to handle tracking, GOTO slews, syncing, parking, coordinate updates, and hand paddle input.

---

## 1. Runtime Requirements

The runtime setup folder should contain:

```text
GVO_Telescope_Control.exe
teleapi.dll
68SERMC.DLL
config.ini
```

or, if the executable has been renamed:

```text
Main.exe
teleapi.dll
68SERMC.DLL
config.ini
```

The program is intended for:

- Windows.
- 32-bit executable builds.
- MinGW g++.
- A Windows version that supports 32-bit applications.
- OMS controller communication through a serial COM port.

It is not currently configured for macOS or Linux.

---

## 2. Build Commands

### Static build

```bat
"C:\Program Files\GCC\winlibs-i686-posix-dwarf-gcc-15.2.0-mingw-w64ucrt-13.0.0-r4\mingw32\bin\g++.exe" -m32 -Wall -static -static-libgcc -static-libstdc++ Main.cpp OMS68SERMC.cpp CommUtils.cpp Utils.cpp Sync.cpp Slew.cpp quad1to1.cpp quad1to2.cpp quad1to3.cpp quad2to1.cpp quad2to2.cpp quad3to1.cpp quad3to3.cpp GetQandYU.cpp CalcCoord.cpp TimerUnit.cpp HandPaddle.cpp ManualPaddle.cpp Config.cpp -o GVO_Telescope_Control_Strict.exe
```

### Less aggressive build

```bat
"C:\Program Files\GCC\winlibs-i686-posix-dwarf-gcc-15.2.0-mingw-w64ucrt-13.0.0-r4\mingw32\bin\g++.exe" -m32 -Wall -static-libgcc -static-libstdc++ Main.cpp OMS68SERMC.cpp CommUtils.cpp Utils.cpp Sync.cpp Slew.cpp quad1to1.cpp quad1to2.cpp quad1to3.cpp quad2to1.cpp quad2to2.cpp quad3to1.cpp quad3to3.cpp GetQandYU.cpp CalcCoord.cpp TimerUnit.cpp HandPaddle.cpp ManualPaddle.cpp Config.cpp -o GVO_Telescope_Control.exe
```

There is currently no `tasks.json`. The project is built directly with the command line.

Note: "C:\Program Files\GCC\winlibs-i686-posix-dwarf-gcc-15.2.0-mingw-w64ucrt-13.0.0-r4\mingw32\bin\g++.exe" should link to wherever mingw is installed in the environment. This path likely will not work as is. 

---

## 3. Source Files

The current build command includes:

```text
Main.cpp
OMS68SERMC.cpp
CommUtils.cpp
Utils.cpp
Sync.cpp
Slew.cpp
quad1to1.cpp
quad1to2.cpp
quad1to3.cpp
quad2to1.cpp
quad2to2.cpp
quad3to1.cpp
quad3to3.cpp
GetQandYU.cpp
CalcCoord.cpp
TimerUnit.cpp
HandPaddle.cpp
ManualPaddle.cpp
Config.cpp
```

Important headers include:

```text
GlobalValues.h
StateVar.h
CommUtils.h
OMS68SERMC.h
Config.h
TimerUnit.h
HandPaddle.h
ManualPaddle.h
Slew.h
Sync.h
GetQandYU.h
CalcCoord.h
Bits.h
Utils.h
```

---

## 4. Program Startup Flow

The program startup flow is:

```text
main()
│
├── loadparams()
│   └── Loads built-in defaults.
│
├── SetStime()
│   └── Sets sidereal time.
│
├── LoadConfig("config.ini")
│   └── Reads config values.
│
├── ApplyConfig()
│   └── Applies config values to globals.
│
├── PrintConfig()
│   └── Prints active configuration.
│
├── CommUtils::InitSharedMem(TeleApiDllPath)
│   └── Initializes TheSky shared memory through teleapi.dll.
│
├── LoadDLL(OmsDllPath)
│   └── Loads 68SERMC.DLL and resolves OMS function pointers.
│
├── CommRecord.CommPortNumber = ComPortNumber
│
├── InitComm()
│   └── Opens the OMS serial connection.
│
├── SendCommand("RS;")
│   └── Resets controller.
│
├── SendCommand("EF; IO0,0; AX; DBI; LP0; SC; AY; DBI; LP0; SC;")
│   └── Configures I/O and axes.
│
├── SendCommand("AX JF<TrkRate>;")
│   └── Starts RA tracking.
│
└── ManualControlMenu()
	└── Starts user menu.
```

After the menu exits, the program unloads the OMS DLL.

---

## 5. Global State

Most runtime state is stored in global variables declared in `GlobalValues.h`.

### Communication and paths

| Variable | Purpose |
|---|---|
| `ComPortNumber` | Serial COM port number. |
| `OmsDllPath` | Runtime path to `68SERMC.DLL`. |
| `TeleApiDllPath` | Runtime path to `teleapi.dll`. |
| `CommRecord` | OMS communication record. |
| `Response[256]` | Shared response buffer for controller queries. |

### Axis state

| Variable | Purpose |
|---|---|
| `Xstate` | RA/X state machine value. |
| `Ystate` | DEC/Y state machine value. |
| `movingRA` | True while an RA/X commanded move is active. |
| `movingDEC` | True while a DEC/Y commanded move is active. |
| `quadrant` | Current telescope quadrant. |
| `targetQuadrant` | Target quadrant for GOTO logic. |
| `yPole` | Current Y-axis polarity. |
| `LastDecNorth` | Last DEC direction used by bump/correction logic. |

### Coordinate and astronomy values

| Variable | Purpose |
|---|---|
| `TrkRate` | RA tracking rate. |
| `RAFact` | RA conversion factor. |
| `DECFACT` | DEC conversion factor. |
| `C_Lat` | Observatory latitude. |
| `C_Long` | Observatory longitude. |
| `SidTime` | Sidereal time value. |
| `RANow` | Current RA. |
| `DECNow` | Current DEC. |
| `RaTarget` | Target RA. |
| `DecTarget` | Target DEC. |
| `Meridian` | Current meridian value. |
| `EastHor` | East horizon value. |
| `WestHor` | West horizon value. |
| `EastHA`, `WestHA`, `NorthHA`, `SouthHA` | Hour-angle boundary values. |
| `RaPos` | Current/calculated RA motor position. |
| `decPos` | Current/calculated DEC motor position. |

### Paddle state

| Variable | Purpose |
|---|---|
| `EastPushed` | East button state. |
| `WestPushed` | West button state. |
| `NorthPushed` | North button state. |
| `SouthPushed` | South button state. |
| `ManualPushed` | Manual/Fast button state. |
| `HandPadRaEastRate` | Normal east correction speed. |
| `HandPadRaWestRate` | Normal west correction speed. |
| `HandPadDecRate` | Normal DEC correction speed. |
| `FastPadXSlew` | Fast paddle X/RA slew speed. |
| `FastPadYSlew` | Fast paddle Y/DEC slew speed. |
| `FastPadXFine` | Fast paddle X/RA fine speed. |
| `FastPadYFine` | Fast paddle Y/DEC fine speed. |

### Parking and safety flags

| Variable | Purpose |
|---|---|
| `Parkit` | Set true to request parking. |
| `parkInProgress` | True while park flow is active. |
| `parkCompleted` | True when park completes. |
| `lowAltitudeLockout` | Stops the timer loop if a low-altitude safety condition occurs. |
| `TimerEnabled` | Enables or disables `TimerUpdate()` calls. |

---

## 6. Axis State Machine

`StateVar.h` defines:

```cpp
enum class StateVar : int {
	Off = 0,
	Tracking,
	CorrectingE,
	CorrectingW,
	CorrectingN,
	CorrectingS,
	Slewing
};
```

### Common X/RA states

| State | Meaning |
|---|---|
| `Off` | Axis is inactive or not initialized. |
| `Tracking` | RA tracking is active. |
| `CorrectingE` | East correction is active. |
| `CorrectingW` | West correction is active. |
| `Slewing` | Manual or commanded slew is active. |

### Common Y/DEC states

| State | Meaning |
|---|---|
| `Off` | Axis is inactive or not initialized. |
| `Tracking` | DEC axis is idle while RA tracks. |
| `CorrectingN` | North correction is active. |
| `CorrectingS` | South correction is active. |
| `Slewing` | Manual or commanded slew is active. |

---

## 7. Configuration System

### Files

```text
Config.cpp
Config.h
config.ini
```

### Loading sequence

```cpp
loadparams();
LoadConfig("config.ini");
ApplyConfig();
PrintConfig();
```

`loadparams()` sets defaults. `LoadConfig()` reads key/value pairs. `ApplyConfig()` overrides matching globals.

### Supported parsing behavior

`LoadConfig()` supports:

- Empty lines.
- Comment lines beginning with `#`.
- Inline comments after `#`.
- Whitespace trimming around keys and values.
- `key=value` format.

### Supported config keys in the provided `ApplyConfig()`

| Key | Applied to |
|---|---|
| `TrkRate` | `TrkRate` |
| `RAFact` | `RAFact` |
| `C_Lat` | `C_Lat` |
| `C_Long` | `C_Long` |
| `handpad_ra_east_rate` | `HandPadRaEastRate` |
| `handpad_ra_west_rate` | `HandPadRaWestRate` |
| `handpad_dec_rate` | `HandPadDecRate` |
| `dec_bump_rate` | `DecBumpRate` |
| `dec_bump_ms` | `DecBumpDurationMs` |
| `fastpad_x_fine` | `FastPadXFine` |
| `fastpad_y_fine` | `FastPadYFine` |
| `xvl` | `xvl` |
| `xac` | `xac` |
| `yvl` | `yvl` |
| `yac` | `yac` |
| `xacmax` | `xacmax` |
| `xvlmax` | `xvlmax` |
| `yacmax` | `yacmax` |
| `yvlmax` | `yvlmax` |
| `com_port` | `ComPortNumber` |
| `oms_dll` | `OmsDllPath` |
| `teleapi_dll` | `TeleApiDllPath` |

### Config issue to verify

`config.ini` includes:

```ini
xvlslew=75000
yvlslew=50000
```

`GlobalValues.h` also declares GOTO/slew-related strings, including:

```cpp
extern std::string GotoXSlew;
extern std::string GotoYSlew;
extern std::string xvlslew, yvlslew;
```

However, the provided `ApplyConfig()` code does not currently apply `xvlslew`, `yvlslew`, `GotoXSlew`, or `GotoYSlew`.

If the quadrant routines use those values, add the missing config overrides:

```cpp
if (config.count("xvlslew"))
	xvlslew = config["xvlslew"];

if (config.count("yvlslew"))
	yvlslew = config["yvlslew"];

if (config.count("goto_x_slew"))
	GotoXSlew = config["goto_x_slew"];

if (config.count("goto_y_slew"))
	GotoYSlew = config["goto_y_slew"];
```

Then add matching entries to `config.ini` if using `GotoXSlew` and `GotoYSlew`.

---

## 8. OMS Controller Interface

### Files

```text
OMS68SERMC.h
OMS68SERMC.cpp
CommUtils.h
CommUtils.cpp
```

### `SCOMM_STRUCT`

The OMS communication record contains:

```cpp
struct SCOMM_STRUCT {
	HANDLE hComm;
	int CommPortNumber;
	int BaudRate;
	int AxisDoneFlags;
	int GlobalDone;
	int Overtravel;
	int CmdError;
	int Slip;
	int Mode;
	int TimeLimit;
	int LF_Count;
	int Timer;
};
```

### Required dynamically loaded functions

The program dynamically resolves:

```cpp
InitOmsCommPort
SendString
SendAndGetString
```

These are stored in function pointers:

```cpp
extern InitOmsCommPort_t pInitOmsCommPort;
extern SendString_t pSendString;
extern SendAndGetString_t pSendAndGetString;
```

### `LoadDLL(const std::string& dllPath)`

Loads `68SERMC.DLL` and resolves the required OMS communication functions.

### `UnloadDLL()`

Frees the DLL and clears the loaded handles/function pointers.

### `InitComm()`

Initializes the controller communication using `CommRecord`.

### `SendCommand(const std::string& cmd)`

Sends a command string to the controller.

### `SendAndGetCommand(...)`

Sends a command string to the controller and reads a response.

---

## 9. TheSky Shared Memory Interface

### File

```text
CommUtils.h
```

### Coordinate structure

```cpp
struct Coord {
	double RA;
	double Dec;
	double RAGoto;
	double DecGoto;
	double RASync;
	double DecSync;
};
```

### Namespace functions

```cpp
namespace CommUtils {
	extern void* pndomem;

	bool InitSharedMem(const std::string& dllPath);
	void ShutdownSharedMem();
	Coord* GetCoordPtr();
}
```

### Meaning of fields

| Field | Meaning |
|---|---|
| `RA` | Current telescope RA. |
| `Dec` | Current telescope DEC. |
| `RAGoto` | Requested GOTO RA from TheSky. |
| `DecGoto` | Requested GOTO DEC from TheSky. |
| `RASync` | Requested sync RA from TheSky. |
| `DecSync` | Requested sync DEC from TheSky. |

`TimerUpdate()` checks these values to decide whether to call `SlewScope()` or `SyncScope()`.

---

## 10. Main Timer Loop

### Files

```text
TimerUnit.cpp
TimerUnit.h
```

### `RunTimerLoopUntilIdleOrEscape()`

Runs the timer loop until:

- Low altitude lockout occurs.
- Parking completes.
- The user presses `ESC`.

The loop calls `TimerUpdate()` about every 100 ms while `TimerEnabled` is true.

### `TimerUpdate()`

`TimerUpdate()` is the central runtime function.

It performs these tasks in priority order:

1. Prevents re-entrant timer execution.
2. Gets the TheSky coordinate memory pointer.
3. Polls RA move completion with `AX QA;`.
4. Polls DEC move completion with `AY QA;`.
5. Periodically calls `UpdateCoord()`.
6. Returns early if a commanded RA/DEC move is active.
7. Handles sync requests.
8. Handles GOTO requests.
9. Handles park requests.
10. Completes park flow.
11. Calls `HandleHandPaddle()`.

### Re-entry guard

The function uses a static guard:

```cpp
static bool inTimerUpdate = false;
```

This prevents overlapping timer updates.

### Move-complete polling

RA query:

```text
AX QA;
```

DEC query:

```text
AY QA;
```

When the response indicates done, movement flags are cleared and request fields are reset.

### RA tracking restart

After RA movement completes, the program restarts tracking:

```text
AX JF<TrkRate>;
```

---

## 11. Parking Flow

### `Park()`

```cpp
void Park() {
	Parkit = true;
	parkInProgress = true;
	parkCompleted = false;
}
```

### Timer park command

When `TimerUpdate()` sees `Parkit == true`, it sends:

```text
AX KL;
AA VL75000,50000; MA0,0,,; GD; ID;
```

This sends the mount toward controller position `0,0`.

### Park completion

After the park move is complete, the program restarts RA tracking:

```text
AX JF<TrkRate>;
```

Then it sets:

```cpp
Xstate = StateVar::Tracking;
Ystate = StateVar::Tracking;
parkInProgress = false;
parkCompleted = true;
```

### Important warning

The park command assumes controller position `0,0` is the correct physical park position. This must be verified against the real telescope mount.

---

## 12. Hand Paddle Input

### File

```text
HandPaddle.cpp
```

### Query command

The hand paddle reads controller input using:

```text
BX;
```

The response is expected to be 2 characters long.

### Bit decoding

The code decodes the first response character for:

- `ManualBit`
- `SouthBit`

It decodes the second response character for:

- `NorthBit`
- `EastBit`
- `WestBit`

The response characters are interpreted as hexadecimal-like nibbles:

```cpp
ioLocal = std::toupper(static_cast<unsigned char>(TempStr[0]));
ioLocal = (ioLocal > '9') ? ioLocal - 0x37 : ioLocal - 0x30;
```

### Normal RA hand paddle behavior

When `Xstate == Tracking`:

| Button | New state | Command behavior |
|---|---|---|
| East | `CorrectingE` | Jog RA using `TrkRate - HandPadRaEastRate`. |
| West | `CorrectingW` | Jog RA using `TrkRate + HandPadRaWestRate`. |

When East/West are released:

1. Send `AX ST;`.
2. Send `AX ST;` again.
3. Restart tracking with `AX JF<TrkRate>;`.
4. Return `Xstate` to `Tracking`.

If East and West are pressed together during correction, the code enters `Slewing` and jogs with `xvl`.

### Normal DEC hand paddle behavior

When `Ystate == Tracking`:

| Button | New state | Command behavior |
|---|---|---|
| North | `CorrectingN` | Optional bump logic, then `AY JG<HandPadDecRate>;`. |
| South | `CorrectingS` | Optional bump logic, then `AY JG-<HandPadDecRate>;`. |

When North/South are released:

```text
AY ST;
```

Then `Ystate` returns to `Tracking`.

---

## 13. Manual / Fast Paddle

### File

```text
ManualPaddle.cpp
```

If the hand paddle manual/fast button is active, `HandleHandPaddle()` calls:

```cpp
HandleFastPaddle();
```

### Speed selection

The fast paddle uses two speed groups:

| Mode | X/RA speed | Y/DEC speed |
|---|---|---|
| Slew | `FastPadXSlew` | `FastPadYSlew` |
| Fine | `FastPadXFine` | `FastPadYFine` |

The selected mode depends on `SlewSelect`.

### Fast RA movement

| Button | New state | Command |
|---|---|---|
| East | `CorrectingE` | `AX ST; AX JG-<xspeed>;` |
| West | `CorrectingW` | `AX ST; AX JG<xspeed>;` |

When released:

```text
AX ST;
AX JF<TrkRate>;
```

### Fast DEC movement

| Button | New state | Command |
|---|---|---|
| North | `CorrectingN` | Optional bump, then `AY JG<yspeed>;` |
| South | `CorrectingS` | Optional bump, then `AY JG-<yspeed>;` |

When released:

```text
AY ST;
```

---

## 14. DEC Bump Logic

### Functions

```cpp
BumpNorth()
BumpSouth()
```

### Purpose

The bump functions send a short DEC jog and stop command before some DEC movements. This appears intended to handle direction reversal, backlash, or mechanical settling.

### Config values

```ini
dec_bump_rate=75
dec_bump_ms=100
```

### Behavior

`BumpNorth()`:

```text
AY JG-<DecBumpRate>;
wait <DecBumpDurationMs>;
AY ST;
```

`BumpSouth()`:

```text
AY JG<DecBumpRate>;
wait <DecBumpDurationMs>;
AY ST;
```

The functions update `LastDecNorth` so later DEC commands know which direction was last used.

---

## 15. GOTO Slew Logic

### File

```text
Slew.cpp
```

### Entry point

```cpp
void SlewScope(double raTarget, double decTarget)
```

### Calculation step

`SlewScope()` calls:

```cpp
GetQandY(
	raTarget,
	decTarget,
	Alt,
	HA,
	Xcount,
	Ycount,
	localTargetQuadrant,
	localTargetYPole
);
```

This calculates:

- Target altitude.
- Target hour angle.
- Target X/RA motor count.
- Target Y/DEC motor count.
- Target quadrant.
- Target Y polarity.

### Safety check

If the target altitude is below 20 degrees or the target Y polarity is invalid, the slew is rejected:

```cpp
if (Alt < 20.0 || localTargetYPole == 0)
```

The request fields are cleared so the same invalid GOTO request does not repeat forever.

### Quadrant routing

The provided code routes these cases:

| Current quadrant | Target quadrant | Function |
|---:|---:|---|
| 1 | 1 | `GoQuad1to1()` |
| 1 | 3 | `GoQuad1to3()` |
| 3 | 3 | `GoQuad1to3()` in the provided code |
| 3 | 1 | `GoQuad3to1()` |

If no matching route is found, the GOTO request is cleared.

### Note

The build command also includes additional quadrant files:

```text
quad1to2.cpp
quad2to1.cpp
quad2to2.cpp
```

If those routes are implemented but not currently used by `SlewScope()`, add routing conditions for quadrants 2 and 4 as needed.

---

## 16. Sync Logic

### File

```text
Sync.cpp
```

### Entry point

```cpp
void SyncScope()
```

### Behavior

`SyncScope()`:

1. Returns immediately if RA or DEC is moving.
2. Gets the shared coordinate pointer.
3. Uses `GetQandY()` to convert sync RA/DEC to motor counts.
4. Rejects sync if altitude is below the horizon.
5. Loads RA controller position with `AX LP...`.
6. Restarts RA tracking with `AX JF<TrkRate>;`.
7. Loads DEC controller position with `AY LP...`.
8. Clears `RASync` and `DecSync`.
9. Calls `UpdateCoord()`.

### RA position load examples

Positive:

```text
AX LP12345;
```

Negative:

```text
AX LP-12345;
```

### DEC position load examples

Positive:

```text
AY LP12345;
```

Negative:

```text
AY LP-12345;
```

---

## 17. Coordinate Conversion

### File

```text
GetQandYU.cpp
```

### Entry point

```cpp
void GetQandY(
	double ra,
	double dec,
	double& Alt,
	double& HA,
	double& Xcount,
	double& Ycount,
	int& q,
	int& y
)
```

### Inputs

| Input | Unit |
|---|---|
| `ra` | Hours |
| `dec` | Degrees |

### Outputs

| Output | Meaning |
|---|---|
| `Alt` | Altitude in degrees. |
| `HA` | Hour angle in hours. |
| `Xcount` | X/RA motor count. |
| `Ycount` | Y/DEC motor count. |
| `q` | Target/current quadrant. |
| `y` | Y-axis polarity. |

### Meridian and horizon values

```cpp
Meridian = GetStime();
EastHor = Meridian + 6.0;
WestHor = Meridian - 6.0;
```

### Altitude calculation

Hour angle is converted to degrees:

```cpp
double HAdeg = 15.0 * (Meridian - ra);
```

Altitude is calculated from declination, latitude, and hour angle:

```cpp
Alt = asin(
	(sin(dec) * sin(latitude)) +
	(cos(dec) * cos(HAdeg) * cos(latitude))
);
```

Then converted to degrees.

### Hour angle normalization

```cpp
HA = Meridian - ra;

if (HA < -12.0) HA += 24.0;
if (HA > 12.0) HA -= 24.0;
```

### Count conversion

Y/DEC:

```cpp
if (dec >= C_Lat)
	Ycount = (dec - C_Lat) * DECFACT;
else
	Ycount = -1.0 * ((C_Lat - dec) * DECFACT);
```

X/RA:

```cpp
Xcount = (HA * 15.0) * RAFact;
```

---

## 18. Controller Command Summary

The project sends these OMS-style commands directly.

| Command | Use in this project |
|---|---|
| `RS;` | Reset controller. |
| `EF;` | Enable/configure controller functions. |
| `IO0,0;` | Configure controller I/O. |
| `AX;` | Select X/RA axis. |
| `AY;` | Select Y/DEC axis. |
| `AA;` | Select both axes. |
| `DBI;` | Axis setup command used during init. |
| `LP0;` | Load/set position-related axis value during init. |
| `SC;` | Save/configure selected axis during init. |
| `AX JF<rate>;` | Start/set RA tracking rate. |
| `AX JG<rate>;` | Jog X/RA axis. |
| `AY JG<rate>;` | Jog Y/DEC axis. |
| `AX ST;` | Stop X/RA axis. |
| `AY ST;` | Stop Y/DEC axis. |
| `AA ST;` | Stop all axes. |
| `BX;` | Read hand paddle/controller input bits. |
| `AX QA;` | Query X/RA axis done/status. |
| `AY QA;` | Query Y/DEC axis done/status. |
| `AX LP<count>;` | Load X/RA motor position. |
| `AY LP<count>;` | Load Y/DEC motor position. |
| `AX KL;` | Kill/clear/stop X/RA before park. |
| `AA VL<x>,<y>;` | Set two-axis velocity. |
| `MA0,0,,;` | Move absolute to controller position `0,0`. |
| `GD; ID;` | Begin/execute motion sequence used by park/GOTO logic. |

Exact command meanings should be verified against the OMS controller manual for the specific hardware model.

---

## 19. Safety Checks

The current code includes several software-level checks:

- `SlewScope()` rejects GOTO targets below 20 degrees altitude.
- `SyncScope()` rejects sync targets below the horizon.
- `RunTimerLoopUntilIdleOrEscape()` exits if `lowAltitudeLockout` is set.
- `HandleHandPaddle()` returns immediately if `movingRA` or `movingDEC` is true.
- `TimerUpdate()` avoids re-entrant execution with a static guard.

Hardware-level safety should still be verified separately.

---

## 20. Known Issues and Recommended Improvements

### Verify park/origin alignment

Parking uses:

```text
MA0,0,,;
```

This assumes controller coordinate `0,0` is the correct physical park location.

Add a documented origin-setting procedure once the correct startup/sync/park relationship is confirmed.

### Clarify quadrant coverage

The build includes `quad1to2.cpp`, `quad2to1.cpp`, and `quad2to2.cpp`, but the provided `SlewScope()` route logic only visibly uses 1-to-1, 1-to-3, 3-to-1, and 3-to-3 behavior.

Add or document quadrant 2 and 4 behavior if needed.

### Add a dry-run mode

A dry-run mode would print commands without sending them to the controller. This would make it safer to test configuration, menu behavior, and TheSky communication without moving hardware.

### Add command logging

Logging every command and response would make debugging much easier.

Suggested log format:

```text
logs/session-YYYYMMDD-HHMMSS.txt
```

### Add VS Code build tasks

A `.vscode/tasks.json` file could store the known-good compile command so the project can be built from VS Code without copying the full command manually.

---

## 21. Development Notes

This codebase still reflects its Pascal/Delphi origin:

- Many values are global.
- Axis state is shared across modules.
- The timer loop drives most behavior.
- Controller commands are often sent as raw strings.
- Hardware state, TheSky state, and controller state are tightly coupled.

Future refactoring could group state into:

```cpp
struct ObservatoryConfig;
struct MotionConfig;
struct PaddleConfig;
struct ControllerState;
struct TelescopeState;
```

For now, the global-state approach is acceptable for a direct hardware-control conversion as long as startup, sync, park, and hand paddle behavior are tested carefully.
