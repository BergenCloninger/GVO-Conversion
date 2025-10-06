unit Sync;
interface
uses utils, sysutils,windows,dialogs;
procedure SyncScope;
implementation
uses Main,calccoord,GetQandYU, OMS68SERMC ;
//
//   updated for gvo fork mount 6/1/08
//
//    if shared mem 'sync' coordinates are not zero, then load motor controller with
//        proper counts for that ra, dec
//    enter, shared mem contains current coord. dec, ra
//    get q and y gets quadrant,ha,alt,xcount,ycount and ypole of current ra,dec
//
//

procedure SyncScope;
Var
  CmdStr,CmdStr2,PreStrng : String;
  Quad : integer;
  Alt,HA,Xcount,Ycount : real;
Begin
  If((MovingRA) OR (MovingDec)) Then Exit;
  GetQandY(Coord(pndomem^).RASync,Coord(pndomem^).decSync,Alt,HA,xcount,ycount,quad,Ypole);
  if(Alt < 0 ) then
  begin
     showmessage('Sync below Horizon');
     halt;
  end;
  if(Xcount < 0 ) Then PreStrng := 'AX LP-'       {Send RA Position}
                  Else PreStrng := 'AX LP' ;
  Xcount := abs(xcount);
  Str(Xcount:14:0,CmdStr);
  CmdStr := TrimLeft(CmdStr);
  CmdStr := PreStrng + CmdStr +';';
  SendString(CommRecord,pchar(CmdStr));
  Str(TrkRate:10:6,CmdStr2);
  CmdStr :='AX JF'+CmdStr2 +';';     //Back to Tracking
  SendString(CommRecord,pchar(CmdStr));    {RA Back To TRACKING}
//
  if(Ycount < 0 ) Then PreStrng := 'AY LP-'
                   Else PreStrng := 'AY LP' ;
  YCount := abs(Ycount);
  Str(YCount:14:0,CmdStr);
  CmdStr := TrimLeft(CmdStr);
  CmdStr := PreStrng + CmdStr+';' ;
  SendString(CommRecord,pchar(CmdStr));
//
  Coord(pndomem^).RASync := 0.0 ;                     //reset request
  Coord(pndomem^).decSync := 0.0 ;
  UpdateCoord;
End;
end.
