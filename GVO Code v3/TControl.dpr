program TControl;

{%File 'Notes.txt'}

uses
  Forms,
  Main in 'MAIN.PAS' {Form1},
  TimerUnit in 'TimerUnit.pas',
  Utils in 'Utils.pas',
  Slew in 'Slew.pas',
  CalcCoord in 'CalcCoord.pas',
  handpadle in 'handpadle.pas',
  quad1to1 in 'quad1to1.pas',
  quad1to3 in 'quad1to3.pas',
  quad3to3 in 'quad3to3.pas',
  quad3to1 in 'quad3to1.pas',
  Sync in 'Sync.pas',
  quad1to2 in 'quad1to2.pas',
  quad2to2 in 'quad2to2.pas',
  quad2to1 in 'quad2to1.pas',
  GetQandYU in 'GetQandYU.pas',
  OMS68SERMC in 'OMS68SERMC.PAS',
  ManualPadle in 'ManualPadle.pas';

{$R *.RES}

begin
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
