library mtsd;

{ Important note about DLL memory management: ShareMem must be the
  first unit in your library's USES clause AND your project's (select
  Project-View Source) USES clause if your DLL exports any procedures or
  functions that pass strings as parameters or function results. This
  applies to all strings passed to and from your DLL--even those that
  are nested in records and classes. ShareMem is the interface unit to
  the BORLNDMM.DLL shared memory manager, which must be deployed along
  with your DLL. To avoid using BORLNDMM.DLL, pass string information
  using PChar or ShortString parameters. }

uses
  SysUtils,
  Classes,
  Windows,
  Forms,
  Controls,
  fmRuleProps in 'fmRuleProps.pas' {fmRuleProperties},
  STDispatcher in 'STDispatcher.pas',
  Interfaces in 'Interfaces.pas',
  Defines in 'Defines.pas',
  STMain in 'STMain.pas',
  SDMain in 'SDMain.pas',
  SDDispatcher in 'SDDispatcher.pas';

{$R *.RES}
//------------------------------------------------------------------------------
function GetLibraryVersion: integer; stdcall;
begin
  result:=1;
end;
//------------------------------------------------------------------------------
function QuerySDReady:longbool; stdcall;
begin
  result:=true;
end;
//------------------------------------------------------------------------------
exports
  GetLibraryVersion name 'GetLibraryVersion',
  QuerySDReady name 'QuerySDReady';

type
  //Блокировка исключений
  TExceptionHandler = class
    procedure OnException(Sender: TObject; E:Exception);
  end;


{ TExceptionHandler }
procedure TExceptionHandler.OnException(Sender: TObject; E: Exception);
begin
  Application.ShowException(E);
end;

var
  ExceptionHandler : TExceptionHandler;

begin
  ExceptionHandler:=TExceptionHandler.Create;
  Application.OnException:=ExceptionHandler.OnException;

  RegisterST;
  RegisterSD;  
end.
