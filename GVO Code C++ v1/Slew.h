#pragma once

#include "GlobalValues.h"
#include "quad1to1.h"
#include "quad1to2.h"
#include "quad1to3.h"
#include "quad2to1.h"
#include "quad2to2.h"
#include "quad3to1.h"
#include "quad3to3.h"
#include "GetQandYU.h"

// Slew the telescope to the target RA/Dec based on quadrant
void SlewScope(double raTarget, double decTarget);
