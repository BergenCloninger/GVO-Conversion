unit Frame;

interface

// some common image data types
Const PBITS_Y8        = $0008;
Const PBITS_Y16       = $0010;
Const PBITS_Yf        = $0120;
Const PBITS_RGB15     = $060F;
Const PBITS_RGB16     = $0210;
Const PBITS_RGB24     = $0218;
Const PBITS_RGB32     = $0620;
Const PBITS_RGBf      = $0360;
Const PBITS_YUV422    = $0810;
Const PBITS_YUV444    = $0818;
Const PBITS_YUV422P   = $1810;
Const PBITS_YUV444P   = $1818;

// bit fields for parsing pixel types 
Const PBITS_FLOAT     = $0100;
Const PBITS_BITS      = $00FF;
Const PBITS_RGB       = $0200;
Const PBITS_ALPHA     = $0400;
Const PBITS_YUV       = $0800;
Const PBITS_PLANES    = $1000;

// type fields which tell what the BUFFERDATA user means 
Const PTYPE_UFLAT     = $01;  // pointer to array of pixels
Const PTYPE_UPAGED    = $02;  // pointer to PAGER structure
Const PTYPE_UPLANES   = $03;  // pointer to a list of frame structures

// type fields which tell what the BUFFERDATA dma means 
Const PTYPE_DFLAT     = $01;  // 32 bit physical bus address
Const PTYPE_DLINEAR   = $02;  // linear address
Const PTYPE_DVIRTUAL  = $03;  // pointer in user's address space
Const PTYPE_DLOCKED   = $04;  // pointer to dma stuff

// the DLOCKED value means that a dma device is using the buffer, and
//may have written arbitrary stuff into the dma field.  It will eventually
//unlock the buffer and restore the type field to something else.
//  The DLINEAR value means that the buffer has a known linear address,
// but not a known physical mapping.  In flat addressing this means the user
// data matches the dma data, but in other addressing modes they may differ.


// BMP File I/O returns 
Const SUCCESS           = 0;
Const FILE_EXISTS       = 2;
Const FILE_OPEN_ERROR   = 3;
Const BAD_WRITE         = 4;
Const BAD_READ          = 5;
Const WRONG_BITS        = 8;
Const FRAME_SIZE        = 9;
Const BAD_FILE          = 11;
Const INVALID_FRAME     = 12;

Const frame_dll         = 'frame_32.dll';



//PXC200 Frame library functions
function AliasFrame (hFrame : longint; x0, y0, dx, dy : smallint; frame_type : word) : longint ; stdcall; external frame_dll;
function AllocateFlatFrame (dx, dy :smallint; frame_type : word) : longint; stdcall; external frame_dll;
function AllocateMemoryFrame(dx, dy : smallint; frame_type : word) : longint ; stdcall; external frame_dll;
procedure FRAME_CloseLibrary (var1 : smallint); stdcall; external frame_dll name 'CloseLibrary';
function CopyFrame (source : longint; sourcex, sourcey :  smallint; dest : longint; destx, desty, dx, dy : smallint) : smallint ; stdcall; external frame_dll;
function ExtractPlane (hFrame : longint; plane : smallint) : longint ; stdcall; external frame_dll;
function FrameBuffer (hFrame : longint) : Pointer; stdcall; external frame_dll;
function FrameHeight (hFrame : longint) :  smallint; stdcall; external frame_dll;
function FrameType (hFrame : longint) : smallint; stdcall; external frame_dll;
function FrameWidth (hFrame : longint) : smallint; stdcall; external frame_dll;
procedure FreeFrame (hFrame : longint); stdcall; external frame_dll;
function GetColumn (hFrame : longint; buf : pointer; column : smallint) : smallint ; stdcall; external frame_dll;
function GetPixel (hFrame : longint; pixel : pointer; x, y: smallint) : smallint ; stdcall; external frame_dll;
function GetRectangle (hFrame : longint; buf : pointer; x0, y0, dx, dy : smallint) : smallint ; stdcall; external frame_dll;
function GetRow (hFrame : longint; buf : pointer; row : smallint) : smallint ; stdcall; external frame_dll;
function FRAME_OpenLibrary (var1, var2 : smallint) : smallint; stdcall; external frame_dll name 'OpenLibrary';
function PutColumn (buf : pointer; hFrame : longint; column : smallint) : smallint ; stdcall; external frame_dll;
function PutPixel (pixel : pointer; hFrame : longint; x, y : smallint ) : smallint ; stdcall; external frame_dll
function PutRectangle (buf : pointer; hFrame : longint; x0, y0, dx, dy : smallint) : smallint ; stdcall; external frame_dll;
function PutRow (buf : pointer; hFrame : longint; row : smallint) : smallint ; stdcall; external frame_dll;
function ReadBin (hFrame : longint; filename : String) : smallint; stdcall; external frame_dll;
function ReadBMP(hFrame : longint; filename : String) :  smallint; stdcall; external frame_dll;
function WriteBin (hFrame : longint; filename : String; overwrite : smallint ) : smallint; stdcall; external frame_dll; 
function WriteBMP (hFrame : longint; filename : String; overwrite : smallint ) : smallint; stdcall; external frame_dll; 
           
//WARNING:  these functions use unsigned long values to represent physical addresses.
//Here, the interface file is using longint which is not quite the same.  In most
//cases, this should not give you any problems, but it could become a problem
//if the longint values are used for any kind of pointer math.
function FrameAddress (hFrame : longint) : longint ; stdcall; external frame_dll;
function AllocateAddress ( physaddress : longint; dx, dy : smallint; frame_type : word)  : longint ; stdcall; external frame_dll;
  
implementation

end.
 