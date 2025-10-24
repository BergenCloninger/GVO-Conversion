unit handpadle;

interface

uses utils, sysutils, windows, dialogs;

procedure HandleHandpadle;

implementation

uses Main, OMS68SERMC, ManualPadle, qgraphics;

procedure HandleHandpadle;
var
	CmdStr, TempStr, xspeed, yspeed: String;
	i: Integer;
Begin
	EastPushed := False; WestPushed := False; NorthPushed := False; SouthPushed := False;
	SendAndGetString(CommRecord, 'BX;', Response);
	Tempstr := Response;
	if (length(tempstr) <> 2) Then exit;
	//Handle Buttons
	io := Byte(TempStr[1]);                    {get First Char}
	if (io > $39) Then io := io - $37
	Else io := io - $30;
	//
	//manual bit is high speed slew
	//
	if (io AND ManualBit <> 0) Then manualPushed := True else ManualPushed := false;
	if (io AND SouthBit <> 0) Then SouthPushed := True else SouthPushed := false;
	//
	io := Byte(TempStr[2]);                    {get Last Char}
	if (io > $39) Then io := io - $37
	Else io := io - $30;
	if (io AND NorthBit <> 0) Then NorthPushed := True else NorthPushed := false;
	if (io AND EastBit <> 0) Then EastPushed := True else EastPushed := false;
	if (io AND WestBit <> 0) Then WestPushed := True else WestPushed := false;
	//
	If (EastPushed) Then MainForm.Label7.Color := clred else MainForm.Label7.Color := clsilver;
	If (WestPushed) Then MainForm.Label8.Color := clred else MainForm.Label8.Color := clsilver;
	If (NorthPushed) Then MainForm.label5.Color := clred else MainForm.label5.Color := clsilver;
	If (SouthPushed) Then MainForm.label6.Color := clred else MainForm.label6.Color := clsilver;
	//  If(manualPushed)Then MainForm.Color := clyellow else MainForm.Color := clBtnFace;

	//
	If ((ManualPushed) OR (MainForm.Select5Inch.Checked = True)) Then
	Begin
		HandleFastPadle;
		Exit;
	End
	Else
	Begin
		xspeed := xvl;
		yspeed := yvl;
	End;
	//
	// Hand padle no high speed
	//
	//  correcting is slow correct
	//
	//  Slewing is fast correcting
	//
	//
	{X Axis (RA) Update}
	Case Xstate of
		Off: Begin
			End;
		Tracking: Begin
			If (EastPushed) Then
			Begin
				Xstate := CorrectingE;
				i := Round(TrkRate / 2);
				Str(i, TempStr);
				CmdStr := 'AX JG' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));          //slow track by 1/2
			End;
			{}
			If (WestPushed) Then
			Begin
				Xstate := CorrectingW;
				i := Round(TrkRate + (TrkRate / 2));
				Str(i, TempStr);
				CmdStr := 'AX JG' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));        //speed up track by 1/2
			End;
			{}
		End;
		CorrectingE: Begin
			If ((NOT EastPushed) AND (NOT WestPushed)) Then
			Begin                        //go to track
				Xstate := Tracking;
				CmdStr := 'AX ST ;';
				SendString(CommRecord, pchar(CmdStr)); SendString(CommRecord, pchar(CmdStr));
				Str(TrkRate:10:6, TempStr);
				CmdStr := 'AX JF' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
			If ((EastPushed) AND (WestPushed)) Then
			Begin       //start intermediate slew  to east
				XState := Slewing;
				CmdStr := 'AX ST; AX JG-' + XSpeed + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
		End;
		CorrectingW: Begin
			If ((NOT EastPushed) AND (NOT WestPushed)) Then
			Begin                         //start tracking
				Xstate := Tracking;
				CmdStr := 'AX ST ;';
				SendString(CommRecord, pchar(CmdStr)); SendString(CommRecord, pchar(CmdStr));
				Str(TrkRate:10:6, TempStr);
				CmdStr := 'AX JF' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
			If ((EastPushed) AND (WestPushed)) Then
			Begin                 //start intermediate slew to west
				XState := Slewing;
				CmdStr := 'AX JG' + XSpeed + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
			{}
		End;
		Slewing: Begin
			If ((NOT EastPushed) AND (NOT WestPushed)) Then
			Begin
				XState := Tracking;
				Str(TrkRate:10:6, TempStr);
				CmdStr := 'AX ST;';
				SendString(CommRecord, pchar(CmdStr));
				CmdStr := 'AX JF' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
		End;
	Else;
	End; {X State case}
	{}
	{Y Axis (dec) Update}
	//
	// bump north or south for future ant- backlas
	//
	Case Ystate of
		Off: Begin
			End;
		Tracking: Begin
			If (NorthPushed) Then
			Begin
				if ((Ypole >= 0) AND (LastDecNorth)) Then
					BumpSouth
				else
					If (NOT LastDecNorth) Then BumpNorth;
				Ystate := CorrectingN;
				i := Round(TrkRate / 4);
				Str(i, TempStr);
				CmdStr := ' AY JG' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
			{}
			If (SouthPushed) Then
			Begin
				if ((Ypole >= 0) AND (Not LastDecNorth)) Then
					BumpNorth
				else
					If (LastDecNorth) Then BumpSouth;
				Ystate := CorrectingS;
				i := Round(TrkRate / 4);
				Str(i, TempStr);
				CmdStr := ' AY JG-' + TempStr + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
		End;
		//
		// end of tracking state
		//
		CorrectingN: Begin
			If ((NOT NorthPushed) AND (NOT SouthPushed)) Then
			Begin
				Ystate := Tracking;
				CmdStr := ' AY ST;';
				SendString(CommRecord, pchar(CmdStr));
			End;
			If ((NorthPushed) AND (SouthPushed)) Then
			Begin
				Ystate := Slewing;
				CmdStr := ' AY JG' + ySpeed + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
		End;
		CorrectingS: Begin
			If ((NOT NorthPushed) AND (NOT SouthPushed)) Then
			Begin
				Ystate := Tracking;
				CmdStr := ' AY ST;';
				SendString(CommRecord, pchar(CmdStr));
			End;
			If ((NorthPushed) AND (SouthPushed)) Then
			Begin
				Ystate := Slewing;
				CmdStr := ' AY JG-' + ySpeed + ';';
				SendString(CommRecord, pchar(CmdStr));
			End;
		End;
		{}
		Slewing: Begin
			If ((NOT NorthPushed) AND (NOT SouthPushed)) Then
			Begin
				Ystate := Tracking;
				CmdStr := ' AY ST; ';
				SendString(CommRecord, pchar(CmdStr));
			End;
		End;
	End; {Y State Case}
End;

end.
