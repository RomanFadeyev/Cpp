{*******************************************************************************

  Верхний уровень класса синтаксических диаграмм: принимает сообщения
  и преобразует их в перевызовы внутренних функций

********************************************************************************}

unit STDispatcher;

interface
  uses Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  Interfaces,Defines,STMain,Dialogs;

type
  TSyntaxTreeDispath = class (TSyntaxTree)
  private
    FTempHWnd   : HWND;
    procedure   STMClear  (var Message: TWMNoParams); message STM_CLEAR;
    procedure   STMArrange(var Message: TWMNoParams); message STM_ARRANGE;
    procedure   STMAddRule(var Message: TSTMAddRule); message STM_ADDRULE;
    procedure   STMGetRuleParents(var Message: TSTMGetParents); message STM_GETRULEPARENTS;
    procedure   STMGetRuleParentCount(var Message: TSTMGetParentCount); message STM_GETRULEPARENTCOUNT;
    procedure   STMBeginUpdate(var Message: TSTMBeginUpdate); message STM_BEGINUPDATE;
    procedure   STMEndUpdate(var Message: TSTMEndUpdate); message STM_ENDUPDATE;
    procedure   STMSetRegKey(var Message: TSTMSetRegKey); message STM_SETREGKEY;
    procedure   STMGetScale(var Message: TWMNoParams); message STM_GETSCALE;
    procedure   STMSetScale(var Message: TSTMSetScale); message STM_SETSCALE;
    procedure   STMLoadData(var Message: TSTMLoadData); message STM_LOADDATA;
    procedure   STMSaveData(var Message: TSTMSaveData); message STM_SaveDATA;
    procedure   STMLoadPlacement(var Message: TSTMLoadPlacement); message STM_LOADPlacement;
    procedure   STMSavePlacement(var Message: TSTMSavePlacement); message STM_SavePlacement;
  public
    //ПРеобразование внутреннего представления данных в дескрипторы HRULE
    function GetHandleFromItem (aRule: TRulePlacementItem): HRULE;
    function GetItemFromHandle (aHandle: HRULE):TRulePlacementItem;

    procedure   CreateParams(var Params: TCreateParams);   override;
    procedure   CreateWnd; override;
    constructor CreateByHWND(Handle:HWND);
  end;

  function RegisterST: boolean;

implementation

//------------------------------------------------------------------------------
function StartWndProc(HWindow: HWnd; Message, WParam,LParam: Longint): Longint; stdcall;
var
  aControl: TSyntaxTree;
begin
  aControl:=TSyntaxTreeDispath.CreateByHWND(HWindow);
  CreationControl:=aControl;
  result:=InitWndProc(HWindow,Message,WParam,LParam);
end;
//------------------------------------------------------------------------------
function RegisterST: boolean;
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
  ClassInfo.lpszClassName := STClassName;
  ClassInfo.hIconSm := 0;
  result := RegisterClassEx(ClassInfo) <> 0;
end;

{ TSyntaxTreeDispath }

//------------------------------------------------------------------------------
constructor TSyntaxTreeDispath.CreateByHWND(Handle:HWND);
begin
  Create(nil);

  FTempHWnd:=Handle;
  HandleNeeded;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.CreateParams(var Params: TCreateParams);
begin
  inherited;
  Params.WinClassName:=STClassName;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.CreateWnd;
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
function TSyntaxTreeDispath.GetHandleFromItem(aRule: TRulePlacementItem): HRULE;
begin
  ASSERT(aRule.ParentCollection<>nil);
  result:=aRule.ParentCollection.Find(aRule)+1;
end;
//------------------------------------------------------------------------------
function TSyntaxTreeDispath.GetItemFromHandle(aHandle: HRULE): TRulePlacementItem;
begin
  if (aHandle<=0) or (aHandle>Items.Count) then
    result:=nil
  else
    result:=Items[aHandle-1];
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMClear(var Message: TWMNoParams);
begin
  Items.Clear;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMArrange(var Message: TWMNoParams);
begin
  Arrange;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMAddRule(var Message: TSTMAddRule);
var
  Rule   : TRule;
  Item   : TRulePlacementItem;
begin
  Message.Result:=0;

  Rule:=TRule.Create(self);
  Rule.Import(Message.RuleStruct^);

  Item:=AddRule(Rule);
  if Item<>nil then
    Message.Result:=GetHandleFromItem(Item)        //Это и есть HRULE
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMGetRuleParentCount(var Message: TSTMGetParentCount);
var
  Parents   : TRulePlacementItemArray;
  ChildItem : TRulePlacementItem;
begin
  Message.Result:=0;

  ChildItem:=GetItemFromHandle(Message.hRule);
  if (ChildItem=nil) then
    exit;

  ChildItem.GetParents(Parents);
  Message.Result:=1;
  Message.ParentCount:=Length(Parents);
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMGetRuleParents(var Message: TSTMGetParents);
var
  Parents   : TRulePlacementItemArray;
  ChildItem : TRulePlacementItem;
  i         : integer;
begin
  Message.Result:=0;

  ChildItem:=GetItemFromHandle(Message.hRule);
  if (ChildItem=nil) then
    exit;

  Message.Result:=1;
  ChildItem.GetParents(Parents);

  for i:=0 to High(Parents) do
    Message.hParentRules[i]:=GetHandleFromItem(Parents[i]);
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMBeginUpdate(var Message: TSTMBeginUpdate);
begin
  Message.Result:=BeginUpdate;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMEndUpdate(var Message: TSTMEndUpdate);
begin
  Message.Result:=EndUpdate;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMSetRegKey(var Message: TSTMSetRegKey);
begin
  RegKey:=Message.Key;
  Message.Result:=1;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMGetScale(var Message: TWMNoParams);
begin
  Message.Result:=CurrentScale;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMSetScale(var Message: TSTMSetScale);
begin
  CurrentScale:=Message.Scale;
  Message.Result:=1;
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMLoadData(var Message: TSTMLoadData);
begin
  Message.Result:=LoadFromFile(Message.FileName);
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMSaveData(var Message: TSTMSaveData);
begin
  Message.Result:=SaveToFile(Message.FileName);
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMLoadPlacement(var Message: TSTMLoadPlacement);
begin
  Message.Result:=LoadPlacementFromFile(Message.FileName);
end;
//------------------------------------------------------------------------------
procedure TSyntaxTreeDispath.STMSavePlacement(var Message: TSTMSavePlacement);
begin
  Message.Result:=SavePlacementToFile(Message.FileName);
end;

end.
