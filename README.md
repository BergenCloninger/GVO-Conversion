# GVO Telescope Control

GVO Telescope Control is a Windows C++ console program for controlling a two-axis telescope mount through an OMS motion controller. It is a C++ conversion/modernization of an older Pascal/Delphi telescope-control workflow.

The program loads the OMS `68SERMC.DLL`, initializes the controller over a serial COM port, starts RA tracking, communicates with TheSky through `teleapi.dll` shared memory, and handles telescope parking, syncing, GOTO slews, normal hand paddle corrections, and fast/manual paddle movement.

> **Hardware warning:** This program sends real motion commands to telescope hardware. Verify COM port, controller wiring, axis direction, tracking rate, park position, sync behavior, and all safety limits before running it on the telescope.

---

## Project Status

This project has completed development. The current C++ version supports:

- OMS controller initialization.
- 32-bit Windows DLL loading.
- Serial communication through a configured COM port.
- RA tracking startup.
- TheSky shared-memory coordinate exchange.
- GOTO request handling.
- Sync request handling.
- Parking command flow.
- Normal hand paddle movement.
- Manual/fast paddle movement.
- Configurable speeds and telescope location values through `config.ini`.

Some behavior, especially controller origin, park alignment, TPoint/TheSky offset behavior, and physical axis direction, should still be verified carefully on hardware. Some parts of the project could be improved or reworked.

---

## Platform Compatibility

This project was written and developed with MinGW in C++.

Supported environment:

- Windows only.
- 32-bit build target.
- Requires a Windows version that supports 32-bit programs.
- Not configured for macOS.
- Not configured for Linux.

The program uses Windows-specific APIs and headers, including:

- `windows.h`
- `LoadLibraryA`
- `GetProcAddress`
- `Sleep`
- `_kbhit`
- `_getch`

The project is built as a 32-bit program because the telescope controller DLL workflow uses 32-bit Windows DLL compatibility.

---

## Required Runtime Files

The setup folder should contain:

```text
GVO_Telescope_Control.exe
teleapi.dll
68SERMC.DLL
config.ini
```

If the executable is renamed after compiling, the setup folder may instead contain something like:

```text
Main.exe
teleapi.dll
68SERMC.DLL
config.ini
```

The important requirement is that the compiled executable, `teleapi.dll`, `68SERMC.DLL`, and `config.ini` are all available together at runtime, unless absolute DLL paths are provided in `config.ini`.

---

## Hardware / Device Setup

Before running the program:

1. Connect the telescope controller to the computer.
2. Open Windows Device Manager:
	- Press `Win + X`.
	- Choose **Device Manager**.
	- Expand **Ports (COM & LPT)**.
3. Look for the serial device connected to the OMS controller.
4. Note the COM port number.
5. Update `config.ini`:

```ini
com_port=1
```

For example, if Device Manager shows the controller on `COM3`, use:

```ini
com_port=3
```

---

## Configuration File

The program reads `config.ini` from the same folder where it is run.

Example:

```ini
# ---- Communication ----
com_port=1
oms_dll=68SERMC.DLL

# ---- Paths ----
teleapi_dll=teleapi.dll

# ---- Tracking / motion ----
TrkRate=500.6
RAFact=120000

# ---- Location ----
C_Lat=38.5
C_Long=-106.9

# ---- Normal hand paddle speeds ----
# Used when Manual/Fast button is NOT held
handpad_ra_east_rate=400
handpad_ra_west_rate=750
handpad_dec_rate=250

# ---- Fast paddle ONLY ----
fastpad_x_fine=2000
fastpad_y_fine=2000

# ---- REAL SLEW SPEEDS (used by quad routines) ----
xvlslew=75000
yvlslew=50000

# ---- DEC bump behavior ----
dec_bump_rate=75
dec_bump_ms=100

# ---- Optional axis defaults ----
xvl=10000
xac=35000
yvl=5000
yac=25000
xacmax=35000
xvlmax=75000
yacmax=25000
yvlmax=100000

# ---- Debug ----
verbose=1
```

---

## Important Configuration Values

| Setting | Purpose |
|---|---|
| `com_port` | Serial COM port number from Windows Device Manager. |
| `oms_dll` | Path to `68SERMC.DLL`. Usually `68SERMC.DLL` if it is beside the executable. |
| `teleapi_dll` | Path to `teleapi.dll`. Usually `teleapi.dll` if it is beside the executable. |
| `TrkRate` | RA tracking rate used by `AX JF<rate>;`. |
| `RAFact` | RA conversion factor used to convert hour angle/degrees into controller counts. |
| `C_Lat` | Observatory latitude. |
| `C_Long` | Observatory longitude. |
| `handpad_ra_east_rate` | Normal hand paddle east correction rate. |
| `handpad_ra_west_rate` | Normal hand paddle west correction rate. |
| `handpad_dec_rate` | Normal hand paddle DEC correction rate. |
| `fastpad_x_fine` | Fast/manual paddle RA/X fine speed. |
| `fastpad_y_fine` | Fast/manual paddle DEC/Y fine speed. |
| `xvlslew` | Intended X/RA GOTO slew speed for quadrant routines. |
| `yvlslew` | Intended Y/DEC GOTO slew speed for quadrant routines. |
| `dec_bump_rate` | DEC bump speed used before some DEC direction changes. |
| `dec_bump_ms` | DEC bump duration in milliseconds. |
| `xvl`, `yvl` | General X/Y velocity values. |
| `xac`, `yac` | General X/Y acceleration values. |
| `xvlmax`, `yvlmax` | Maximum X/Y velocity values. |
| `xacmax`, `yacmax` | Maximum X/Y acceleration values. |

---

## Build Instructions

This project does not currently use a `tasks.json` file. It can be built directly with a 32-bit MinGW g++ command.

### Static build

```bat
"C:\Program Files\GCC\winlibs-i686-posix-dwarf-gcc-15.2.0-mingw-w64ucrt-13.0.0-r4\mingw32\bin\g++.exe" -m32 -Wall -static -static-libgcc -static-libstdc++ Main.cpp OMS68SERMC.cpp CommUtils.cpp Utils.cpp Sync.cpp Slew.cpp quad1to1.cpp quad1to2.cpp quad1to3.cpp quad2to1.cpp quad2to2.cpp quad3to1.cpp quad3to3.cpp GetQandYU.cpp CalcCoord.cpp TimerUnit.cpp HandPaddle.cpp ManualPaddle.cpp Config.cpp -o GVO_Telescope_Control_Strict.exe
```

### Less aggressive build

```bat
"C:\Program Files\GCC\winlibs-i686-posix-dwarf-gcc-15.2.0-mingw-w64ucrt-13.0.0-r4\mingw32\bin\g++.exe" -m32 -Wall -static-libgcc -static-libstdc++ Main.cpp OMS68SERMC.cpp CommUtils.cpp Utils.cpp Sync.cpp Slew.cpp quad1to1.cpp quad1to2.cpp quad1to3.cpp quad2to1.cpp quad2to2.cpp quad3to1.cpp quad3to3.cpp GetQandYU.cpp CalcCoord.cpp TimerUnit.cpp HandPaddle.cpp ManualPaddle.cpp Config.cpp -o GVO_Telescope_Control.exe
```

The static build is intended to reduce dependency issues by statically linking GCC runtime libraries. The less aggressive build still links the standard GCC runtime libraries statically but does not use full `-static`.

---

## Running the Program

After compiling, place the executable in the setup folder with:

```text
teleapi.dll
68SERMC.DLL
config.ini
```

Then run:

```bat
GVO_Telescope_Control.exe
```

or:

```bat
GVO_Telescope_Control_Strict.exe
```

depending on which build command was used.

On startup, the program:

1. Loads default telescope and controller parameters.
2. Sets sidereal time.
3. Loads `config.ini`.
4. Applies configuration overrides.
5. Prints the active configuration.
6. Initializes TheSky shared memory through `teleapi.dll`.
7. Loads `68SERMC.DLL`.
8. Sets the configured COM port.
9. Initializes communication with the OMS controller.
10. Resets the controller.
11. Configures controller I/O and axes.
12. Starts RA tracking.
13. Opens the console menu.

---

## Console Menu

```text
=== Telescope Control Menu ===
1. Park Telescope
2. Connect to TheSky
0. Exit
```

### 1. Park Telescope

Starts the park request flow. The timer loop detects the park flag and sends the controller command sequence for moving toward controller position `0,0`.

### 2. Connect to TheSky

Runs the timer loop so the program can respond to TheSky shared-memory requests. While this loop is running, the program can process:

- GOTO requests.
- Sync requests.
- Coordinate updates.
- Hand paddle input.
- Park completion.
- Move-complete polling.

Press `ESC` to return to the menu.

### 0. Exit

Exits the menu and unloads the OMS DLL.

---

## Main Program Features

### OMS DLL Loading

The program dynamically loads `68SERMC.DLL` and resolves the required functions:

- `InitOmsCommPort`
- `SendString`
- `SendAndGetString`

The controller communication record uses a baud rate of `9600`.

### Controller Initialization

Startup sends:

```text
RS;
```

Then waits approximately 5 seconds.

After that, it sends:

```text
EF; IO0,0; AX; DBI; LP0; SC; AY; DBI; LP0; SC;
```

Then it starts RA tracking:

```text
AX JF<TrkRate>;
```

Example:

```text
AX JF500.600000;
```

### TheSky Shared Memory

The program communicates with TheSky through `teleapi.dll` shared memory using a coordinate structure containing:

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

The timer loop checks this shared memory for GOTO and sync requests.

### GOTO Slews

When both `RAGoto` and `DecGoto` are nonzero, the timer loop calls:

```cpp
SlewScope(raTarget, decTarget);
```

`SlewScope()` calculates altitude, hour angle, target motor counts, target quadrant, and target Y polarity. It blocks slews below 20 degrees altitude or invalid target polarity.

Supported quadrant routes in the provided code include:

- `GoQuad1to1`
- `GoQuad1to3`
- `GoQuad3to1`

### Sync

When both `RASync` and `DecSync` are nonzero, the timer loop calls:

```cpp
SyncScope();
```

`SyncScope()` calculates the controller count position for the supplied RA/DEC, loads the position into the controller with `AX LP...` and `AY LP...`, restarts RA tracking, clears the sync request, and updates the current coordinates.

### Parking

Parking is requested by:

```cpp
Park();
```

The timer loop then sends:

```text
AX KL;
AA VL75000,50000; MA0,0,,; GD; ID;
```

This moves toward controller position `0,0`.

> Verify that controller position `0,0` matches the real physical park position before relying on this command on hardware.

### Normal Hand Paddle

The normal hand paddle reads controller I/O with:

```text
BX;
```

The response is decoded into:

- Manual/Fast button.
- South button.
- North button.
- East button.
- West button.

Normal RA corrections:

- East uses `TrkRate - handpad_ra_east_rate`.
- West uses `TrkRate + handpad_ra_west_rate`.
- Releasing East/West stops RA and restarts tracking.

Normal DEC corrections:

- North jogs positive Y after optional DEC bump logic.
- South jogs negative Y after optional DEC bump logic.
- Releasing North/South stops DEC.

### Manual / Fast Paddle

If the manual/fast button is active, the program calls:

```cpp
HandleFastPaddle();
```

Fast paddle uses fine speeds which directly control motor speeds. These should not be set above the max motor slew speeds (x max = 75000, y max = 50000)

```ini
fastpad_x_fine=2000
fastpad_y_fine=2000
```

Fast RA movement stops the X axis, jogs in the selected direction, and restarts tracking when released.

Fast DEC movement jogs Y in the selected direction and stops when released.

### DEC Bump Behavior

Before some DEC moves, the program sends a short bump to handle backlash or direction reversal behavior.

Config values:

```ini
dec_bump_rate=75
dec_bump_ms=100
```

Related functions:

- `BumpNorth()`
- `BumpSouth()`

---

## Useful Functions

| Function | Purpose |
|---|---|
| `loadparams()` | Loads default values before reading `config.ini`. |
| `LoadConfig()` | Reads `config.ini`. |
| `ApplyConfig()` | Applies config values to runtime globals. |
| `PrintConfig()` | Prints active configuration. |
| `LoadDLL()` | Loads `68SERMC.DLL` and resolves function pointers. |
| `UnloadDLL()` | Frees the loaded OMS DLL. |
| `InitComm()` | Initializes communication with the OMS controller. |
| `SendCommand()` | Sends a command string to the controller. |
| `SendAndGetCommand()` | Sends a controller query and reads the response. |
| `TimerUpdate()` | Main update function for movement, sync, GOTO, park, coordinate updates, and hand paddle input. |
| `RunTimerLoopUntilIdleOrEscape()` | Runs `TimerUpdate()` until park completes, lockout occurs, or `ESC` is pressed. |
| `ManualControlMenu()` | Console menu. |
| `Park()` | Sets flags to request parking. |
| `StopAll()` | Sends `AA ST;` to stop all axes. |
| `HandleHandPaddle()` | Normal hand paddle behavior. |
| `HandleFastPaddle()` | Manual/fast paddle behavior. |
| `BumpNorth()` | Short DEC bump in one direction. |
| `BumpSouth()` | Short DEC bump in the opposite direction. |
| `SlewScope()` | Processes GOTO slew requests. |
| `SyncScope()` | Processes sync requests. |
| `GetQandY()` | Converts RA/DEC into altitude, hour angle, motor counts, quadrant, and Y polarity. |
| `UpdateCoord()` | Updates current coordinate state. |

---

## Known Issues / Items to Verify

- Park moves to controller position `0,0`; the controller origin must match the intended physical park location.
- The relationship between controller origin, TheSky, and TPoint corrections should be verified before regular operation.
- Axis signs and hand paddle directions should be tested on hardware.
- GOTO quadrant routing currently depends on the available `quad*.cpp` routines.
- `config.ini` includes `xvlslew` and `yvlslew`. Confirm that the active code applies these values wherever the quadrant routines expect them.
- The program is Windows-only and currently set up for 32-bit MinGW builds.

---

## Troubleshooting

### The program cannot find the controller

Check:

- The controller is powered on.
- The serial cable is connected.
- Device Manager shows the serial device.
- `com_port` in `config.ini` matches the Device Manager COM port.
- No other program is using the same COM port.

### `LoadLibrary` fails

Check:

- `68SERMC.DLL` is in the setup folder or the path in `config.ini` is correct.
- `teleapi.dll` is in the setup folder or the path in `config.ini` is correct.
- The DLLs are compatible with the 32-bit executable.

### Hand paddle input does not work correctly

Check:

- The initialization sequence completed.
- The controller received `EF; IO0,0; AX; DBI; LP0; SC; AY; DBI; LP0; SC;`.
- The `RS;` reset delay is long enough.
- The I/O bit wiring matches the code.
- The manual/fast button bit is wired correctly.

### Telescope moves the wrong way

Check:

- Axis motor wiring.
- Sign of `AX JG...` and `AY JG...` commands.
- `yPole` and quadrant logic.
- Whether the target is east/west of the meridian.
- Whether the controller origin was loaded correctly.

### Parking is offset

Check:

- Controller internal `0,0` position.
- Last sync operation.
- TheSky/TPoint model state.
- Whether the original Pascal/Delphi program loaded a controller position before parking.
- Mechanical zero alignment.

---

## Development Notes

- The project uses global state because it is a direct conversion from an older Pascal/Delphi-style control program.
- `StateVar` defines the axis state machine:
	- `Off`
	- `Tracking`
	- `CorrectingE`
	- `CorrectingW`
	- `CorrectingN`
	- `CorrectingS`
	- `Slewing`
- `TimerUpdate()` is the central runtime function.
- `SlewScope()` and the `quad*.cpp` files handle quadrant-specific GOTO behavior.
- `SyncScope()` loads calculated motor positions into the controller.
- `HandPaddle.cpp` handles normal hand paddle input.
- `ManualPaddle.cpp` / fast paddle logic handles manual high-speed or fine paddle movement.
