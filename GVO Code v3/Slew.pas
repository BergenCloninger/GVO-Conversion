unit Slew;

interface
uses utils, sysutils,windows,dialogs;

Procedure SlewScope;

implementation
uses Main,quad1to1,quad1to3,quad3to3,quad3to1,
     quad1to2, quad2to1, quad2to2,GetQandYU ;

Procedure SlewScope;
label endit;
var
 i:integer;
 Alt,HA,Xcount,Ycount : real;
 thandle : HWND;
 Begin
   thandle := 0;
   mainform.sjog.enabled := False;
   mainform.njog.enabled := False;
   mainform.ejog.enabled := False;
   mainform.wjog.enabled := False;
   RaTarget := Coord(pndomem^).RAGoTo ;
   DecTarget := Coord(pndomem^).DECGoTo ;
   GetQandY(RaTarget,DecTarget,Alt,HA,Xcount,Ycount,TargetQuadrant,i);
   if(Alt < 20.0) then
   begin
      Coord(pndomem^).RAGoTo := 0.0   ;
      Coord(pndomem^).decGoTo := 0.0  ;
      windows.MessageBox(thandle,'No slew below horizon','Telescope Control',MB_SYSTEMMODAL);
      goto endit;
   End;
   if(i= 0) then
   begin
      Coord(pndomem^).RAGoTo := 0.0   ;
      Coord(pndomem^).decGoTo := 0.0  ;
      windows.MessageBox(thandle,'No slew below horizon','Telescope Control',MB_SYSTEMMODAL);
     goto endit;
   End;

//
   If((TargetQuadrant = 1) And (Quadrant = 1) ) Then    GoQuad1to1(Xcount,Ycount)
   else If((TargetQuadrant = 3) And (Quadrant = 1) ) Then    GoQuad1to3(Xcount,Ycount)
   else If((TargetQuadrant = 3) And (Quadrant = 3) ) Then    GoQuad1to3(Xcount,Ycount)
   else If((TargetQuadrant = 1) And (Quadrant = 3) ) Then    GoQuad3to1(Xcount,Ycount)
   else
     begin
      Coord(pndomem^).RAGoTo := 0.0   ;
      Coord(pndomem^).decGoTo := 0.0  ;
      windows.MessageBox(thandle,'Slew below pole not yet implemented','Telescope Control',MB_SYSTEMMODAL);

     end;
endit:

//   If((TargetQuadrant = 4) And (Quadrant = 3) ) Then    GoQuad3to4 ;
//   If((TargetQuadrant = 3) And (Quadrant = 4) ) Then    GoQuad4to3 ;
//   If((TargetQuadrant = 4) And (Quadrant = 4) ) Then    GoQuad4to4 ;
//   If((TargetQuadrant = 1) And (Quadrant = 2) ) Then    GoQuad1to1 ;
//   If((TargetQuadrant = 2) And (Quadrant = 2) ) Then    GoQuad1to1 ;
//   If((TargetQuadrant = 2) And (Quadrant = 1) ) Then    GoQuad1to1 ;


End;
end.
