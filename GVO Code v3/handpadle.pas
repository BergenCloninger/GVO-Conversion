unit handpadle;

interface

uses
	utils, sysutils, windows, dialogs;

procedure HandleHandpadle;

implementation

uses
	Main, OMS68SERMC, ManualPadle, qgraphics;

procedure HandleHandpadle;
var
	CmdStr, TempStr, xspeed, yspeed: String;
	i: Integer;
begin
	EastPushed := False;
	WestPushed := False;
	NorthPushed := False;
	SouthPushed := False;

	SendAndGetString(CommRecord, 'BX;', Response);
	TempStr := Response;
	if (Length(TempStr) <> 2) then Exit;

	// Handle Buttons
	io := Byte(TempStr[1]); // get First Char
	if (io > $39) then
		io := io - $37
	else
		io := io - $30;

	// Manual bit is high speed slew
	if (io AND ManualBit <> 0) then
		ManualPushed := True
	else
		ManualPushed := False;

	if (io AND SouthBit <> 0) then
		SouthPushed := True
	else
		SouthPushed := False;

	io := Byte(TempStr[2]); // get Last Char
	if (io > $39) then
		io := io - $37
	else
		io := io - $30;

	if (io AND NorthBit <> 0) then
		NorthPushed := True
	else
		NorthPushed := False;

	if (io AND EastBit <> 0) then
		EastPushed := True
	else
		EastPushed := False;

	if (io AND WestBit <> 0) then
		WestPushed := True
	else
		WestPushed := False;

	if (EastPushed) then
		MainForm.Label7.Color := clRed
	else
		MainForm.Label7.Color := clSilver;

	if (WestPushed) then
		MainForm.Label8.Color := clRed
	else
		MainForm.Label8.Color := clSilver;

	if (NorthPushed) then
		MainForm.Label5.Color := clRed
	else
		MainForm.Label5.Color := clSilver;

	if (SouthPushed) then
		MainForm.Label6.Color := clRed
	else
		MainForm.Label6.Color := clSilver;

	// if (ManualPushed) then MainForm.Color := clYellow else MainForm.Color := clBtnFace;

	if ((ManualPushed) or (MainForm.Select5Inch.Checked = True)) then
	begin
		HandleFastPadle;
		Exit;
	end
	else
	begin
		xspeed := xvl;
		yspeed := yvl;
	end;

	// Hand padle no high speed
	// correcting is slow correct
	// slewing is fast correcting

	// X Axis (RA) Update
	case Xstate of
		Off:
			begin
			end;

		Tracking:
			begin
				if (EastPushed) then
				begin
					Xstate := CorrectingE;
					i := Round(TrkRate / 2);
					Str(i, TempStr);
					CmdStr := 'AX JG' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr)); // slow track by 1/2
				end;

				if (WestPushed) then
				begin
					Xstate := CorrectingW;
					i := Round(TrkRate + (TrkRate / 2));
					Str(i, TempStr);
					CmdStr := 'AX JG' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr)); // speed up track by 1/2
				end;
			end;

		CorrectingE:
			begin
				if ((not EastPushed) and (not WestPushed)) then
				begin // go to track
					Xstate := Tracking;
					CmdStr := 'AX ST ;';
					SendString(CommRecord, pchar(CmdStr));
					SendString(CommRecord, pchar(CmdStr));
					Str(TrkRate:10:6, TempStr);
					CmdStr := 'AX JF' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;

				if ((EastPushed) and (WestPushed)) then
				begin // start intermediate slew to east
					XState := Slewing;
					CmdStr := 'AX ST; AX JG-' + XSpeed + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;

		CorrectingW:
			begin
				if ((not EastPushed) and (not WestPushed)) then
				begin // start tracking
					Xstate := Tracking;
					CmdStr := 'AX ST ;';
					SendString(CommRecord, pchar(CmdStr));
					SendString(CommRecord, pchar(CmdStr));
					Str(TrkRate:10:6, TempStr);
					CmdStr := 'AX JF' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;

				if ((EastPushed) and (WestPushed)) then
				begin // start intermediate slew to west
					XState := Slewing;
					CmdStr := 'AX JG' + XSpeed + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;

		Slewing:
			begin
				if ((not EastPushed) and (not WestPushed)) then
				begin
					XState := Tracking;
					Str(TrkRate:10:6, TempStr);
					CmdStr := 'AX ST;';
					SendString(CommRecord, pchar(CmdStr));
					CmdStr := 'AX JF' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;
	else;
	end; // X State case

	// Y Axis (Dec) Update
	// bump north or south for future anti-backlash
	case Ystate of
		Off:
			begin
			end;

		Tracking:
			begin
				if (NorthPushed) then
				begin
					if ((Ypole >= 0) and (LastDecNorth)) then
						BumpSouth
					else if (not LastDecNorth) then
						BumpNorth;

					Ystate := CorrectingN;
					i := Round(TrkRate / 4);
					Str(i, TempStr);
					CmdStr := ' AY JG' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;

				if (SouthPushed) then
				begin
					if ((Ypole >= 0) and (not LastDecNorth)) then
						BumpNorth
					else if (LastDecNorth) then
						BumpSouth;

					Ystate := CorrectingS;
					i := Round(TrkRate / 4);
					Str(i, TempStr);
					CmdStr := ' AY JG-' + TempStr + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;

		CorrectingN:
			begin
				if ((not NorthPushed) and (not SouthPushed)) then
				begin
					Ystate := Tracking;
					CmdStr := ' AY ST;';
					SendString(CommRecord, pchar(CmdStr));
				end;

				if ((NorthPushed) and (SouthPushed)) then
				begin
					Ystate := Slewing;
					CmdStr := ' AY JG' + ySpeed + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;

		CorrectingS:
			begin
				if ((not NorthPushed) and (not SouthPushed)) then
				begin
					Ystate := Tracking;
					CmdStr := ' AY ST;';
					SendString(CommRecord, pchar(CmdStr));
				end;

				if ((NorthPushed) and (SouthPushed)) then
				begin
					Ystate := Slewing;
					CmdStr := ' AY JG-' + ySpeed + ';';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;

		Slewing:
			begin
				if ((not NorthPushed) and (not SouthPushed)) then
				begin
					Ystate := Tracking;
					CmdStr := ' AY ST; ';
					SendString(CommRecord, pchar(CmdStr));
				end;
			end;
	end; // Y State case
end;

end.
