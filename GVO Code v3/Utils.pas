unit Utils;

interface
Uses
 Sysutils,Dialogs,windows;

const
	pi = 3.1415926535897932;
	pi2 = (2.000000000000*pi);
	DToR = (pi/180.000000000);
	HToR = (pi/12.0000000000);
	RToD = (180.0000000000/pi);
	RToH = (12.0000000000/pi);
	SToR = (DToR/3600.000000000);
	EarthRadius = 6378.14;
  SolarParallax = (8.794*SToR);
  FK4System = 1;
  FK5System = 2;
  type
  	MeusDATE = record
		yy, mm : integer;
		dd : real;
		h, m : integer;
		s : real;
		end;

function SiderealTime(T : extended) : extended;
function SiderealTime0(T : extended) : extended;
function JDToT(JD : extended) : extended;
function CalendarToJD(var Date : MeusDATE) : extended;
Function SetStime : Real ;
Function BumpNorth : Boolean;
Function BumpSouth : Boolean;
Function GetStime : Real ;
Function Init : Boolean;
implementation
Uses Main, OMS68SERMC ;
function modpi2(x : extended) : extended;
begin
x := x - int(x / pi2) * pi2;
if (x < 0) then
  x := x + pi2;
modpi2 := x;
end;
(*****************************************************************************)
(* Name:   SiderealTime0                                                     *)
(* Type:   Function                                                          *)
(* Purpose: calculate sidereal time at Greenwich at 0h UT                    *)
(* Arguments:                                                                *)
(*   T : number of Julian centuries since J2000                              *)
(* Return value:                                                             *)
(*   Greenwich sidereal time                                                 *)
(*****************************************************************************)

function SiderealTime0(T : Extended) : Extended;
var
	Theta,TempReal : Extended;
begin
T := T - frac(T*36525+0.5)*36525;       {T at 0h of the day}
Theta := T*(36000.770053608 + T*(0.000387933 - T/38710000));
TempReal := modpi2((100.46061837 + Theta) * DToR);
SiderealTime0 := TempReal * RToH  ;  { radians to hours}

end;
function SiderealTime(T : extended) : extended;
(*****************************************************************************)
(* Name:    SiderealTime                                                     *)
(* Type:    Function                                                         *)
(* Purpose: calculate sidereal time at Greenwich                             *)
(* Arguments:                                                                *)
(*   T : number of Julian centuries since J2000                              *)
(* Return value:                                                             *)
(*   Greenwich sidereal time                                                 *)
(*****************************************************************************)
var
  Theta : extended;
  TempReal : extended ;
begin
  Theta := T*(360.98564736629*36525.00000 + T*(0.000387933 - T/38710000.000));
  TempReal := modpi2((280.46061837 + Theta) * DToR);
  SiderealTime := TempReal * RToH  ;  { radians to hours}
end;

(*****************************************************************************)
(* Name:    CalendarToJD                                                     *)
(* Type:    Function                                                         *)
(* Purpose: Julian day from calendar day                                     *)
(* Arguments:                                                                *)
(*   Date : calendar date to be converted                                    *)
(* Return value:                                                             *)
(*   The Julian day corresponding to the date                                *)
(*****************************************************************************)

function CalendarToJD(var Date : MeusDATE) : extended;
var
	A, B, m, y : integer;
	D : extended;

begin
D := Date.dd;
if (Date.mm > 2) then begin
	y := date.yy;
	m := date.mm;
	end
else begin
	y := Date.yy - 1;
	m := Date.mm + 12;
	end;

A := y div 100;

if (Date.yy < 1582)
or ((Date.yy = 1582) and ((Date.mm < 10)
or ((Date.mm = 10) and (Date.dd <= 4)))) then
	B := 0
else
	B := 2 - A + A div 4;
CalendarToJD := int(365.25 * (Y + 4716)) + int(30.6001 * (M + 1)) + D + B - 1524.5
end;

(*****************************************************************************)
(* Name:    JDToT                                                            *)
(* Type:    Function                                                         *)
(* Purpose: convert Julian Day to centuries since J2000.0.                   *)
(* Arguments:                                                                *)
(*   JD : the Julian Day to convert                                          *)
(* Return value:                                                             *)
(*   the T value corresponding to the Julian Day                             *)
(*****************************************************************************)

function JDToT(JD : extended) : extended;
begin
JDToT := (JD - 2451545.0) / 36525.0;
end;
Function SetStime : Real ;
Var
 UT,JD,InstantG,InstantHere : extended;
 CurrentDate : MeusDate;
 Present : TdateTime;
 Year, Month, Day, Hour, Min, Sec, MSec: Word;
 error : integer;
 TZInfo : TTimeZoneInformation;
Begin
  Present:= Now;
  InstantHere := Frac(Present)* 24.0000;
  {get date & Time at Greenwich}
  Present := Present +  ((int(C_Long/15.0000))/24.000000) ;
  InstantG :=  Frac(Present)* 24.0000;
  Error := GetTimeZoneInformation(TZInfo);
   case Error of
    0 : { Unknown } ;
    1 : { Standard Time } ;
    2 : InstantG := InstantG  - 1  ; { Subtract 1 for  daylight savings time !!!!!}
   end;
  If(InstantG < 0 ) Then InstantG := Instantg + 24;
{InstantG = current Time of Day at Greenwich}
  DecodeTime(Present, Hour, Min, Sec, MSec);
  CurrentDate.h := Hour;
  CurrentDate.m := Min;
  CurrentDate.s := Sec;
  DecodeDate(Present, Year, Month, Day);
  CurrentDate.yy := Year  ;
  CurrentDate.mm := Month;
  CurrentDate.dd := Day;
  JD := CalendarToJD(CurrentDate);     {JD at Greenwich at 0h }
  UT := JDToT(JD);
  SidTime := SiderealTime0(UT);
  SidTime := SidTime + (1.00273790935 * (InstantG)) ;
  if(Sidtime > 24)then SidTime := SidTime - 24.0000;
{Sidtime = LST At Greenwich, now convert to our LST}
  SidTime := SidTime - (C_Long/15.0000);
  if(Sidtime < 0)then SidTime := SidTime + 24.0000;
  SidTimeFract := SidTime - InstantHere ;
  Result := SidTime;
End;
Function GetStime : Real ;
Var
  TempReal : real;
Begin
  TempReal := Frac(Now)* 24.0000 ;
  Result := SidTimeFract + TempReal;
  If(Result > 24.0)Then Result := Result -24.0 ;
End;
Function BumpNorth : Boolean;
Begin
BumpNorth := True;
End;
Function BumpSouth : Boolean;
Begin
BumpSouth := True;
End;



Function Init : boolean;
Var
  TempTime : Real;
  TempStr,cmdstr : String ;
Begin
    MovingRa := False;
    MovingDec := False;
    Parkit := False;
    cmdstr := 'AA ST; ' ;
    SendString(CommRecord,pchar(CmdStr));
    Coord(pndomem^).RA := SetStime; Coord(pndomem^).Dec := c_lat  ;
    Coord(pndomem^).RAGoTO := 0.0; Coord(pndomem^).DecGoTO := 0.0 ;
    Coord(pndomem^).RASync := SetStime; Coord(pndomem^).DecSync :=  c_lat ;
    HalfSecondCounter := 0 ;
    CommandBuffer := '';
    XState := Tracking;
    YState := Tracking;                       
    TempTime := SetStime;
    Str(TempTime :7:4,TempStr);
    MainForm.CmdBx.Items.Insert(0,TempStr);          //write lst
    SendString(CommRecord,'RS;');
    Sleep(5000);
    SendString(CommRecord,'EF; IO0,0; AX; DBI; LP0; SC; AY; DBI; LP0; SC;');
    sleep(100);
//
    Tempstr := (yvlslew);
    Cmdstr := 'AY VL'  + TempStr + ';' ;
    SendString(CommRecord,pchar(CmdStr));
    Tempstr := (Yac);
    Cmdstr := 'AY AC'  + TempStr + ';' ;
    SendString(CommRecord,pchar(CmdStr));
//
    Tempstr := (xvlslew);
    Cmdstr := 'AX VL'  + TempStr + ';' ;
    SendString(CommRecord,pchar(CmdStr));
    Tempstr := (xac);
    Cmdstr := 'AX AC'  + TempStr + ';' ;
    SendString(CommRecord,pchar(CmdStr));
//
    Str(TrkRate:10:6,TempStr);
    sleep(100);
    CmdStr := 'AX JF' + TempStr +';';
    SendString(CommRecord,pchar(CmdStr));
    Result := True;
End;
end.

