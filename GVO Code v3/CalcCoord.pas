unit CalcCoord;

interface
uses utils, sysutils,windows,math;

Procedure UpdateCoord;

implementation
uses Main,GetQandYU,dialogs, OMS68SERMC ;

Procedure UpdateCoord;  {from stepper controller}
Var
 CmdStr,TempStr1,TempStr2,TempStr3 : String;
 x,y,alt,ha,Xcount,Ycount,TempStime : real;
 thandle : HWND;
Label DoDec;
//
//  read stepper counts
//  x = ra, y = dec
//
Begin {Update coord}    {from stepper controller}
   thandle := 0;
   MainForm.labeltkrate.Caption := floattostr(TrkRate);
   if(MainForm.PanelStatus.Visible = True) Then  MainForm.PanelStatus.Visible := False     //blink active indicator
      Else MainForm.PanelStatus.Visible := True;
//
    {Get Positions, wait for them}
    While (SendAndGetString(CommRecord,'PP;',Response)<> 0) Do ; //get position of all axis
       Begin
         TempList.CommaText := Response;
         x := strtoFloat(TempList[0]);                  // x = ra  counts
//         MainForm.Labelraval.Caption := TempList[0];   //write to Form     ra counts
         MainForm.labeldecval.Caption := TempList[1];   //write to Form    dec counts
       End;
//
    CmdStr := '';
    TempStime := GetStime;
    If(X = 0.0)Then X := 1.0;
//
// Ra Counts
// east of meridian is neg counts     (-ha)
// west of meridian is positive counts    (+ha)
//
    If( ((X > 0.0) AND (Ypole > 0)))Then              //west of meridian, above pole. quad 3
    Begin {quad 3}                              //positive counts
       RANow := (TempStime );                    //     meridian = 0 counts
       RANow := RANow - ((X/RAFact)/15.0) ;
       if(RANow < 0 ) Then RANow := RANow + 24;
//cal hr min, secs
       RAHr :=  Trunc(RANow);
       RAMin :=   Trunc(Frac(RANow) * 60);
       RASec :=   Trunc((Frac(RANow) * 3600)- (RAMin*60)) ;
       Str(RAHr:2,TempStr1) ;
// display ra
       MainForm.Rahredit.Text := TempStr1;
       Str(RAMin:2,TempStr2);
       MainForm.Raminedit.Text := TempStr2;
       Str(RAsec:2,TempStr3) ;
       MainForm.Rasecedit.Text := TempStr3;
       GoTo DoDec;
    End;{quad 3}
    If( (X < 0.0) AND (Ypole < 0) )Then
    Begin {Quad 2}
       X := Abs(X);
       RANow := (TempStime);
       RANow := RANow + ((X/RAFact)/15.0) ;
       if(RANow > 24 ) Then RANow := RANow - 24;
//calc hr, min, sec
       RAHr :=  Trunc(RANow);
       RAMin :=   Trunc(Frac(RANow) * 60);
       RASec :=   Trunc((Frac(RANow) * 3600)- (RAMin*60)) ;
// display ra
       Str(RAHr:2,TempStr1) ;
       MainForm.Rahredit.Text := TempStr1;
       Str(RAMin:2,TempStr2);
       MainForm.Raminedit.Text := TempStr2;
       Str(RAsec:2,TempStr3) ;
       MainForm.Rasecedit.Text := TempStr3;
       GoTo DoDec;
    End; {Quad 2}
    If( (X < 0.0) AND (Ypole > 0) )Then                    //east of meridian, above pole, quad 1
    Begin {Quad 1}
       X := Abs(X);
       RANow := (TempStime);
       RANow := RANow + ((X/RAFact)/15.0) ;
       if(RANow > 24 ) Then RANow := RANow - 24;
//calc hr, min, sec
       RAHr :=  Trunc(RANow);
       RAMin :=   Trunc(Frac(RANow) * 60);
       RASec :=   Trunc((Frac(RANow) * 3600)- (RAMin*60)) ;
// display ra
       Str(RAHr:2,TempStr1) ;
       MainForm.Rahredit.Text := TempStr1;
       Str(RAMin:2,TempStr2);
       MainForm.Raminedit.Text := TempStr2;
       Str(RAsec:2,TempStr3) ;
       MainForm.Rasecedit.Text := TempStr3;
       GoTo DoDec;
    End; {Quad 1}
    If( ((X > 0.0) AND (Ypole < 0)))Then
    Begin {quad 4}
       RANow := (TempStime );                    //     meridian = 0 counts
       RANow := RANow - ((X/RAFact)/15.0) ;
       if(RANow < 0 ) Then RANow := RANow + 24;
//cal hr min, secs
       RAHr :=  Trunc(RANow);
       RAMin :=   Trunc(Frac(RANow) * 60);
       RASec :=   Trunc((Frac(RANow) * 3600)- (RAMin*60)) ;
       Str(RAHr:2,TempStr1) ;
// display ra
       MainForm.Rahredit.Text := TempStr1;
       Str(RAMin:2,TempStr2);
       MainForm.Raminedit.Text := TempStr2;
       Str(RAsec:2,TempStr3) ;
       MainForm.Rasecedit.Text := TempStr3;
       GoTo DoDec;
    End;{quad 4}
//***********************{Do Dec Now} *************************************

//
//dec stepper counts positive when north of zenith, neg when south of zenith
//
DoDEc:
    CmdStr := '';
    Y := strtoFloat(TempList[1]);          //y = counts
    DECNow:= C_Lat  + (Y/DECFact);
//calc deg, min, sec
    decdeg :=  Trunc(decNow);
    decMin :=   Trunc(Frac(decNow) * 60);
    decSec :=   Trunc((Frac(decNow) * 3600)- (decMin*60)) ;
//display dec
    Str(decdeg:2,TempStr1) ;
    MainForm.decdegedit.Text := TempStr1;
    Str(decMin:2,TempStr2);
    MainForm.decminedit.Text := TempStr2;
    Str(decsec:2,TempStr3) ;
    MainForm.decsecedit.Text := TempStr3;
//**************{Set Quadrant Now}***********************************************
    Coord(pndomem^).RA := RANow; Coord(pndomem^).Dec := DecNow ;
    GetQandY(RANow,DecNow,Alt,HA,xcount,YCount,Quadrant,Ypole);
//check for below horizon (20 degrees)    
    if(alt <20) then
    begin
      cmdstr := 'AA ST;' ;
      SendString(CommRecord,pchar(CmdStr));
      MainForm.ButtonStop.Caption := 'START';
      Coord(pndomem^).RAGoTo := 0.0   ;
      Coord(pndomem^).decGoTo := 0.0  ;
      windows.MessageBox(thandle,'No track below horizon','Telescope Control',MB_SYSTEMMODAL);
    end;
//display alt, azimuth
    altdeg :=  Trunc(alt);
    altmin :=   Trunc(Frac(alt) * 60);
    altsec :=   Trunc((Frac(alt) * 3600)- (altmin*60)) ;
    Str(altdeg:2,TempStr1) ;
    MainForm.editaltdeg.Text := TempStr1;
    Str(altmin:2,TempStr2);
    MainForm.editaltmin.Text := TempStr2;
    Str(altsec:2,TempStr3) ;
    MainForm.editaltsec.Text := TempStr3;
//
    azimuth :=  (- sin(DECNow)) / cos(c_lat);
    if (azimuth < -1) then azimuth := -1;
    if (azimuth > 1) then azimuth := 1;
    azimuth := radtodeg(ArcCos( azimuth ));
    
    azdeg :=  Trunc(azimuth);
    azmin :=   Trunc(Frac(azimuth) * 60);
    azsec :=   Trunc((Frac(azimuth) * 3600)- (azmin*60)) ;
    Str(azdeg:2,TempStr1) ;
    MainForm.editazdeg.Text := TempStr1;
    Str(azmin:2,TempStr2);
    MainForm.editazmin.Text := TempStr2;
    Str(azsec:2,TempStr3) ;

    MainForm.Refresh;
{}
End; {Update Coord}
end.
