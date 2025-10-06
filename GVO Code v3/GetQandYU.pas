unit GetQandYU;
//
// updated for gvo fork mount    6/1/08
//
interface
Procedure GetQandY(ra,dec:Real; var Alt,HA,xcount,ycount:Real ;var q,y : integer);
implementation

uses Main, Utils,Math;
Procedure GetQandY(ra,dec:Real; var Alt,HA,xcount,ycount:Real ;var q,y : integer);
//
//get q and y gets quadrant,ha,alt,xcount,ycount and ypole of current ra,dec
//

Begin
MainForm.Timer_Main.Enabled := false;   //don't allow other calls
  Meridian := GetStime;
  EastHor := Meridian + 6.0;        //ha at east horizon
  WestHor := Meridian - 6.0;        //ha at west horizon
  If(WestHor < 0.0) Then WestHor := WestHor + 24.0 ;
  If(EastHor > 24.0) then EastHor  := EastHor - 24.0 ;
  EastHA := -6;
  NorthHA := 12;
  WestHA := 6;
  SouthHA := 0;
//
//Get Altitude
//
  HA := 15*(Meridian -  Ra);
  Alt := ArcSin((Sin(DToR*Dec)*sin(DToR*c_Lat)) + (cos(DToR*dec)*cos(DToR*HA)*cos(DToR*C_Lat)) );
  Alt := RToD * Alt ;   //A = -90 to +90  degrees Altitude
//
//get Ypole
//
  HA := (Meridian - Ra);
  If(HA < -12) Then HA := HA + 24;
  if(HA > 12) Then HA := HA - 24;
  if((HA <=  SouthHA ) AND (HA >= EastHA)) Then  Begin Y := 1; Q := 1 End
  Else if((HA < EastHA) AND (HA > -NorthHA)) Then  Begin Y := -1; Q := 2 End
  Else if((HA >= SouthHA ) AND (HA <= WestHA)) Then  Begin Y := 1; Q := 3  End
  Else if((HA > WestHA ) AND (HA < NorthHA)) Then  Begin Y := -1; Q := 4 End ;
//
// get position counts  -  Dec First
// dec stepper counts positive when north of zenith(> lat), neg when south of zenith (< lat)
//
  if( dec >= c_lat) then
  begin   //counts positive
    ycount := (dec - c_lat)* DecFact;
  end
  else
  begin
    ycount := -1*((c_lat - dec) * DecFact);
  end;
//
// Get Ra Counts
// east of meridian is neg counts     (-ha)
// west of meridian is positive counts    (+ha)
//
   xcount :=  (HA * 15)  * RaFact;

//from german equatorial model
//   if((Q=1)) Then  Xcount := ( (6.0 - abs(TempHa)) * 15)* RaFact
//   Else if((Q=2)) Then  Xcount := ( (abs(TempHa)-6.0) * 15)* RaFact
//   Else if((Q=3)) Then  Xcount := ( (abs(TempHa) - 6.0) * 15)* RaFact
//   Else if((Q=4)) Then  Xcount := ( (6.0 - abs(TempHa)) * 15)* RaFact ;
//   if((q = 4) OR (q = 2)) Then  Xcount := Xcount * -1.0 ;
   MainForm.Timer_Main.Enabled := True;
   End;
end.
