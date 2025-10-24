unit quad1to1;

interface
uses utils, sysutils,windows;
procedure GoQuad1to1(Xcount,Ycount:real);

implementation
uses Main, OMS68SERMC ;

procedure GoQuad1to1(Xcount,Ycount:real);    //east of meridian
Var
	CmdStr,CmdStr1, Cmdstr2 : string ;
Begin {1 to 1}
	MovingRa := True;
	MovingDec := True;
//
	RaPos := xcount;                     //xcount should be negative
	Rapos := abs(Rapos)  ;
	Str(Rapos:14:0,CmdStr);
	CmdStr := TrimLeft(CmdStr);
	Str(TrkRate,CmdStr2);
	CmdStr1 := 'AX ST;';              { Send RA STOP}
	SendString(CommRecord,pchar(CmdStr));
//       
	CmdStr := 'AX  VL'+xvlslew+' MA-'+ CmdStr + ' GD ID; ';      {Send RA SLEW}
	SendString(CommRecord,pchar(CmdStr));
//
	DecPos := ycount;
	Str(abs(Decpos):14:0,CmdStr);
	cmdStr := TrimLeft(cmdStr);
	if(DecPos <0) then
	begin
		CmdStr := 'AY  VL'+yvlslew+ ' MA-'+ CmdStr + ' GD ID; ';          {DEC SLEW}
		SendString(CommRecord,pchar(CmdStr));
	end
	else
	begin
		CmdStr := 'AY  VL'+yvlslew+ ' MA'+ CmdStr + ' GD ID; ';          {DEC SLEW}
		SendString(CommRecord,pchar(CmdStr));
	end;
{Scope is SLEWING}
End;  {1 to 1}

end.
