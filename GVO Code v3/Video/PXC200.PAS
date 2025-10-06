unit Pxc200;

interface

  {  ********  Flags  ********  }
  Const EITHER = $3;
  Const FIELD0 = $1;
  Const FIELD1 = $2;
  Const IMMEDIATE = $4;
  Const QUEUED = $8;
  Const SINGLE_FLD = $10;

  {  ********  Trigger Types  ********  }
  Const LATCH_RISING = $1;
  Const LATCH_FALLING = $2;
  Const LATCH_CHANGE = $3;
  Const IO_INPUT = $4;
  Const IO_OUTPUT = $5;

  {  ********  Error Numbers  ********  }
  Const ERR_NONE = $0;
  Const ERR_NOT_VALID = $1;
  Const ERR_CORRUPT = $2;
  Const ERR_IO_FAIL = $4;
  Const WARN_OVERFLOW = $8;

  {  ********  File I/O Return Values  ********  }
  Const SUCCESS = 0;
  Const FILE_EXISTS = 2;
  Const FILE_OPEN_ERROR = 3;
  Const BAD_WRITE = 4;
  Const BAD_READ = 5;
  Const WRONG_BITS = 8;
  Const BAD_FILE = 11;
  Const INVALID_FRAME = 12;

  {  ********  Filter Control Flags  ********  }
  Const COMB_FILTER = $1;
  Const SVIDEO = $2;
  Const NOTCH_FILTER = $4;
  Const BW_DETECT = $8;
  Const AGC = $10;

  Const GAMMA_CORRECT = $2;
  Const CORE_8 = $10;
  Const CORE_16 = $20;
  Const CORE_32 = $30;
  Const LOW_FILT_AUTO = $800;
  Const LOW_FILT_1 = $900;
  Const LOW_FILT_2 = $A00;
  Const LOW_FILT_3 = $B00;
  Const PEAK_FILT_0 = $C00;
  Const PEAK_FILT_1 = $D00;
  Const PEAK_FILT_2 = $E00;
  Const PEAK_FILT_3 = $F00;

  {  ********  Video Detect Types  ********  }
  Const AUTO_FORMAT = 0;
  Const NTSC_FORMAT = 1;
  Const NTSCJ_FORMAT = 2;
  Const PAL_FORMAT = 3;
  Const PALM_FORMAT = 4;
  Const PALN_FORMAT = 5;
  Const SECAM_FORMAT = 6;


  Const Pxc200_Dll = 'Pxc2_95';

  { **********  Setup Functions  **************************************  }
  function PXC200_OpenLibrary(i: longint; s: smallint): smallint; stdcall; external Pxc200_Dll name 'OpenLibrary';
  procedure PXC200_CloseLibrary(i: longint); stdcall; external Pxc200_Dll name 'CloseLibrary';

  { ---------- General Device Maintenance functions -------------------- }
  function AllocateFG(n: smallint): longint; stdcall; external Pxc200_Dll;
  procedure FreeFG(fgh: longint); stdcall; external Pxc200_Dll;
  function ReadRevision(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  procedure Reset(fgh: longint); stdcall; external Pxc200_Dll;
  function CheckError(fgh: longint): longint; stdcall; external Pxc200_Dll;
  function ReadProtection(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function ReadSerial(fgh: longint): longint; stdcall; external Pxc200_Dll;

  { ---------- Basic Digital I/O Functions ----------------------------- }
  function GetIOType(fgh: longint; n: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetIOType(fgh: longint; n, tp: smallint): smallint; stdcall; external Pxc200_Dll;
  function ReadIO(fgh: longint): cardinal; stdcall; external Pxc200_Dll;
  function WriteImmediateIO(fgh: longint; mask, state: cardinal): smallint; stdcall; external Pxc200_Dll;

  { ---------- Basic Frame Grabber Functions --------------------------- }
  function Grab(fgh, frh: longint; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function Wait(fgh: longint; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function WaitVB(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  procedure WaitFinished(fgh, qh: longint); stdcall; external Pxc200_Dll;
  function IsFinished(fgh: longint; qh: integer): smallint; stdcall; external Pxc200_Dll;
  procedure KillQueue(fgh: longint); stdcall; external Pxc200_Dll;
  function AllocateBuffer(dx, dy, tp: smallint): longint; stdcall; external Pxc200_Dll;
  procedure FreeFrame(frh: longint); stdcall; external Pxc200_Dll;
  function SetFieldCount(fgh, c: longint): integer; stdcall; external Pxc200_Dll;
  function GetFieldCount(fgh: longint): longint; stdcall; external Pxc200_Dll;

  { ---------- Optional FG Functions ----------------------------------- }
  function GrabContinuous(fgh, frh: longint; state, flags: smallint): smallint; stdcall; external Pxc200_Dll;
  function VideoType(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function SetXResolution(fgh: longint; rez: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetYResolution(fgh: longint; rez: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetWidth(fgh: longint; w: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetHeight(fgh: longint; h: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetLeft(fgh: longint; l: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetTop(fgh: longint; t: smallint): smallint; stdcall; external Pxc200_Dll;
  function GetXResolution(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function GetYResolution(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function GetWidth(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function GetHeight(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function GetLeft(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function GetTop(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function WaitAnyEvent(fgh, ioh: longint; mask, state: cardinal; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function WaitAllEvents(fgh, ioh: longint; mask, state: cardinal; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function SwitchGrab(fgh, f0, f1, f2, f3: longint; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function SwitchCamera(fgh: longint; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function GetSwitch(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function SetCamera(fgh: longint; n, flags: smallint): smallint; stdcall; external Pxc200_Dll;
  function GetCamera(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function SetContrast(fgh: longint; c: single; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function SetBrightness(fgh: longint; b: single; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function GetContrast(fgh: longint):single; stdcall; external Pxc200_Dll;
  function GetBrightness(fgh: longint):single; stdcall; external Pxc200_Dll;

  { ---------- HSV custom functions --------------------------- }
  function SetHue(fgh: longint; h: single; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function SetSaturation(fgh: longint; s: single; flags: smallint): longint; stdcall; external Pxc200_Dll;
  function GetHue(fgh: longint): single; stdcall; external Pxc200_Dll;
  function GetSaturation(fgh: longint): single; stdcall; external Pxc200_Dll;

  { ----------- These are PXC200 custom features ------------- }
  function SetPixelFormat(fgh: longint; tp: smallint): smallint; stdcall; external Pxc200_Dll;
  function SetVideoLevel(fgh: longint; white: single): single; stdcall; external Pxc200_Dll;
  function GetVideoLevel(fgh: longint): single; stdcall; external Pxc200_Dll;
  function SetChromaControl(fgh: longint; cf: smallint): smallint; stdcall; external Pxc200_Dll;
  function GetChromaControl(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function SetLumaControl(fgh: longint; cf: smallint): smallint; stdcall; external Pxc200_Dll;
  function GetLumaControl(fgh: longint): smallint; stdcall; external Pxc200_Dll;
  function SetVideoDetect(fgh: longint; tp: smallint): smallint; stdcall; external Pxc200_Dll;
  function GetVideoDetect(fgh: longint): smallint; stdcall; external Pxc200_Dll;

implementation

end.


