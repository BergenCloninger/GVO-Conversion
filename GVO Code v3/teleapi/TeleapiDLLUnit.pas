unit TeleapiDLLUnit;

interface
Uses
  Windows,Sysutils,Dialogs ;
{}
type TeleAPICallBack  = procedure( i : integer ) of object;

Function  DllMain(Reason:Dword) : Bool ; stdcall;
Function  tapiGetDLLVersion : Integer; stdcall;
Function  tapiEstablishLink : Integer; stdcall;
Function  tapiTerminateLink : Integer; stdcall;
Function  tapiGetRaDec(var Ra,Dec : real) : Integer; stdcall;
Function  tapiSetRaDec(Ra,Dec : Real) : Integer; stdcall;
Function  tapiGotoRaDec(Ra,Dec : Real) : Integer; stdcall;
Function  tapiIsGotoComplete(Var Complete:Boolean) : Integer; stdcall;
Function  tapiAbortGoto : Integer; stdcall;
Function  tapiSettings : Integer; stdcall;
Function  tapiPulseFocuser(Fast,Inward : WordBool) : Integer; stdcall;
procedure SetCallBack( CallBackAdd: TeleAPICallBack ); stdcall;

{GVO Observatory custom DLL Shared memory functions}
Function  GetNdoMem : Pointer; stdcall;
{}
Exports
  DllMain            index 19   name 'DllMain' ,
  tapiGetDLLVersion  index 20   name 'tapiGetDLLVersion',
  tapiEstablishLink  index 21   name 'tapiEstablishLink', {}
  tapiTerminateLink  index 22   name 'tapiTerminateLink', {}
  tapiGetRaDec       index 23   name 'tapiGetRaDec', {}
  tapiSetRaDec       index 24   name 'tapiSetRaDec',
  tapiGotoRaDec      index 25   name 'tapiGotoRaDec', {}
  tapiIsGotoComplete index 26   name 'tapiIsGotoComplete',
  tapiAbortGoto      index 27   name 'tapiAbortGoto',
  tapiSettings       index 28   name 'tapiSettings',
  tapiPulseFocuser   index 29   name 'tapiPulseFocuser',
  GetNdoMem          index 40   name 'GetNdoMem',
  SetCallBack        index 41   name 'SetCallBack';
implementation

Procedure MapMemory(dwallocsize : dword); Forward ;    {Allocate Memory}
Procedure UnMapMemory;    Forward ;     {Release Memory}

Const
  key:string = '_ndomem';
  maxsize = $1000;
Type
  Coord = Record
    RA : Real ;
    DEC : Real ;
    RAGoTO : Real ;
    DECGoTO : Real ;
    RASync : Real;
    DecSync : Real;
  End;
Var
  hobjhand : Thandle;
  pndomem : pointer;
  CallBack : TeleAPICallBack;
(* ----------------------------------------SKY Standard Functions-------------*)
Function DllMain(Reason:Dword):Bool ;
Begin
  Case Reason of
     dll_process_attach:
     Begin
       showmessage('DLL starting, memory initialized');
       pndomem := nil;
       hobjhand := 0;
       mapmemory(maxsize);
     end;
     dll_process_detach:
     Begin
        unmapmemory;
        showmessage('DLL Stopping');
     End;
  end;
  Coord(pndomem^).RAGoTo := 0.0;  Coord(pndomem^).DecGoTo := 0.0 ;
  DllMain := true;
End;

Function  tapiGetDLLVersion;
Begin
  ShowMessage('Version 202');
  tapiGetDLLVersion := 202 ;
End;
Function  tapiEstablishLink;
Begin
//  ShowMessage('Establish Link');
  tapiEstablishLink := 0;
End;
Function  tapiTerminateLink;
Begin
//ShowMessage('Terminate Linkage');
   tapiTerminateLink := 0;
End;
Function  tapiGetRaDec(Var Ra,Dec : real): Integer;
Begin
//  ShowMessage('get ra');
  ra:= Coord(pndomem^).RA ; Dec := Coord(pndomem^).Dec;
  tapiGetRaDec := 0;
End;
Function  tapiSetRaDec(Ra,Dec : Real): Integer;
Begin
  Coord(pndomem^).RASync := ra;  Coord(pndomem^).DecSync := dec ;
  tapiSetRaDec := 0;
End;
Function  tapiGotoRaDec(Ra,Dec : Real) : Integer;
Begin
  Coord(pndomem^).RAGoTo := ra;  Coord(pndomem^).DecGoTo := dec ;
  tapiGotoRaDec := 0;
End;
Function  tapiIsGotoComplete(Var Complete:Boolean) : Integer;
Begin
  tapiIsGotoComplete := 0;
  if( (Coord(pndomem^).RAGoTo = 0.0)  AND (Coord(pndomem^).DecGoTo = 0.0)) Then Complete := True
  Else Complete := False;
End;
Function  tapiAbortGoto;
Begin
  Coord(pndomem^).RAGoTo := 0.0;  Coord(pndomem^).DecGoTo := 0.0;
  tapiAbortGoto := 0;
End;
Function  tapiSettings;
Begin
  ShowMessage('Telescope API Verson 2.02 By GVO Observatory');
  tapiSettings := 0;
End;
Function  tapiPulseFocuser(Fast,Inward : WordBool): Integer;
Begin
//  ShowMessage('Pulse Focus');
  tapiPulseFocuser := 0;
End;
{}
(* -----------------------------North Dallas Observatory Custom Functions-----------------*)
{}
/////////////////////Call back stuff//////////////////////////////
procedure SetCallBack( CallBackAdd: TeleAPICallBack );
  begin
  CallBack := CallBackAdd;
  end;
procedure DoCallBack(i : Integer);
  begin
  if Assigned(CallBack) then CallBack( I );
  end;
Function GetNdoMem : Pointer ;  {Pass Shared Memory Pointer to Control PGM}
Begin
//  showmessage('get mem');
  Result := pndomem;
End;
Procedure UnMapMemory;         {Release Memory}
Begin
  if Assigned(pndomem) then
  begin
    unmapviewoffile(pndomem) ;
    pndomem := nil;
  end;
  if hobjhand > 0 Then closehandle(hobjhand);
  hobjhand := 0 ;
End;
Procedure mapmemory(dwallocsize : dword);     {Allocate Memory}
begin
  hobjhand := createfilemapping(maxdword,nil,page_readwrite,0,dwallocsize,pchar(key));
  if(hobjhand = 0 ) then Raise exception.create('Could not create file mapping object');
  pndomem := mapviewoffile(hobjhand,file_map_write,0,0,1);
  if not assigned(pndomem) then
  begin
    unmapmemory;
    raise exception.create('could not map file');
  end;
end;
Procedure NDOupdateRaDec(RA,DEC : Real) ;    {Get RA,DEC from Control Program}
Begin
  Coord(pndomem^).RA := ra;  Coord(pndomem^).Dec := dec ;
End;


end.
