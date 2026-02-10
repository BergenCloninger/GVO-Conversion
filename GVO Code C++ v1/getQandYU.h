#pragma once

// Compute quadrant, y-pole, altitude, hour angle, and stepper counts from RA/Dec
void GetQandY(double ra, double dec, double& Alt, double& HA, double& Xcount, double& Ycount, int& q, int& y);
