unit quad1to3;

interface
uses utils, sysutils,windows;
procedure GoQuad1to3(Xcount,Ycount:real);

implementation
uses Main, OMS68SERMC ;

procedure GoQuad1to3(Xcount,Ycount:real);
Var
	CmdStr,CmdStr1, Cmdstr2 : string ;
Begin {1 to 3}               //ra target = positive counts
	MovingRa := True;
	MovingDec := True;

	RaPos := xCount ;     // is positive
	Str(Rapos:14:0,CmdStr);
	CmdStr := TrimLeft(CmdStr);
	Str(TrkRate,CmdStr2);
	CmdStr1 := 'AX ST;';              { Send RA STOP}
	SendString(CommRecord,pchar(CmdStr));
//
	CmdStr := 'AX  VL'+xvlslew+' MA'+ CmdStr + ' GD ID; ' ;
	SendString(CommRecord,pchar(CmdStr));                       {Send RA SLEW}
{}
	DecPos := abs(Ycount);
	Str(Decpos:14:0,CmdStr);
	cmdStr := TrimLeft(cmdStr);
	if ( DecTarget < c_lat) then
	begin
		CmdStr := 'AY VL'+yvlslew+' MA-'+ CmdStr + ' GD ID;';
		SendString(CommRecord,pchar(CmdStr));                 {Send DEC SLEW}
	end
	else
	begin
		CmdStr := 'AY VL'+yvlslew+' MA'+ CmdStr + ' GD ID;';
		SendString(CommRecord,pchar(CmdStr));                 {Send DEC SLEW}
	end;
{Scope is SLEWING}
End;  {1 to 3}

end.
