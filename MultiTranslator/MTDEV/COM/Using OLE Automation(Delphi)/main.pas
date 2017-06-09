unit main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  MTDev_TLB,StdCtrls, ExtCtrls, OleServer;

type
  TfmMain = class(TForm)
    Bevel1   : TBevel;
    Bevel2   : TBevel;
    buStart  : TButton;
    buClose  : TButton;
    buNext   : TButton;
    lbReport : TListBox;
    dlgOpen  : TOpenDialog;
    procedure buCloseClick(Sender: TObject);
    procedure buStartClick(Sender: TObject);
    procedure buNextClick(Sender: TObject);
  private
    MTDEv        : TApplication;
    FNextEnabled : boolean;
    procedure Report(const aMessage: string; args: array of const);
    procedure Wait;
  public
    constructor Create (aOwner: TComponent);                            override;
    destructor Destroy; override;
  end;

var
  fmMain : TfmMain;

implementation
{$R *.DFM}

const
  BoolStr: array [boolean] of string = ('NO','YES');

//------------------------------------------------------------------------------
constructor TfmMain.Create(aOwner: TComponent);
begin
  inherited;
  MTDEv:=MTDev_TLB.TApplication.Create(self);
end;
//------------------------------------------------------------------------------
destructor TfmMain.Destroy;
begin
  FreeAndNil(MTDEv);
  inherited;
end;
//------------------------------------------------------------------------------
procedure TfmMain.buCloseClick(Sender: TObject);
begin
  Close;
end;
//------------------------------------------------------------------------------
procedure TfmMain.Report(const aMessage: string; args: array of const);
begin
  lbReport.Items.Add(Format(aMessage,args));
  Forms.Application.ProcessMessages;
end;
//------------------------------------------------------------------------------
procedure TfmMain.buStartClick(Sender: TObject);
var
  aDoc        : ProjectDocument;
  RunExitCode : integer;
begin
  try
    Report('Connecting to server...',[]);
    MTDEv.Connect;
    buStart.Enabled:=false;
    buNext.Enabled:=true;
    try
      Report('Application visible: %s',[BoolStr[MTDEv.Visible]]);

      Report('Press "Perform" to show main form of MultiTranslator...',[]); Wait;
      MTDEv.Visible:=true;

      Report('Opening project... ',[]);
      dlgOpen.FileName:=MTDEv.MRUDocument;
      dlgOpen.InitialDir:=ExtractFileDir(dlgOpen.FileName);

      if not dlgOpen.Execute then
        abort;
      Report(dlgOpen.FileName,[]);

      //Create empty project document
      //aDoc:=MTDEv.CreateProject('c:\test.pjt');

      aDoc:=MTDEv.OpenDocument(dlgOpen.FileName) as IProjectDocument;
      if (aDoc=nil) then
        Report('Cannot open project.',[])
      else begin
        Report('Project loaded',[]);
        Report('TargetPath: %s',[aDoc.TargetPath]);
        Report('Params: %s',[aDoc.Params]);
        Report('Press "Perform" to start building the project...',[]); Wait;
        buNext.Enabled:=false;
        if not aDoc.Build then
        begin
          Report('Cannot build project.',[]);
          exit;
        end;

        buNext.Enabled:=true;

        Report('Press "Perform" to start running the project... ',[]);  Wait;
        buNext.Enabled:=false;
        aDoc.Run(RunExitCode);
        buNext.Enabled:=true;

        if aDoc.Modified then
        begin
          Report('The project was modified. Press "Perform" to start saving the project... ',[]);   Wait;
          aDoc.SaveAll;
        end;
      end;
    finally
      aDoc:=nil; //Finalize Document Interface Pointer

      Report('Press "Perform" to disconnecting...',[]);
      Wait;
      if MessageBox(Handle,'Do you want to shutdown Multitranslator before disconnecting?','Shutdown COM-server',MB_YESNO or MB_ICONQUESTION)=ID_YES then
        MTDEv.Quit(true);
      MTDEv.Disconnect;
    end;
  finally
    buNext.Enabled:=false;
    buStart.Enabled:=true;
  end;
end;
//------------------------------------------------------------------------------
procedure TfmMain.Wait;
begin
  FNextEnabled:=false;
  while not FNextEnabled do
  begin
    Forms.Application.ProcessMessages;
    if Forms.Application.Terminated then
      abort;     
  end;
end;
//------------------------------------------------------------------------------
procedure TfmMain.buNextClick(Sender: TObject);
begin
  FNextEnabled:=true;
end;

end.
