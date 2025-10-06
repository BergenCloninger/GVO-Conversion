unit Main;
//  ************************
//  ** Version 2.0 started 5/29/08
//  ** version 3.0 8/1/2009   added fractional traking rate
//  ************************************
interface

uses
  SysUtils, WinTypes, WinProcs, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, ExtCtrls, StdCtrls, Spin, ComCtrls,Inifiles,dbpwdlg;


type
  Coord = Record
  RA : Real;
  Dec : Real;
  RAGoTo : Real;
  DecGoto : Real;
  RASync : Real;
  DecSync : Real ;
  End;
  type TeleAPICallBack  = procedure( i : integer ) of object;
   StateVar = (Off, Tracking,CorrectingE, CorrectingW,CorrectingN,CorrectingS,
              Slewing) ;
  TMainForm = class(TForm)
    CmdEdit: TEdit;
    cmdbx: TListBox;
    Label2: TLabel;
    Timer_Main: TTimer;
    InitializeButton: TButton;
    Panel1: TPanel;
    NJog: TButton;
    SJog: TButton;
    EJog: TButton;
    WJog: TButton;
    Panel2: TPanel;
    SlewSelect: TRadioButton;
    RadioButton1: TRadioButton;
    Label1: TLabel;
    rahredit: TEdit;
    raminedit: TEdit;
    rasecedit: TEdit;
    decdegedit: TEdit;
    decminedit: TEdit;
    decsecedit: TEdit;
    Timer_Padle: TTimer;
    Label5: TLabel;
    Label6: TLabel;
    Label8: TLabel;
    Quad4: TLabel;
    Quad1: TLabel;
    Quad2: TLabel;
    Quad3: TLabel;
    Label7: TLabel;
    YPolarity: TLabel;
    ButtonStop: TButton;
    Select5Inch: TRadioButton;
    ButtonPark: TButton;
    PanelStatus: TPanel;
    Label9: TLabel;
    editaltdeg: TEdit;
    editaltmin: TEdit;
    editaltsec: TEdit;
    Altitude: TPanel;
    Label10: TLabel;
    editazdeg: TEdit;
    editazmin: TEdit;
    editazsec: TEdit;
    Azmuth: TPanel;
    Labeldecval: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    BaseEdit: TEdit;
    Labeltkrate: TLabel;

    procedure CmdEditClick(Sender: TObject);
    procedure Timer_MainTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure NJogMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure NJogMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure InitializeButtonClick(Sender: TObject);
    procedure SJogMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure SJogMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure EJogMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure EJogMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure WJogMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure WJogMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Timer_PadleTimer(Sender: TObject);
    procedure ButtonStopClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure ButtonParkClick(Sender: TObject);
    procedure CmdEditEnter(Sender: TObject);
    procedure BaseEditDblClick(Sender: TObject);
  private
    Procedure DLLWork(I:integer);
    { Private declarations }
  public
    { Public declarations }
  end;
  Procedure setcallback( DLLWork: TeleAPICallBack); stdcall ; external
       'C:\Program Files\Common Files\System\teleapi.dll' Index 41;
  Function getndomem : Pointer ; stdcall;  external
      'C:\Program Files\Common Files\System\teleapi.dll' Index 40;
(*
//GVO
//*******************************************************************************
RA
tracking rate -       SID=1.00273790935  solar time / sidereal time

gear ration = 480/1
        1 rev of worm = 360/480 = .75 degrees of sky  = 2700  ArcSec  of sky

        stepper = 5,000 usteps /rev X 18 gear reduction =  90,000 usteps/rev of worm
        1 rev of worm  2700 arc sec sky
        1 ustep of worm  = .03  arc sec of sky(2700/90,000)  = 33.3333333   usteps/arc sec
        1 ustep of worm = .046875 arc sec sky       = 21.3333 usteps / arc sec
        33.3333333  usteps/arc sec   =  120,000 usteps/deg

tracking = 14.959043494948 arcsec/sec = 498.635 usteps/sec  trackking rate

    RA FACT (steps/deg) = 120,000
//********************************************************************************************
DEC
    
    gear ration = 338/1
    1 rev of worm = 360/338 = 1.0650887573964497 degrees of sky  = 3834.31952  ArcSec  of sky

    stepper = 50,000 usteps/rev of worm
    1 rev of worm  3834.31952  arc sec sky
    1 ustep of worm  = .0766863905 arc sec of sky(3834.31952/50000)  = 13.040123456   usteps/arc sec
    13.040123456  usteps/arc sec   =  46944.4444 usteps/deg
    Dec fact (steps/deg) = 46944.4444


*)


Const
  EastofMeridian  : Boolean = True;          //Object east of Meridian
  LookingEast  : Boolean = True;            //Mount looking east
  TargetEastOfMeridian  : Boolean = True;
  LastDecNorth : Boolean = True;
// quadrant = 1 when east of meridian and above the pole
// quadrant = 2 when east of meridian and Below the pole
// quadrant = 3 when west of meridian and above the pole
// quadrant = 4 when west of meridian and Below the pole
//
// when below pole "EastofMeridian" is opposite of above the pole!!!!
//
//Also OMS board uses IRQ 5
  Data_reg  = $300;
  Done_reg  = $301;
  Control_reg = $302;
  Status_reg = $303;

  Cmderr_bit = $01;
  Init_Bit  = $02;
  Enc_bit = $04;
  Ovrt_bit = $08;
  Done_bit = $10;
  Ibf_bit = $20;
  Tbe_bit = $40;
  Irq_bit = $80;

  EastBit = $04;      //Lowbyte    term 13    i/o 2
  WestBit = $08;      //Lowbyte    term 34    i/o 3
  NorthBit = $01;    //Lowbyte     term 11    i/o 0
  SouthBit = $02;    //High Byte   term 57    i/o 5
  ManualBit = $01;   //high byte   term 36    i/o 4
Var                                                        //         high     low
  TrkRate  : real  ;                                      //       8  4  2  1   8  4  2  1
  xvlslew : string   ;     //computer slew                         x  x  x  x   x  x  x  x
  yvlslew : string   ;    //computer slew                                S  M   W  E     N
  xvl5inch : string   ;     //5 inch auto guide                    7  6  5  4   3  2  1  0
  yvl5inch : string   ;    //5 inc auto guide                      59 58 57 36  34 13 12 11`
  xvl : string  ;          //tracking adjustment
  xac,xacmax,xvlmax : string  ;
  yvl : string  ;          //tracking adjustment
  yac, yacmax,yvlmax : string  ;
  RAFact   : real ;
  DecFact  : real ;
  DecBack : Integer ;    //backlash
  C_Lat   : real  ;
  C_Long  : real  ;

var
  Keystroke,pcxdataout,status: byte;
  Name,commandbuffer : string;
  pndomem : Pointer;
  HalfSecondCounter : Byte;
  Quadrant,TargetQuadrant : Integer ;
  MovingRA : Boolean ;
  MovingDEC : Boolean ;
  Ypole : integer ;     // neg when Object Below the Pole
var
  MainForm: TMainForm;
  XState,Ystate : StateVar;
  io : byte;
  EastPushed,WestPushed,NorthPushed,SouthPushed,ManualPushed : boolean ;
  SidTimeFract,SidTime,RANow,DECNow,azimuth : Real;
  RAHr,RAMin,RASec : Integer;
  DECDeg,DECMin,DECSec,altdeg,altmin,altsec,azdeg,azmin,azsec : Integer;
  RaTarget,DecTarget,Meridian,EastHor,WestHor,
  EastHA,WestHA,NorthHA,SouthHA,A,H : real;
  RaPos, decPos,tdecfact  : real;
  Parkit,NorthofZenith,NoPassword : Boolean;
implementation

uses TimerUnit, Utils, OMS68SERMC;

{$R *.DFM}
//local subs
procedure loadparms();
begin
  tdecfact := ((360/338)* 3600) / 50000  ;
  tdecfact := 1 / tdecfact ;
  tdecfact := tdecfact * 3600;
  TrkRate  := strtoFloat(PFCIniFile.ReadString( 'Main','TrkRate', '500.6')) ;
  MainForm.BaseEdit.text   := FloatToStr(Trkrate);

  xvlslew  := PFCIniFile.ReadString( 'Main','xvlslew','75000') ;     //computer slew
  yvlslew  := PFCIniFile.ReadString( 'Main','yvlslew','50000')  ;    //computer slew
  xvl5inch  := PFCIniFile.ReadString( 'Main','xvl5inch','2000') ;     //5 inch auto guide
  yvl5inch  := PFCIniFile.ReadString( 'Main','yvl5inch','2000')  ;    //5 inch auto guide
  xvl      := PFCIniFile.ReadString( 'Main','xvl','10000');
  xac      := PFCIniFile.ReadString( 'Main','xac','35000');
  yvl      := PFCIniFile.ReadString( 'Main','yvl','5000');
  yac      := PFCIniFile.ReadString( 'Main','yac', '25000');
  YacMax   := PFCIniFile.ReadString( 'Main','yacmax', '25000');
  YvlMax   := PFCIniFile.ReadString( 'Main','yvlmax', '100000');
  XacMax   := PFCIniFile.ReadString( 'Main','xacmax', '35000');
  XvlMax   := PFCIniFile.ReadString( 'Main','xvlmax', '75000');


  RAFact   := strtofloat(PFCIniFile.ReadString( 'Main','RAFact', '120000'));
  DecFact  := strtofloat(PFCIniFile.ReadString( 'Main','DecFact', FloatToStr(tdecfact)));
  DecBack  := strtoint(PFCIniFile.ReadString( 'Main','DecBack','30'));
  C_Lat   := strtofloat(PFCIniFile.ReadString( 'Main','c_Lat','38.508741')) ;     {gvo 38 30 31.47}
  C_Long  := strtofloat(PFCIniFile.ReadString( 'Main','C_Long', '106.9385583')) ;     {gvo 106 56 18.93}
//
  PFCIniFile.WriteString( 'Main','TrkRate',floatToStr(TrkRate)) ;     //computer slew
  PFCIniFile.WriteString( 'Main','xvlslew',xvlslew) ;     //computer slew
  PFCIniFile.WriteString( 'Main','yvlslew',yvlslew)  ;    //computer slew
  PFCIniFile.WriteString( 'Main','xvl5inch',xvl5inch) ;     //5 inch auto guide
  PFCIniFile.WriteString( 'Main','yvl5inch',yvl5inch)  ;    //5 inch auto guide
  PFCIniFile.WriteString( 'Main','xvl',xvl);
  PFCIniFile.WriteString( 'Main','xac',xac);
  PFCIniFile.WriteString( 'Main','yvl',yvl);
  PFCIniFile.WriteString( 'Main','yac', yac);
  PFCIniFile.WriteString( 'Main','yacmax', YacMax);
  PFCIniFile.WriteString( 'Main','yvlmax', YvlMax);
  PFCIniFile.WriteString( 'Main','xacmax', xacMax);
  PFCIniFile.WriteString( 'Main','xvlmax', xvlMax);
  PFCIniFile.WriteString( 'Main','RAFact',  floattostr(RAfact));
  PFCIniFile.WriteString( 'Main','DecFact',floattostr(DecFact));
  PFCIniFile.WriteString( 'Main','DecBack', Inttostr(DecBack));
  PFCIniFile.WriteString( 'Main','C_Lat',  floattostr(C_Lat)) ;     {gvo 38 30 31.47}
  PFCIniFile.WriteString( 'Main','C_Long', floattostr(C_Long)) ;      {gvo 106 56 18.93}
  ///
end;



////
Procedure TMainForm.DLLWork(I:integer);
Begin
// {Entry Point For DLL Callback}
End;

procedure TMainForm.Timer_MainTimer(Sender: TObject);
begin
  TimerUpdate;
end;

procedure TMainForm.cmdeditClick(Sender: TObject);
begin
(*  Timer_Main.enabled := False;
  sleep(1000);
  MainForm.cmdBx.Items.Insert (0,cmdEdit.Text);
  SendAndGetString(CommRecord,pchar(cmdEdit.text +';'),Response) ;
  Tempstr :=  Response;
  MainForm.cmdbx.items.insert(0,Tempstr);   //write to cmd box
  cmdEdit.text := '';
  Timer_Main.enabled := True;   *)
//
end;

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
VAR
  cmdstr : string;
begin
  cmdstr := 'AA ST;' ;
SendString(CommRecord,pchar(CmdStr));
end;

procedure TMainForm.NJogMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr, TempStr : String;
  i : integer;
begin
  MainForm.Timer_Main.enabled := False;
  if(NOT LastDecNorth)Then BumpNorth;
  If(SlewSelect.Checked = True)Then
     Begin
       If( Ypole >=0 ) Then CmdStr := 'AY JG+'+ yvl +';' Else  CmdStr := 'AY JG-'+ yvl +';' ;
       SendString(CommRecord,pchar(CmdStr));
     End
     Else
     Begin
       i := Round(TrkRate/4);
       Str(i,TempStr);
       If( Ypole >= 0 ) Then CmdStr := 'AY JG'+ TempStr +';' Else CmdStr := 'AY JG-'+ TempStr+';'    ;
       SendString(CommRecord,pchar(CmdStr));
     End ;
     MainForm.Timer_Main.enabled := True;
end;

procedure TMainForm.NJogMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr: String;
begin
  MainForm.Timer_Main.enabled := False;
  CmdStr := 'AY ST; ';
  SendString(CommRecord,pchar(CmdStr));
  MainForm.Timer_Main.enabled := True;
end;


procedure TMainForm.SJogMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr, TempStr : String;
  i : integer;
begin
  MainForm.Timer_Main.enabled := False;
  if(LastDecNorth)Then BumpSouth;
  If(SlewSelect.Checked = True)Then
     Begin
       If( Ypole >= 0 ) Then CmdStr := 'AY JG-'+ yvl +';' Else CmdStr := 'AY JG'+ yvl +';';
       SendString(CommRecord,pchar(CmdStr));
     End
     Else
     Begin
       i := Round(TrkRate/4);
       Str(i,TempStr);
       If(Ypole >= 0)Then CmdStr := 'AY JG-'+ TempStr +';' Else CmdStr := 'AY JG'+ TempStr +';' ;
       SendString(CommRecord,pchar(CmdStr));
     End ;
   MainForm.Timer_Main.enabled := True;
end;

procedure TMainForm.SJogMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr : String;
begin
  MainForm.Timer_Main.enabled := False;
  CmdStr := 'AY ST; ';
  SendString(CommRecord,pchar(CmdStr));
  MainForm.Timer_Main.enabled := True;
end;

procedure TMainForm.EJogMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr, TempStr : String;
  i : integer;
begin
  MainForm.Timer_Main.enabled := False;
  If(SlewSelect.Checked = True)Then
     Begin
       CmdStr := 'AX jg0; AX jg-'+ xvl +';' ;
       SendString(CommRecord,pchar(CmdStr));
     End
     Else
     Begin
       i := Round(TrkRate/2);
       Str(i,TempStr);
       CmdStr := 'AX jg'+ TempStr +';' ;
       SendString(CommRecord,pchar(CmdStr));
     End ;
end;

procedure TMainForm.EJogMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr, TempStr : String;
begin
  MainForm.Timer_Main.enabled := False;
  Str(TrkRate,TempStr);
  CmdStr := 'AX ST; jg'+ TempStr +';';
  SendString(CommRecord,pchar(CmdStr));
  MainForm.Timer_Main.enabled := True;
end;

procedure TMainForm.WJogMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr, TempStr : String;
  i: integer;

begin
  MainForm.Timer_Main.enabled := False;
  If(SlewSelect.Checked = True)Then
     Begin
       CmdStr := 'AX jg0; jg'+ xvl +';';
       SendString(CommRecord,pchar(CmdStr));
     End
     Else
     Begin
       i := Round(TrkRate + (TrkRate/2));
       Str(i,TempStr);
       CmdStr := 'AX jg'+ TempStr +';';
       SendString(CommRecord,pchar(CmdStr));
     End ;
end;

procedure TMainForm.WJogMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
Var
  CmdStr, TempStr : String;
begin
  MainForm.Timer_Main.enabled := False;
  Str(TrkRate,TempStr);
  CmdStr := 'AX ST; jg'+ TempStr +';';
  SendString(CommRecord,pchar(CmdStr));
  MainForm.Timer_Main.enabled := True;
end;


procedure TMainForm.ButtonStopClick(Sender: TObject);
Var
  CmdStr : String;
begin
  if(ButtonStop.caption = 'STOP') Then
  begin
    ButtonStop.Caption := 'START';
    cmdstr := 'AA ST;' ;
    SendString(CommRecord,pchar(CmdStr));
    Timer_Main.Enabled := false;
  End
  Else
  Begin
    ButtonStop.Caption := 'STOP';
    Timer_Main.Enabled := True;
  End;
end;

procedure TMainForm.FormCreate(Sender: TObject);
begin
    NoPassword := True;
    PFCIniFile := TIniFile.Create('OBS_CONFIG.Ini');
    Ypole := 1;
    TempList := TStringList.Create;
    setcallback( DLLWork );  {Dll Work is the entry procedure for the Dll Callback}
    pndomem := getndomem;
//
  CommRecord.CommPortNumber := strtoint(PFCIniFile.ReadString('Main','CommPort','1'))  ;
   loadparms;

  if( not(MotorInit)) then halt ;
  init;   //State Variables
  Timer_Main.enabled := True;

end;
procedure TMainForm.InitializeButtonClick(Sender: TObject);
begin
  Timer_Main.enabled := False;
  loadparms;
  Init;      //State Variables
//
  Timer_Main.enabled := True;
end;

procedure TMainForm.FormDestroy(Sender: TObject);
begin
   PFCIniFile.Free;
   TempList.Free;
end;


procedure TMainForm.ButtonParkClick(Sender: TObject);
begin
  Parkit := True;
end;

procedure TMainForm.BaseEditDblClick(Sender: TObject);
begin
   TrkRate := StrToFloat(BaseEdit.Text);
end;


procedure TMainForm.CmdEditEnter(Sender: TObject);
begin
//check for password

     if(CmdEdit.Text = 'operatorsonly') then NoPassword := False;
     CmdEdit.Text := '';
     init;
     MainForm.Refresh;
end;

procedure TMainForm.Timer_padleTimer(Sender: TObject);
begin
  Padle_timer_update;
end;


end.
