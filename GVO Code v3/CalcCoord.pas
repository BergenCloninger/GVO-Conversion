unit CalcCoord;

interface

uses
  Utils, SysUtils, Windows, Math;

procedure UpdateCoord;

implementation

uses
	Main, GetQandYU, Dialogs, OMS68SERMC;

procedure UpdateCoord;  {from stepper controller}
var
	CmdStr, TempStr1, TempStr2, TempStr3: string;
	x, y, alt, ha, Xcount, Ycount, TempStime: real;
	thandle: HWND;
label
	DoDec;

begin
	thandle := 0;
	MainForm.labeltkrate.Caption := FloatToStr(TrkRate);

  	// blink active indicator
	if (MainForm.PanelStatus.Visible = True) then
		MainForm.PanelStatus.Visible := False
	else
		MainForm.PanelStatus.Visible := True;

	// get positions, wait for them
  	while (SendAndGetString(CommRecord, 'PP;', Response) <> 0) do;
  	begin
		TempList.CommaText := Response;
		x := StrToFloat(TempList[0]); // x = ra counts
		// MainForm.Labelraval.Caption := TempList[0]; // write to Form ra counts
		MainForm.labeldecval.Caption := TempList[1];  // write to Form dec counts
  	end;

	CmdStr := '';
	TempStime := GetStime;
  	if (X = 0.0) then X := 1.0;
	
	// Ra Counts
	// east of meridian is neg counts (-ha)
	// west of meridian is positive counts (+ha)
 	if ((X > 0.0) and (Ypole > 0)) then // west of meridian, above pole. quad 3
  	begin {quad 3}
		RANow := TempStime; // meridian = 0 counts
		RANow := RANow - ((X / RAFact) / 15.0);
		if (RANow < 0) then RANow := RANow + 24;

		// calc hr min, secs
		RAHr := Trunc(RANow);
		RAMin := Trunc(Frac(RANow) * 60);
		RASec := Trunc((Frac(RANow) * 3600) - (RAMin * 60));
		Str(RAHr:2, TempStr1);

		// display ra
		MainForm.Rahredit.Text := TempStr1;
		Str(RAMin:2, TempStr2);
		MainForm.Raminedit.Text := TempStr2;
		Str(RASec:2, TempStr3);
		MainForm.Rasecedit.Text := TempStr3;

    goto DoDec;
  	end;  {quad 3}

	if ((X < 0.0) and (Ypole < 0)) then
	begin  {Quad 2}
		X := Abs(X);
		RANow := TempStime;
		RANow := RANow + ((X / RAFact) / 15.0);
		if (RANow > 24) then RANow := RANow - 24;

		// calc hr, min, sec
		RAHr := Trunc(RANow);
		RAMin := Trunc(Frac(RANow) * 60);
		RASec := Trunc((Frac(RANow) * 3600) - (RAMin * 60));

		// display ra
		Str(RAHr:2, TempStr1);
		MainForm.Rahredit.Text := TempStr1;
		Str(RAMin:2, TempStr2);
		MainForm.Raminedit.Text := TempStr2;
		Str(RASec:2, TempStr3);
		MainForm.Rasecedit.Text := TempStr3;

		goto DoDec;
	end;  {Quad 2}

	if ((X < 0.0) and (Ypole > 0)) then  // east of meridian, above pole, quad 1
	begin  {Quad 1}
		X := Abs(X);
		RANow := TempStime;
		RANow := RANow + ((X / RAFact) / 15.0);
		if (RANow > 24) then RANow := RANow - 24;

		// calc hr, min, sec
		RAHr := Trunc(RANow);
		RAMin := Trunc(Frac(RANow) * 60);
		RASec := Trunc((Frac(RANow) * 3600) - (RAMin * 60));

		// display ra
		Str(RAHr:2, TempStr1);
		MainForm.Rahredit.Text := TempStr1;
		Str(RAMin:2, TempStr2);
		MainForm.Raminedit.Text := TempStr2;
		Str(RASec:2, TempStr3);
		MainForm.Rasecedit.Text := TempStr3;

		goto DoDec;
	end;  {Quad 1}

	if ((X > 0.0) and (Ypole < 0)) then
	begin  {Quad 4}
		RANow := TempStime; // meridian = 0 counts
		RANow := RANow - ((X / RAFact) / 15.0);
		if (RANow < 0) then RANow := RANow + 24;

		// calc hr min, secs
		RAHr := Trunc(RANow);
		RAMin := Trunc(Frac(RANow) * 60);
		RASec := Trunc((Frac(RANow) * 3600) - (RAMin * 60));
		Str(RAHr:2, TempStr1);

		// display ra
		MainForm.Rahredit.Text := TempStr1;
		Str(RAMin:2, TempStr2);
		MainForm.Raminedit.Text := TempStr2;
		Str(RASec:2, TempStr3);
		MainForm.Rasecedit.Text := TempStr3;

		goto DoDec;
	end;  {Quad 4}

  // *********************** Do Dec Now *************************************
  // dec stepper counts positive when north of zenith, neg when south of zenith

DoDec:
	CmdStr := '';
	Y := StrToFloat(TempList[1]); // y = counts
	DECNow := C_Lat + (Y / DECFact);

	// calc deg, min, sec
	decdeg := Trunc(DecNow);
	decMin := Trunc(Frac(DecNow) * 60);
	decSec := Trunc((Frac(DecNow) * 3600) - (decMin * 60));

	// display dec
	Str(decdeg:2, TempStr1);
	MainForm.decdegedit.Text := TempStr1;
	Str(decMin:2, TempStr2);
	MainForm.decminedit.Text := TempStr2;
	Str(decSec:2, TempStr3);
	MainForm.decsecedit.Text := TempStr3;

	// ************** Set Quadrant Now ****************************************
	Coord(pndomem^).RA := RANow;
	Coord(pndomem^).Dec := DecNow;
	GetQandY(RANow, DecNow, Alt, HA, Xcount, Ycount, Quadrant, Ypole);

	// check for below horizon (20 degrees)
	if (Alt < 20) then
	begin
		CmdStr := 'AA ST;';
		SendString(CommRecord, PChar(CmdStr));
		MainForm.ButtonStop.Caption := 'START';
		Coord(pndomem^).RAGoTo := 0.0;
		Coord(pndomem^).decGoTo := 0.0;
		Windows.MessageBox(thandle, 'No track below horizon', 'Telescope Control', MB_SYSTEMMODAL);
	end;

	// display alt, azimuth
	altdeg := Trunc(Alt);
	altmin := Trunc(Frac(Alt) * 60);
	altsec := Trunc((Frac(Alt) * 3600) - (altmin * 60));

	Str(altdeg:2, TempStr1);
	MainForm.editaltdeg.Text := TempStr1;
	Str(altmin:2, TempStr2);
	MainForm.editaltmin.Text := TempStr2;
	Str(altsec:2, TempStr3);
	MainForm.editaltsec.Text := TempStr3;

	azimuth := (-Sin(DECNow)) / Cos(C_Lat);
	if (azimuth < -1) then azimuth := -1;
	if (azimuth > 1) then azimuth := 1;
	azimuth := RadToDeg(ArcCos(azimuth));

	azdeg := Trunc(azimuth);
	azmin := Trunc(Frac(azimuth) * 60);
	azsec := Trunc((Frac(azimuth) * 3600) - (azmin * 60));

	Str(azdeg:2, TempStr1);
	MainForm.editazdeg.Text := TempStr1;
	Str(azmin:2, TempStr2);
	MainForm.editazmin.Text := TempStr2;
	Str(azsec:2, TempStr3);

	MainForm.Refresh;
end;  {UpdateCoord}

end.
