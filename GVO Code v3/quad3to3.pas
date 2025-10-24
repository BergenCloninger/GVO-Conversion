unit quad3to3;

interface
uses utils, sysutils,windows;
procedure GoQuad3to3(Xcount,Ycount:real);

implementation
uses Main, OMS68SERMC ;

procedure GoQuad3to3(Xcount,Ycount:real);
Var
	CmdStr,CmdStr1, Cmdstr2 : string ;
Begin {3 to 3}
	MovingRa := True;
	MovingDec := True;
	RaPos := xCount ;
	Rapos := Abs(RaPos);
	Str(Rapos:14:0,CmdStr);
	CmdStr := TrimLeft(CmdStr);
	Str(TrkRate,CmdStr2);
	CmdStr1 := 'AX ST;';              { Send RA STOP}
	SendString(CommRecord,pchar(CmdStr));
//
	CmdStr := 'AX  VL'+xvlslew+' MA-'+ CmdStr + ' GD ID; ';
	SendString(CommRecord,pchar(CmdStr));                   {Send RA SLEW}
{}
	DecPos := Ycount;
	Str(Decpos:14:0,CmdStr);
	cmdStr := TrimLeft(cmdStr);
	CmdStr := 'AY VL'+yvlslew+' MA-'+ CmdStr + ' GD ID; ';
	SendString(CommRecord,pchar(CmdStr));                   {Send DEC SLEW}
{Scope is SLEWING}
End;  {3 to 3}

end.
