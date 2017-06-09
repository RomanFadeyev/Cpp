unit SDDispatcher;

interface
uses
  Windows,Messages,SysUtils,Forms,Dialogs,SDMain,Controls,Defines,Interfaces;

type
  TSyntaxDiagramDispath = class (TSyntaxDiagram)
  private
    FTempHWnd   : HWND;

    procedure   STMClear  (var Message: TWMNoParams); message STM_CLEAR;
    procedure   STMArrange(var Message: TWMNoParams); message STM_ARRANGE;
    procedure   STMAddRule(var Message: TSTMAddRule); message STM_ADDRULE;
    procedure   STMBeginUpdate(var Message: TSTMBeginUpdate); message STM_BEGINUPDATE;
    procedure   STMEndUpdate(var Message: TSTMEndUpdate); message STM_ENDUPDATE;
    procedure   STMSetRegKey(var Message: TSTMSetRegKey); message STM_SETREGKEY;
    procedure   STMGetScale(var Message: TWMNoParams); message STM_GETSCALE;
    procedure   STMSetScale(var Message: TSTMSetScale); message STM_SETSCALE;
    procedure   STMLoadPlacement(var Message: TSTMLoadPlacement); message STM_LOADPlacement;
    procedure   STMSavePlacement(var Message: TSTMSavePlacement); message STM_SavePlacement;

    //ПРеобразование внутреннего представления данных в дескрипторы HRULE
    function    GetHandleFromItem (aRule: TRuleItemLocation): HRULE;
    function    GetItemFromHandle (aHandle: HRULE):TRuleItemLocation;
  public
    procedure   CreateParams(var Params: TCreateParams);   override;
    procedure   CreateWnd; override;
    constructor CreateByHWND(Handle:HWND);
  end;


  function RegisterSD: boolean;

implementation

//------------------------------------------------------------------------------
function StartWndProc(HWindow: HWnd; Message, WParam,LParam: Longint): Longint; stdcall;
var
  aControl: TSyntaxDiagramDispath;
begin
  aControl:=TSyntaxDiagramDispath.CreateByHWND(HWindow);
  CreationControl:=aControl;
  result:=InitWndProc(HWindow,Message,WParam,LParam);
end;
//------------------------------------------------------------------------------
function RegisterSD: boolean;
var
  ClassInfo:WNDCLASSEX;
begin
  FillChar(ClassInfo, SizeOf(ClassInfo), 0);

  ClassInfo.cbSize := sizeof(ClassInfo);
  ClassInfo.style := CS_GLOBALCLASS or CS_DBLCLKS;// or CS_HREDRAW or CS_VREDRAW;
  ClassInfo.lpfnWndProc :=@StartWndProc;
  ClassInfo.cbClsExtra := 0;
  ClassInfo.cbWndExtra := 0;
  ClassInfo.hInstance := hInstance;
  ClassInfo.hIcon := 0;
  ClassInfo.hCursor :=Screen.Cursors[crDefault];
  ClassInfo.hbrBackground := 0;
  ClassInfo.lpszMenuName := nil;
  ClassInfo.lpszClassName := SDClassName;
  ClassInfo.hIconSm := 0;
  result := RegisterClassEx(ClassInfo) <> 0;
end;

{ TSyntaxDiagramDispath }

constructor TSyntaxDiagramDispath.CreateByHWND(Handle: HWND);
begin
  Create(nil);

  FTempHWnd:=Handle;
  HandleNeeded;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.CreateParams(var Params: TCreateParams);
begin
  inherited;
  Params.WinClassName:=SDClassName;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.CreateWnd;
var
  Params: TCreateParams;
begin
  if FTempHWnd=0 then
  begin
    inherited;
  end
  else begin
    WindowHandle:=FTempHWnd;

    CreateParams(Params);
    DefWndProc := Params.WindowClass.lpfnWndProc;

    StrDispose(WindowText);
    WindowText := nil;

    FTempHWnd:=0;
  end;
end;
//------------------------------------------------------------------------------
function TSyntaxDiagramDispath.GetHandleFromItem(aRule: TRuleItemLocation): HRULE;
begin
  if (aRule.aNode=nil) then
    result:=-1
  else
    result:=TreeIndex(aRule.aTree);
  if result=-1 then exit;

  result:=(result shl 24) or aRule.aNode.ParentCollection.Find(aRule.aNode)+1;
end;
//------------------------------------------------------------------------------
function TSyntaxDiagramDispath.GetItemFromHandle(aHandle: HRULE): TRuleItemLocation;
var
  i,j: integer;
begin
  result.aTree:=nil;
  result.aNode:=nil;

  if aHandle=-1 then exit;

  i:= aHandle shr 24; //Номер дерева
  j:= aHandle and $00FFFFFF;

  if (i=-1) or (j=-1) then exit;
  if (i>=TReeCount) then exit;

  result.aTree:=Tree[i];
  result.aNode:=result.aTree.Items[j];
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMAddRule(var Message: TSTMAddRule);
var
  Rule   : TRule;
  Item   : TRuleItemLocation;
begin
  Message.Result:=0;

  Rule:=TRule.Create(self);
  Rule.Import(Message.RuleStruct^);

  Item:=AddRule(Rule);
//  Message.Result:=GetHandleFromItem(Item);
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMArrange(var Message: TWMNoParams);
begin
  Arrange;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMBeginUpdate(var Message: TSTMBeginUpdate);
begin
  BeginUpdate;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMClear(var Message: TWMNoParams);
begin
  Clear;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMEndUpdate(var Message: TSTMEndUpdate);
begin
  EndUpdate;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMGetScale(var Message: TWMNoParams);
begin

end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMLoadPlacement(var Message: TSTMLoadPlacement);
begin
  LoadPlacementFromFile(Message.FileName);
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMSavePlacement(var Message: TSTMSavePlacement);
begin
  SavePlacementToFile(Message.FileName);
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMSetRegKey(var Message: TSTMSetRegKey);
begin
  RegKey:=Message.Key;
end;
//------------------------------------------------------------------------------
procedure TSyntaxDiagramDispath.STMSetScale(var Message: TSTMSetScale);
begin

end;

end.
