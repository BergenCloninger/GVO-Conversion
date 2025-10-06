unit ManualPadle;

interface
uses utils, sysutils,windows,dialogs;
procedure HandleFastPadle;
implementation
uses Main, OMS68SERMC,qgraphics ;

procedure HandleFastPadle;
Var
 CmdStr,TempStr,xspeed,yspeed : String;
Begin
  if(NOT MainForm.Select5Inch.Checked) then
  Begin
    xspeed := xvlSlew;    //high rate
    yspeed := yvlSlew;     //high rate
  End
  Else
  Begin
    xspeed := xvl5inch ;  //5 inch guide
    yspeed := yvl5inch;     //5 inch guide
  End;
//
  If(EastPushed)Then MainForm.Label7.Color := clred else MainForm.Label7.Color := clsilver;
  If(WestPushed)Then MainForm.Label8.Color := clred else MainForm.Label8.Color := clsilver;
  If(NorthPushed)Then MainForm.label5.Color := clred else MainForm.label5.Color := clsilver;
  If(SouthPushed)Then MainForm.label6.Color := clred else MainForm.label6.Color := clsilver;
//  If(manualPushed)Then MainForm.Color := clyellow else MainForm.Color := clBtnFace;


{X Axis (RA) Update}
  Case Xstate of
     Off :        Begin

                  End;
     Tracking :   Begin
                    If(EastPushed)Then
                    Begin
                       Xstate := CorrectingE ;
                       CmdStr := 'AX ST; AX JG-'+ XSpeed +';';
                       SendString(CommRecord,pchar(CmdStr));
                    End;
{}
                    If(WestPushed)Then
                    Begin
                       Xstate := CorrectingW ;
                       CmdStr := 'AX ST; AX JG'+ XSpeed +';';
                       SendString(CommRecord,pchar(CmdStr));
                    End;
{}
                  End;
     CorrectingE : Begin
                    If((NOT EastPushed) AND (NOT WestPushed))Then
                    Begin
                       Xstate := Tracking ;
                       CmdStr := 'AX ST ;' ;
                       SendString(CommRecord,pchar(CmdStr));
                       Str(TrkRate:10:6,TempStr);
                       CmdStr := 'AX JF' + TempStr +';';
                       SendString(CommRecord,pchar(CmdStr));
                    End;
                    If((EastPushed)AND(WestPushed))Then
                    Begin
                      XState := CorrectingE;
                    End;
                  End;
     CorrectingW : Begin
                    If((NOT EastPushed) AND (NOT WestPushed))Then
                    Begin
                       Xstate := Tracking ;
                       CmdStr := 'AX ST;' ;
                       SendString(CommRecord,pchar(CmdStr));
                       Str(TrkRate:10:6,TempStr);
                       CmdStr := 'AX JF' + TempStr +';';
                       SendString(CommRecord,pchar(CmdStr));
                    End;
                    If((EastPushed)AND(WestPushed))Then
                    Begin
                      XState := CorrectingW;
                    End;
{}
                  End;
     Slewing :    Begin
                    If((NOT EastPushed)AND(NOT WestPushed))Then
                    Begin
                      XState := Tracking;
                      Str(TrkRate:10:6,TempStr);
                      CmdStr := 'AX ST;';
                      SendString(CommRecord,pchar(CmdStr));
                      CmdStr := 'AX JF'+ TempStr +';';
                      SendString(CommRecord,pchar(CmdStr));
                    End;
                  End;
  Else  ;
  End; {X State case}
{}
{Y Axis (dec) Update}
  Case Ystate of
            Off : Begin
                  End;
       Tracking : Begin
                    If(NorthPushed)Then
                    Begin
                      if((Ypole>=0)AND(LastDecNorth))Then
                        BumpSouth
                        else
                        If(NOT LastDecNorth)Then BumpNorth;
                      Ystate := CorrectingN;
                      CmdStr := ' AY JG' + ySpeed +';';
                      SendString(CommRecord,pchar(CmdStr));
                     End;
{}
                    If(SouthPushed)Then
                    Begin
                      if((Ypole>=0)AND(Not LastDecNorth))Then
                        BumpNorth
                        else
                        If(LastDecNorth)Then BumpSouth;
                      Ystate := CorrectingS;
                      CmdStr := ' AY JG-' + ySpeed +';';
                      SendString(CommRecord,pchar(CmdStr));
                     End;
                   End;
       CorrectingN:Begin
                        If((NOT NorthPushed)AND(NOT SouthPushed))Then
                        Begin
                          Ystate := Tracking;
                          CmdStr := ' AY ST;';
                          SendString(CommRecord,pchar(CmdStr));
                        End;
                        If(( NorthPushed)AND( SouthPushed))Then
                        Begin
                          Ystate := Slewing  ;
                        End;
                    End;
       CorrectingS:Begin
                        If((NOT NorthPushed)AND(NOT SouthPushed))Then
                        Begin
                          Ystate := Tracking;
                          CmdStr := ' AY ST;';
                          SendString(CommRecord,pchar(CmdStr));
                        End;
                        If(( NorthPushed)AND( SouthPushed))Then
                        Begin
                          Ystate := Slewing ;
                        End;
                    End;
{}
           Slewing :Begin
                        If((NOT NorthPushed)AND(NOT SouthPushed))Then
                        Begin
                          Ystate := Tracking;
                          CmdStr := ' AY ST; ';
                          SendString(CommRecord,pchar(CmdStr));
                        End;
                     End;
  End; {Y State Case}


End;




end.
