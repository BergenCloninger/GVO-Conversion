unit TimerUnit;

interface
uses SysUtils, Utils, windows;

Procedure TimerUpdate();
Procedure AddTicks();
procedure Padle_Timer_Update();

implementation
Uses main, slew, CalcCoord, HandPadle, Sync, OMS68SERMC;

procedure TimerUpdate();
Var
	CmdStr, CmdStr2, TempStr: String;
	Label EndTimer;
Begin {Timer Update}
	IF (NoPassword) then
	begin
		cmdstr := 'AA ST;';
		SendString(CommRecord, pchar(CmdStr));
		MainForm.Timer_Main.enabled := True;
		exit;
	end;

	MainForm.CmdEdit.Enabled := False;

	// Timer Off
	MainForm.Timer_Main.enabled := False;

	// ***********************if moving check for end of move***********************
	If (MovingRA) Then
	Begin
		SendAndGetString(CommRecord, 'AX QA;', Response);
		Tempstr := Response;
		if (length(tempstr) <> 4) Then exit;
		If (UpCase(Tempstr[2]) = 'D') Then
		Begin
			MovingRA := False;
			MainForm.cmdbx.Items.Insert(0, 'RA Axis Move Complete');
			Str(TrkRate:10:6, CmdStr2);
			CmdStr := 'AX JF' + CmdStr2 + ';';     //Back to Tracking
			SendString(CommRecord, pchar(CmdStr)); {RA Back To TRACKING}
			Coord(pndomem^).RAGoTo := 0.0;
			mainform.ejog.enabled := true;
			mainform.wjog.enabled := true;
		End;
	End;

	If (MovingDEC) Then
	Begin
		SendAndGetString(CommRecord, 'AY QA;', Response);
		Tempstr := Response;
		if (length(tempstr) <> 4) Then exit;
		If (UpCase(Tempstr[2]) = 'D') Then
		Begin
			MovingDEC := False;
			MainForm.cmdbx.Items.Insert(0, 'DEC Axis Move Complete');
			Coord(pndomem^).decGoTo := 0.0;
			mainform.sjog.enabled := true;
			mainform.njog.enabled := true;
		End;
	End;

	// *********************Update Coordinates*********************
	HalfSecondCounter := HalfSecondCounter + 1;
	If (HalfSecondCounter = 5) Then HalfSecondCounter := 0;
	If (HalfSecondCounter = 4) then UpdateCoord; {coordinates updated - at .5 sec}

	// ************************Leave if in motion**********************************
	If ((MovingRA) OR (MovingDec)) Then GoTo EndTimer;

	// ***************************Sync Scope****************************
	If ((Coord(pndomem^).RASync <> 0.0) AND (Coord(pndomem^).decSync <> 0.0)) Then
		SyncScope;  // sync requested

	// ********************** Slew if commanded ***********************
	If ((Coord(pndomem^).RAGoTo <> 0.0) AND (Coord(pndomem^).decGoTo <> 0.0)) Then
	Begin
		SlewScope;
		GoTO EndTimer;
	End;

	// *************Check for Park************
	If (parkit = True) Then
	Begin
		Parkit := False;
		mainform.sjog.enabled := False;
		mainform.njog.enabled := False;
		mainform.ejog.enabled := False;
		mainform.wjog.enabled := False;
		MovingRa := True;
		MovingDec := True;

		CmdStr := 'AX  kl; ';      {Send RA stop}
		SendString(CommRecord, pchar(CmdStr));
		sleep(300);
		CmdStr := 'AA vl75000,50000; MA0,0,,; GD; ID; ';      {Send home}
		SendString(CommRecord, pchar(CmdStr));
		MainForm.ButtonStop.Caption := 'START';
		GoTO EndTimer;
	End;

	// *****************handle padle***********
	HandleHandPadle;

	// *******************{Timer Back On}**********************
EndTimer:
	if (MainForm.ButtonStop.Caption = 'STOP') then
	begin
		MainForm.Timer_Main.enabled := True;
	end;
End; {Timer Update}

Procedure AddTicks();
Begin
	// Update Quadrant display
	MainForm.Quad1.Visible := False;
	MainForm.Quad2.Visible := False;
	MainForm.Quad3.Visible := False;
	MainForm.Quad4.Visible := False;

	If (Ypole > 0) Then MainForm.YPolarity.Caption := '+'
	Else MainForm.YPolarity.Caption := '-';

	If (Quadrant = 1) Then MainForm.Quad1.Visible := True
	Else If (Quadrant = 2) Then MainForm.Quad2.Visible := True
	Else If (Quadrant = 3) Then MainForm.Quad3.Visible := True
	Else If (Quadrant = 4) Then MainForm.Quad4.Visible := True;

	MainForm.Refresh;
End; {Add Ticks}

procedure Padle_Timer_Update();
Begin {Timer Update}
	// Timer Off
	MainForm.Timer_padle.Enabled := False;
	HandleHandPadle;
	MainForm.Timer_padle.Enabled := True;
End;

End.
