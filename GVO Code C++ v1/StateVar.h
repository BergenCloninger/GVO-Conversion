#pragma once

enum class StateVar : int {
	Off = 0,
	Tracking,
	CorrectingE,
	CorrectingW,
	CorrectingN,
	CorrectingS,
	Slewing
};
