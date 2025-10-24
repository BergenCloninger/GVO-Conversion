unit quad3to1;

interface
uses utils, sysutils,windows;
procedure GoQuad3to1(Xcount,Ycount:real);

implementation
uses Main, IO, OMS68SERMC ;

procedure GoQuad3to1(Xcount,Ycount:real);
Var
	CmdStr,CmdStr1, Cmdstr2 : string ;
Begin {3 to 1}
	MovingRa := True;
	MovingDec := True;

	RaPos := xCount ;         //is positive

	Str(Rapos:14:0,CmdStr);
	CmdStr := TrimLeft(CmdStr);
	Str(TrkRate,CmdStr2);
	CmdStr1 := 'AX ST;';              { Send RA STOP}
	SendString(CommRecord,pchar(CmdStr));
//
	CmdStr := 'AX  VL'+xvlslew+' MA'+ CmdStr + ' GD ID; ';      {Send RA SLEW}
	SendString(CommRecord,pchar(CmdStr));
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
End;  {3 to 1}

end.
