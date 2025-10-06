unit video_32;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls;

procedure pxSetWindowSize (x,y,dx,dy : Integer); stdcall; external 'video_32.dll';
procedure pxPaintDisplay (hdc : HDC; frh : longint; x,y,dx,dy : Integer); stdcall;
          external 'video_32.dll';
implementation

end.
 