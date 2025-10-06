unit GetQandYU;
//
// Updated for GVO fork mount 6/1/08
//

interface

procedure GetQandY(ra, dec: Real; var Alt, HA, xcount, ycount: Real; var q, y: Integer);

implementation

uses
	Main, Utils, Math;

procedure GetQandY(ra, dec: Real; var Alt, HA, xcount, ycount: Real; var q, y: Integer);

// GetQandY gets quadrant, HA, Alt, xcount, ycount, and ypole of current RA/Dec

begin
	MainForm.Timer_Main.Enabled := False;	// don't allow other calls

	Meridian := GetStime;
	EastHor := Meridian + 6.0;				// ha at east horizon
	WestHor := Meridian - 6.0;				// ha at west horizon

	if (WestHor < 0.0) then
		WestHor := WestHor + 24.0;
	if (EastHor > 24.0) then
		EastHor := EastHor - 24.0;

	EastHA := -6;
	NorthHA := 12;
	WestHA := 6;
	SouthHA := 0;

	// Get Altitude
	HA := 15 * (Meridian - Ra);
	Alt := ArcSin(
		(Sin(DToR * Dec) * Sin(DToR * c_Lat)) +
		(Cos(DToR * Dec) * Cos(DToR * HA) * Cos(DToR * C_Lat))
	);
	Alt := RToD * Alt;	// A = -90 to +90 degrees Altitude

	// Get Ypole
	HA := (Meridian - Ra);
	if (HA < -12) then
		HA := HA + 24;
	if (HA > 12) then
		HA := HA - 24;

	if ((HA <= SouthHA) and (HA >= EastHA)) then
	begin
		Y := 1;
		Q := 1;
	end
	else if ((HA < EastHA) and (HA > -NorthHA)) then
	begin
		Y := -1;
		Q := 2;
	end
	else if ((HA >= SouthHA) and (HA <= WestHA)) then
	begin
		Y := 1;
		Q := 3;
	end
	else if ((HA > WestHA) and (HA < NorthHA)) then
	begin
		Y := -1;
		Q := 4;
	end;

	// Get position counts - Dec first
	// Dec stepper counts positive when north of zenith (> lat),
	// negative when south of zenith (< lat)
	if (Dec >= c_Lat) then
	begin
		// counts positive
		ycount := (Dec - c_Lat) * DecFact;
	end
	else
	begin
		ycount := -1 * ((c_Lat - Dec) * DecFact);
	end;

	// Get RA Counts
	// East of meridian is negative counts (-HA)
	// West of meridian is positive counts (+HA)
	xcount := (HA * 15) * RaFact;

	// From german equatorial model
	// if (Q = 1) then Xcount := ((6.0 - abs(TempHa)) * 15) * RaFact
	// else if (Q = 2) then Xcount := ((abs(TempHa) - 6.0) * 15) * RaFact
	// else if (Q = 3) then Xcount := ((abs(TempHa) - 6.0) * 15) * RaFact
	// else if (Q = 4) then Xcount := ((6.0 - abs(TempHa)) * 15) * RaFact;
	// if ((Q = 4) or (Q = 2)) then Xcount := Xcount * -1.0;

	MainForm.Timer_Main.Enabled := True;
end;

end.
