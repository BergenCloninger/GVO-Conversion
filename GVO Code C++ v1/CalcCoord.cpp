#include "CalcCoord.h"

// Interface includes, not sure if windows api calls are required for this now, but im doing 1:1 for now
#include <cmath>
#include <windows.h>
#include <string>
// #include <dialogs> not sure if there is an analogue for this, ill have to see what each function does as I work.
// It's possible that I can use std instead of windows.h as well when doing a command line implementation.

// #include "Utils.h" to be implemented, dependent on Utils.pas
// #include "Main.h"
// #include "GetQandYU.h"
// #include "OMS68SERMC.h" //high priority

//Implementation (start of program)
void UpdateCoord() {
    std::string CmdStr, TempStr1, TempStr2, TempStr3; //really cool and useful names here
    double x, y, alt, ha, Xcount, Ycount, TempStime;
    HWND thandle; 

    //start of "begin" from pascal code
    thandle = 0;
    // requires main implementation for further work
}

void doDec() {

}