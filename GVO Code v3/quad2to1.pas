unit quad2to1;

interface
uses utils, sysutils,windows;
procedure GoQuad2to1;
implementation
uses Main, OMS68SERMC ;
procedure GoQuad2to1;
Var
  CmdStr,CmdStr1, Cmdstr2 : string ;
   Begin {2 to 1}
     MovingRa := True;
     MovingDec := True;
     RaPos := RaTarget - EastHor ;
     if(RaPos > 0.0)Then RaPos := 24.0 - RaPos;
     RaPos := abs(Rapos);
     Rapos := (Rapos * RaFact)*15 ;
     Str(Rapos:14:0,CmdStr);
     CmdStr := TrimLeft(CmdStr);
     Str(TrkRate,CmdStr2);
     CmdStr1 := 'AX ST;';              { Send RA STOP}
       SendString(CommRecord,pchar(CmdStr));
     CmdStr := 'AX  VL'+xvlslew+' MA'+ CmdStr + ' GD ID; ';      {Send RA SLEW}
       SendString(CommRecord,pchar(CmdStr));
     DecPos := 90.0 - Dectarget;
     If(DecTarget < 0 ) Then  DecPos := 90.0 + Abs(DecTarget);
     DecPos := (DecPos * DecFact);
     Str(Decpos:14:0,CmdStr);
     cmdStr := TrimLeft(cmdStr);
     CmdStr := 'AY  VL'+yvlslew+ ' MA'+ CmdStr + ' GD ID; ';          {DEC SLEW}
        SendString(CommRecord,pchar(CmdStr));
{Scope is SLEWING}
   End;  {2 to 1}
end.





