unit Pxc_sample2;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls, ComCtrls;

type
  TForm1 = class(TForm)
    Image1: TImage;
    ImageDoneTimer: TTimer;
    ExternalConnector: TRadioGroup;
    ImageDepth: TRadioGroup;
    ImagePosition: TTrackBar;
    Label1: TLabel;
    Label2: TLabel;
    PixelCapture: TCheckBox;
    GroupBox1: TGroupBox;
    LocationLabel: TLabel;
    labelGrey: TLabel;
    labelRed: TLabel;
    LabelBlue: TLabel;
    LabelGreen: TLabel;
    SaveBMP: TButton;
    PixelTimer: TTimer;
    SaveDialog1: TSaveDialog;
    OpenDialog1: TOpenDialog;
    HoldImage: TButton;
    ImageBtn1: TButton;
    ImageBtn2: TButton;
    ImageBtn3: TButton;
    ImageBtn4: TButton;
    Live: TButton;
    ClearALL: TButton;
    Stream: TButton;
    procedure FormCreate(Sender: TObject);
    procedure FormPaint(Sender: TObject);
    procedure ImageDoneTimerTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure ExternalConnectorClick(Sender: TObject);
    procedure ImagePositionChange(Sender: TObject);
    procedure PixelCaptureClick(Sender: TObject);
    procedure Image1MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure EnableLabels (rgb24 : Boolean);
    procedure SaveBMPClick(Sender: TObject);
    procedure PixelTimerTimer(Sender: TObject);
    procedure ImageDepthClick(Sender: TObject);
    procedure HoldImageClick(Sender: TObject);
    procedure ImageBtn1Click(Sender: TObject);
    procedure ImageBtn2Click(Sender: TObject);
    procedure ImageBtn3Click(Sender: TObject);
    procedure ImageBtn4Click(Sender: TObject);
    procedure LiveClick(Sender: TObject);
    procedure ClearALLClick(Sender: TObject);
    procedure StreamClick(Sender: TObject);

  private
    { Private declarations }
     procedure RetrieveHeldImage (slotID : Integer);
  public
    { Public declarations }
  end;


var
  Form1: TForm1;
  hPXC, hFrame, h24Frame, h8Frame : longint;
  opHandle : Integer;
  bitDepth : smallint;
  
  //Operation states
  hasImage,Streaming : Boolean;
  liveCamera,StreamCounter : Integer;           //which input is live

  //pixel capture data
  captureMode : Boolean;
  PixelX : smallint;
  PixelY: smallint;
  PixelData : Array [0..2] of Byte;
  pData : Pointer;

  Streamfilename : string;
  holdIndex : smallint;             //points to last held image
  slotStatus : Array [1..4] of Boolean;      //true indicates slot holds an image
  slotFrame : Array [1..4] of longint;
  slotType : Array [1..4] of String;         //composite or s-video
  slotDepth : Array [1..4] of smallint;      //bitdepth

implementation

uses frame, video_32, pxc200;

{$R *.DFM}

Const
     IMAGE_WIDTH = 640;
     IMAGE_HEIGHT = 480;

Function Makeitfour(Var s : string) : String;
Begin
  if ((s = '') or (length(s)> 4 )) then s := '0000'
  Else while (Length(s) < 4 ) Do  s := '0' + s;
  Result := s;
End ;

procedure TForm1.FormCreate(Sender: TObject);

var
   initResult : longint;
begin
     Streaming := False;
     initResult := PXC200_OpenLibrary(0,0);
     if (initResult = 0) then
        color := clRed
     else
         color := clGreen;
     initResult := FRAME_OpenLibrary (0,0);
     if (initResult = 0) then
        color := clRed
     else
         color := clGreen;

     hPXC := AllocateFG (-1);
     if (hPXC = 0) then
        color := clRed;

     h24Frame := AllocateMemoryFrame (IMAGE_WIDTH, IMAGE_HEIGHT, PBITS_RGB24);
     h8Frame := AliasFrame (h24Frame, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PBITS_Y8);

     //verify that frames were successfully created, and gracefully
     //handle failures to allocate.
     if (h24Frame = 0) then
     begin
          //no frames were created, so kill the entire program
          ShowMessage ('Unable to create image buffers.  Program will terminate.');
          Form1.Close ();   //end program
    end
    else
    begin
          //Set 24-bit frame to be the default
         ImageDepth.ItemIndex := 1;
         hFrame := h24Frame;
         bitDepth := 24;
         //check that 8-bit frame was created, and disable it if necessary
         if (h8Frame = 0) then
            ImageDepth.Enabled := False;
     end;

     pxSetWindowSize (0, 0, Image1.Width, Image1.Height);
     SetLeft (hPXC, 0);
     SetTop (hPXC, 0);
     SetWidth (hPXC, IMAGE_WIDTH);
     SetHeight (hPXC, IMAGE_HEIGHT);
     SetXResolution (hPXC, IMAGE_WIDTH);
     SetYResolution (hPXC, IMAGE_HEIGHT);


     //initialize data members
     hasImage := False;
     holdIndex := 0;

     //start first grab
     opHandle := Grab (hPXC, hFrame, 0);
     
end;

procedure TForm1.FormPaint(Sender: TObject);
begin
     //this check has two purposes; make sure there's something to do when
     //it's called and guard against a possible exception when Image doesn't exist
       if (hasImage = True) then
       begin
          pxPaintDisplay (Image1.Canvas.handle, hFrame, 0, 0,
                         IMAGE_WIDTH, IMAGE_HEIGHT);
          Image1.Invalidate();
          hasImage := False;
       end;
end;
procedure TForm1.ImageDoneTimerTimer(Sender: TObject);
var
  ioresult : integer;
  TempFileNumber : string;
begin
     //make sure the grab is complete before bothering to process it;
     //as this timer gets called every ms, it needs to be efficient.
     if (IsFinished (hPXC, opHandle) > 0) then
     begin
          hasImage := True;
          Form1.FormPaint(Image1.Canvas);
          if(Streaming) Then
          Begin
             TempFileNumber := inttostr(streamcounter);
             TempFileNumber := Makeitfour(TempFileNumber);
             TempFileNumber :=  Streamfilename + TempFileNumber + '.bmp' ;
             ioresult := WriteBMP (hFrame, TempFileNumber, 1);
             if (ioresult <> SUCCESS) then
                ShowMessage ('Unable to save as bitmap.  Error code ' + IntToStr (ioresult));
             StreamCounter := StreamCounter + 1;
          End;
          opHandle := Grab (hPXC, hFrame, 0);
     end;

end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
var
   i: smallint;
begin

     //shut off image updates
     ImageDoneTimer.Enabled := False;

     //clean out the Queue
     if (IsFinished (hPXC, opHandle) < 1) then
        KillQueue (hPXC);

     //free all resources
     FreeFG (hPXC);
     FreeFrame (h24Frame);
     FreeFrame (h8Frame);

     for i := 1 to 4 do
         if (slotStatus[i] = True) then
            FreeFrame (slotFrame[i]);

     PXC200_CloseLibrary (0);
     FRAME_CloseLibrary (0);

end;

procedure TForm1.ExternalConnectorClick(Sender: TObject);

var
   OpResult : Integer;
begin
     OpResult := 0;
     //make sure we're actually changing inputs
     if (ExternalConnector.ItemIndex <> liveCamera) then
        OpResult := SetCamera (hPXC, ExternalConnector.ItemIndex, 0);
     //give the frame grabber eight fields to sync up
     //If the PXC200 is switching between different video formats
     //(ie, NTSC to CCIR or vice versa), this switch will take MUCH
     //longer -- around 2.5 seconds.  During this time, the program
     //will probably display bad video. C'est la vie.
     if (OpResult <> 0) then
     begin
        ImageDoneTimer.Enabled := False;
        liveCamera := ExternalConnector.ItemIndex;
        WaitVB(hPXC);
        WaitVB(hPXC);
        WaitVB(hPXC);
        WaitVB(hPXC);
        WaitVB(hPXC);
        WaitVB(hPXC);
        WaitVB(hPXC);
        WaitVB(hPXC);
        if (liveCamera = 1) then
           SetChromaControl (hPXC, SVIDEO)
        else
            SetChromaControl (hPXC, NOTCH_FILTER);
        ImageDoneTimer.Enabled := True;
     end;
end;

procedure TForm1.ImagePositionChange(Sender: TObject);
begin

     ImagePosition.Position := SetLeft (hPXC, ImagePosition.Position);

end;

procedure TForm1.PixelCaptureClick(Sender: TObject);
var
   i : Integer;
begin
     //toggle setting of capture mode
     captureMode := (captureMode = False);
     if (captureMode = True) then
     begin
          PixelX := -1;
          PixelY := -1;
          for i := 0 to 3 do
              PixelData[i] := 0;

          pData := @PixelData;
          EnableLabels (Boolean (bitDepth = 24));
          locationLabel.Enabled := True;
     end
     else
     begin
          labelGrey.Enabled     := False;
          labelRed.Enabled      := False;
          labelBlue.Enabled     := False;
          labelGreen.Enabled    := False;
          locationLabel.Enabled := False;
     end;
end;

procedure TForm1.EnableLabels (rgb24 : Boolean);
begin
     if (rgb24 = True) then
          begin
             labelGrey.Enabled := False;
             labelRed.Enabled := True;
             labelBlue.Enabled := True;
             labelGreen.Enabled := True;
          end
     else
          begin
               labelGrey.Enabled := True;
               labelRed.Enabled := False;
               labelBlue.Enabled := False;
               LabelGreen.Enabled := False;
          end;
end;


procedure TForm1.Image1MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
   clickPoint : TPoint;
begin
     if (captureMode = True) then
     begin
          clickPoint := Point (X, Y);
          PixelX := clickPoint.X;
          PixelY := clickPoint.Y;
     end;
end;

procedure TForm1.SaveBMPClick(Sender: TObject);
var
   filename : PChar;
   ioresult : smallint;
begin
          //disable the live display
          ImageDoneTimer.Enabled := False;
          if SaveDialog1.Execute then
          begin
             filename := Pointer (SaveDialog1.Filename);
             ioResult := WriteBMP (hFrame, filename, 1);
             if (ioresult <> SUCCESS) then
                ShowMessage ('Unable to save as bitmap.  Error code ' + IntToStr (ioresult));
          end;
          ImageDoneTimer.Enabled := True;
end;



procedure TForm1.PixelTimerTimer(Sender: TObject);
begin
     if (captureMode = True) then
     begin
          if (GetPixel (hFrame, pData, PixelX, PixelY) <> 0) then
          begin
               locationLabel.Caption := 'Pixel at (' + IntToStr (PixelX)
                                        + ',' + IntToStr (PixelY) + ')';
               if (bitDepth = 24) then
               begin
                  labelRed.Caption := 'Red: '+ IntToStr (pixelData[2]);
                  labelGreen.Caption := 'Green: '+ IntToStr (pixelData[1]);
                  labelBlue.Caption := 'Blue: '+ IntToStr (pixelData[0]);
               end
               else
                  labelGrey.Caption := 'Greyscale: '+IntToStr (pixelData[0]);

          end;
     end;

end;



procedure TForm1.ImageDepthClick(Sender: TObject);
begin
     If (ImageDepth.ItemIndex = 1) then
     begin  //24 bit setting
            bitDepth := 24;
            hFrame := h24Frame;
     end
     else
     begin  //8 bit setting
            bitDepth := 8;
            hFrame := h8Frame;
     end;

     //if we're changing bit-depths, change which labels are active
     if (CaptureMode = True) then
        EnableLabels (bitDepth = 24);
end;




procedure TForm1.HoldImageClick(Sender: TObject);

var
   sourceBitDepth : smallint;
   operationResult : smallint;
   ImageStr : String;
   i : smallint;
   targetSlot : smallint;
   liveState : Boolean;       //records original state of timer

begin
     //make sure we have a free image space

     targetSlot := 0;
     //find lowest possible slot available
     for i:= 1 to 4 do
         if slotStatus[i] = False then
            begin
            targetSlot := i;
            break;
            end;

     if (targetSlot <> 0) then
     begin
          //save the original state of the timer, then
          //disable live display while we're working
          liveState := ImageDoneTimer.Enabled;
          ImageDoneTimer.Enabled := False;

          //duplicate source's bitdepth
          sourceBitDepth := FrameType (hFrame);

          //allocate a buffer equal to the current buffer
          slotFrame[targetSlot] := AllocateMemoryFrame
                                  (IMAGE_WIDTH, IMAGE_HEIGHT, sourceBitDepth);


          operationResult := CopyFrame (hFrame, 0,0, slotFrame[targetSlot], 0, 0,
                                       IMAGE_WIDTH, IMAGE_HEIGHT);

          If (operationResult <> 0) then
          begin
               //determine video format by reading input connector type
                  if (liveCamera = 0) then
                     slotType[targetSlot] := 'Composite'
                  else
                      slotType[targetSlot] := 'S-Video';

               slotDepth[targetSlot] := bitDepth;
               imageStr := Format ('%d: '+ '%d bit %s',
                                  [targetSlot, bitDepth, slotType[targetSlot]]);

               case targetSlot of
               1: begin
                  ImageBtn1.Caption := imageStr;
                  ImageBtn1.Enabled := True;
                  end;
               2: begin
                  ImageBtn2.Caption := imageStr;
                  ImageBtn2.Enabled := True;
                  end;
               3: begin
                  ImageBtn3.Caption := imageStr;
                  ImageBtn3.Enabled := True;
                  end;
               4: begin
                  ImageBtn4.Caption := imageStr;
                  ImageBtn4.Enabled := True;
                  end;
               end;

               slotStatus[targetSlot] := True;

          end
          else
          begin
               //Copy failed; cleanup
               FreeFrame (slotFrame[targetSlot]);
               slotStatus[targetSlot] := False;
          end;

          //restore timer to original state
          ImageDoneTimer.Enabled := liveState;
     end;

     //return without activity if slot are all full
end;

procedure TForm1.ImageBtn1Click(Sender: TObject);
begin
     if (slotStatus[1] <> False) then
     begin
          RetrieveHeldImage (1);
     end;

end;

procedure TForm1.ImageBtn2Click(Sender: TObject);
begin
     if (slotStatus[2] <> False) then
     begin
          RetrieveHeldImage (2);
     end;
end;

procedure TForm1.ImageBtn3Click(Sender: TObject);
begin
     if (slotStatus[3] <> False) then
     begin
          RetrieveHeldImage (3);
     end;

end;

procedure TForm1.ImageBtn4Click(Sender: TObject);
begin
     if (slotStatus[4] <> False) then
     begin
          RetrieveHeldImage (4);
     end;

end;

procedure TForm1.RetrieveHeldImage (slotID : Integer);
begin
     if (ImageDoneTimer.Enabled = True) then
        Live.Click ();
     if (slotDepth[slotID] = 24) then
        hFrame := h24Frame
     else
         hFrame := h8Frame;

     if (slotType[slotID] = 'S-Video') then
           SetChromaControl (hPXC, SVIDEO)
     else
           SetChromaControl (hPXC, NOTCH_FILTER);

     CopyFrame (slotFrame[slotID], 0, 0, hFrame, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
     hasImage := True;
     Form1.FormPaint(Image1.Canvas);

end;

procedure TForm1.LiveClick(Sender: TObject);
var
   newState : Boolean;

begin

     newState := (Live.Caption <> 'Live');

     //if live, we are trying to turn off video.  newState is false.
     //Otherwise, video is off, and we're turning it on; newState is true.

     if (newState) then
     //trying to restart the camera after a pause.  This pause could have been
     //a held image recall, or just a pause.  Need to set ALL settings again.
     begin
          Live.Caption := 'Live';
          //restore original connector
          if (liveCamera = 1) then
              SetChromaControl (hPXC, SVIDEO)
          else
              SetChromaControl (hPXC, NOTCH_FILTER);

          if (bitDepth = 24) then
              hFrame := h24Frame
          else
              hFrame := h8Frame;
          ImageDoneTimer.Enabled := True;
     end

     else
     //pausing the live display for some reason.  Either user-request, or
     //a recall of a held image
     begin
          //stop the timer
          ImageDoneTimer.Enabled := False;
          Live.Caption := 'Paused';
          if (IsFinished (hPXC, opHandle) < 1) then
            KillQueue (hPXC);
     end;

     //set live controls accordingly
     ExternalConnector.Enabled := newState;
     ImageDepth.Enabled := newState;
     ImagePosition.Enabled := newState;


end;

procedure TForm1.ClearALLClick(Sender: TObject);
var
   i : smallint;
begin
     for i := 1 to 4 do
         begin
              if (slotStatus[i] = True) then
                  begin
                       FreeFrame (slotFrame[i]);
                       slotStatus[i] := False;
                  end;
         end;
     ImageBtn1.Caption := '(empty)';
     ImageBtn1.Enabled := False;
     ImageBtn2.Caption := '(empty)';
     ImageBtn2.Enabled := False;
     ImageBtn3.Caption := '(empty)';
     ImageBtn3.Enabled := False;
     ImageBtn4.Caption := '(empty)';
     ImageBtn4.Enabled := False;
end;

procedure TForm1.StreamClick(Sender: TObject);
begin
   ImageDoneTimer.Enabled := False;
   if(stream.Caption = 'Stream to Disk') Then
   Begin  {Start the streaming}
     if SaveDialog1.Execute then
     begin
       Streamfilename :=  (SaveDialog1.Filename);
       StreamCounter := 0;
       stream.Caption := 'Stop Streaming' ;
       Streaming := True;
       ImageDoneTimer.Enabled := True;
     end;
   End {Start the streaming}
   Else
   Begin {Stop the streaming}
       stream.Caption := 'Stream to Disk' ;
       Streaming := False;
       ImageDoneTimer.Enabled := True;
   End; {Stop the streaming}
end;

end.
