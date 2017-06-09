

{ IMPORTANT!!!!
  If you import Multitranslator type library via Project\Import Type Library
  you have to open file MTDevLib_TLB after import
  and delete manually all procedures which name begins from GhostMethod...
  It's a Delphi bug.
} 
program AutomationExample;

uses
  Forms,
  XPMan,
  main in 'main.pas' {fmMain},
  mtdev_TLB in '..\..\MTDevLib\mtdev_TLB.pas';

{$R *.RES}

begin
  Forms.Application.Initialize;
  Forms.Application.Title := 'MultiTranslator Automation Example';
  Forms.Application.CreateForm(TfmMain, fmMain);
  Forms.Application.Run;
end.
