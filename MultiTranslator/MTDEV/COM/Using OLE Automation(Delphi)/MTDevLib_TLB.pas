unit MTDevLib_TLB;

// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// PASTLWTR : 1.2
// File generated on 27.05.2004 8:56:46 from Type Library described below.

// ************************************************************************  //
// Type Lib: X:\MultiTranslator\Sources\MTDEV\Debug\MTDevD.exe (1)
// LIBID: {B5708153-CD7A-416D-9022-D46E486C53F0}
// LCID: 0
// Helpfile: 
// HelpString: MTDev (MultiTranslator) 1.0 Type Library
// DepndLst: 
//   (1) v2.0 stdole, (C:\WINDOWS\System32\stdole2.tlb)
// ************************************************************************ //
// *************************************************************************//
// NOTE:                                                                      
// Items guarded by $IFDEF_LIVE_SERVER_AT_DESIGN_TIME are used by properties  
// which return objects that may need to be explicitly created via a function 
// call prior to any access via the property. These items have been disabled  
// in order to prevent accidental use from within the object inspector. You   
// may enable them by defining LIVE_SERVER_AT_DESIGN_TIME or by selectively   
// removing them from the $IFDEF blocks. However, such items must still be    
// programmatically created via a method of the appropriate CoClass before    
// they can be used.                                                          
{$TYPEDADDRESS OFF} // Unit must be compiled without type-checked pointers. 
{$WARN SYMBOL_PLATFORM OFF}
{$WRITEABLECONST ON}
{$VARPROPSETTER ON}
interface

uses Windows, ActiveX, Classes, Graphics, OleServer, StdVCL, Variants;
  

// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:        
//   Type Libraries     : LIBID_xxxx                                      
//   CoClasses          : CLASS_xxxx                                      
//   DISPInterfaces     : DIID_xxxx                                       
//   Non-DISP interfaces: IID_xxxx                                        
// *********************************************************************//
const
  // TypeLibrary Major and minor versions
  MTDevLibMajorVersion = 1;
  MTDevLibMinorVersion = 0;

  LIBID_MTDevLib: TGUID = '{B5708153-CD7A-416D-9022-D46E486C53F0}';

  IID_IDocument: TGUID = '{C7242A99-D06C-4A9E-818D-5D3180EB8D90}';
  IID_IProjectDocument: TGUID = '{C7242A99-D06C-4A9E-818D-5D3180EB8D99}';
  CLASS_ProjectDocument: TGUID = '{206DDA10-728E-4C72-81CC-36FB3BE80916}';
  IID_IApplication: TGUID = '{EE3E904C-6AA0-401A-BCB5-A936E3B717F6}';
  CLASS_Application: TGUID = '{ADC6D969-C5D5-49E4-A75C-F58156BDDBC9}';
type

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
  IDocument = interface;
  IDocumentDisp = dispinterface;
  IProjectDocument = interface;
  IProjectDocumentDisp = dispinterface;
  IApplication = interface;
  IApplicationDisp = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
// *********************************************************************//
  ProjectDocument = IProjectDocument;
  Application = IApplication;


// *********************************************************************//
// Interface: IDocument
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {C7242A99-D06C-4A9E-818D-5D3180EB8D90}
// *********************************************************************//
  IDocument = interface(IDispatch)
    ['{C7242A99-D06C-4A9E-818D-5D3180EB8D90}']
    procedure Save; safecall;
    function Get_Application: IApplication; safecall;
    function Get_Modified: WordBool; safecall;
    procedure Set_Modified(pVal: WordBool); safecall;
    property Application: IApplication read Get_Application;
    property Modified: WordBool read Get_Modified write Set_Modified;
  end;

// *********************************************************************//
// DispIntf:  IDocumentDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {C7242A99-D06C-4A9E-818D-5D3180EB8D90}
// *********************************************************************//
  IDocumentDisp = dispinterface
    ['{C7242A99-D06C-4A9E-818D-5D3180EB8D90}']
    procedure Save; dispid 1;
    property Application: IApplication readonly dispid 2;
    property Modified: WordBool dispid 3;
  end;

// *********************************************************************//
// Interface: IProjectDocument
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {C7242A99-D06C-4A9E-818D-5D3180EB8D99}
// *********************************************************************//
  IProjectDocument = interface(IDocument)
    ['{C7242A99-D06C-4A9E-818D-5D3180EB8D99}']
    function SaveAll: WordBool; safecall;
    function Build: WordBool; safecall;
    function Run(out ExitCode: Integer): WordBool; safecall;
    procedure Clean; safecall;
    function AddFile(const FilePath: WideString): WordBool; safecall;
    function RemoveFile(const FilePath: WideString): WordBool; safecall;
    function GetFileFirst(out FileName: WideString): WordBool; safecall;
    function GetFileNext(out FileName: WideString): WordBool; safecall;
    function Get_Active: WordBool; safecall;
    procedure Set_Active(pVal: WordBool); safecall;
    function Get_TargetPath: WideString; safecall;
    function Get_TargetDirectory: WideString; safecall;
    function Get_Notes: WideString; safecall;
    procedure Set_Notes(const pVal: WideString); safecall;
    function Get_Params: WideString; safecall;
    procedure Set_Params(const pVal: WideString); safecall;
    function Get_Switches: WideString; safecall;
    procedure Set_Switches(const pVal: WideString); safecall;
    function Get_LangGens: WideString; safecall;
    procedure Set_LangGens(const pVal: WideString); safecall;
    property Active: WordBool read Get_Active write Set_Active;
    property TargetPath: WideString read Get_TargetPath;
    property TargetDirectory: WideString read Get_TargetDirectory;
    property Notes: WideString read Get_Notes write Set_Notes;
    property Params: WideString read Get_Params write Set_Params;
    property Switches: WideString read Get_Switches write Set_Switches;
    property LangGens: WideString read Get_LangGens write Set_LangGens;
  end;

// *********************************************************************//
// DispIntf:  IProjectDocumentDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {C7242A99-D06C-4A9E-818D-5D3180EB8D99}
// *********************************************************************//
  IProjectDocumentDisp = dispinterface
    ['{C7242A99-D06C-4A9E-818D-5D3180EB8D99}']
    function SaveAll: WordBool; dispid 5;
    function Build: WordBool; dispid 6;
    function Run(out ExitCode: Integer): WordBool; dispid 7;
    procedure Clean; dispid 8;
    function AddFile(const FilePath: WideString): WordBool; dispid 9;
    function RemoveFile(const FilePath: WideString): WordBool; dispid 11;
    function GetFileFirst(out FileName: WideString): WordBool; dispid 12;
    function GetFileNext(out FileName: WideString): WordBool; dispid 13;
    property Active: WordBool dispid 14;
    property TargetPath: WideString readonly dispid 15;
    property TargetDirectory: WideString readonly dispid 16;
    property Notes: WideString dispid 17;
    property Params: WideString dispid 18;
    property Switches: WideString dispid 19;
    property LangGens: WideString dispid 20;
    procedure Save; dispid 1;
    property Application: IApplication readonly dispid 2;
    property Modified: WordBool dispid 3;
  end;

// *********************************************************************//
// Interface: IApplication
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {EE3E904C-6AA0-401A-BCB5-A936E3B717F6}
// *********************************************************************//
  IApplication = interface(IDispatch)
    ['{EE3E904C-6AA0-401A-BCB5-A936E3B717F6}']
    procedure Quit(SaveChanges: WordBool); safecall;
    procedure ScreenRefresh; safecall;
    procedure Resize(Width: Integer; Height: Integer); safecall;
    procedure Activate; safecall;
    function OpenProject(const Path: WideString): IProjectDocument; safecall;
    function CreateProject(const Path: WideString): IProjectDocument; safecall;
    function Get_Visible: WordBool; safecall;
    procedure Set_Visible(pVal: WordBool); safecall;
    function Get_ActiveDocument: IProjectDocument; safecall;
    function Get_MRUDocument: WideString; safecall;
    property Visible: WordBool read Get_Visible write Set_Visible;
    property ActiveDocument: IProjectDocument read Get_ActiveDocument;
    property MRUDocument: WideString read Get_MRUDocument;
  end;

// *********************************************************************//
// DispIntf:  IApplicationDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {EE3E904C-6AA0-401A-BCB5-A936E3B717F6}
// *********************************************************************//
  IApplicationDisp = dispinterface
    ['{EE3E904C-6AA0-401A-BCB5-A936E3B717F6}']
    procedure Quit(SaveChanges: WordBool); dispid 1;
    procedure ScreenRefresh; dispid 2;
    procedure Resize(Width: Integer; Height: Integer); dispid 3;
    procedure Activate; dispid 4;
    function OpenProject(const Path: WideString): IProjectDocument; dispid 5;
    function CreateProject(const Path: WideString): IProjectDocument; dispid 9;
    property Visible: WordBool dispid 6;
    property ActiveDocument: IProjectDocument readonly dispid 7;
    property MRUDocument: WideString readonly dispid 8;
  end;

// *********************************************************************//
// The Class CoProjectDocument provides a Create and CreateRemote method to          
// create instances of the default interface IProjectDocument exposed by              
// the CoClass ProjectDocument. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoProjectDocument = class
    class function Create: IProjectDocument;
    class function CreateRemote(const MachineName: string): IProjectDocument;
  end;


// *********************************************************************//
// OLE Server Proxy class declaration
// Server Object    : TProjectDocument
// Help String      : ProjectDocument Class
// Default Interface: IProjectDocument
// Def. Intf. DISP? : No
// Event   Interface: 
// TypeFlags        : (2) CanCreate
// *********************************************************************//
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
  TProjectDocumentProperties= class;
{$ENDIF}
  TProjectDocument = class(TOleServer)
  private
    FIntf:        IProjectDocument;
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
    FProps:       TProjectDocumentProperties;
    function      GetServerProperties: TProjectDocumentProperties;
{$ENDIF}
    function      GetDefaultInterface: IProjectDocument;
  protected
    procedure InitServerData; override;
    function Get_Application: IApplication;
    function Get_Modified: WordBool;
    procedure Set_Modified(pVal: WordBool);
    function Get_Active: WordBool;
    procedure Set_Active(pVal: WordBool);
    function Get_TargetPath: WideString;
    function Get_TargetDirectory: WideString;
    function Get_Notes: WideString;
    procedure Set_Notes(const pVal: WideString);
    function Get_Params: WideString;
    procedure Set_Params(const pVal: WideString);
    function Get_Switches: WideString;
    procedure Set_Switches(const pVal: WideString);
    function Get_LangGens: WideString;
    procedure Set_LangGens(const pVal: WideString);
  public
    constructor Create(AOwner: TComponent); override;
    destructor  Destroy; override;
    procedure Connect; override;
    procedure ConnectTo(svrIntf: IProjectDocument);
    procedure Disconnect; override;
    procedure Save;
    function SaveAll: WordBool;
    function Build: WordBool;
    function Run(out ExitCode: Integer): WordBool;
    procedure Clean;
    function AddFile(const FilePath: WideString): WordBool;
    function RemoveFile(const FilePath: WideString): WordBool;
    function GetFileFirst(out FileName: WideString): WordBool;
    function GetFileNext(out FileName: WideString): WordBool;
    property DefaultInterface: IProjectDocument read GetDefaultInterface;
    property Application: IApplication read Get_Application;
    property TargetPath: WideString read Get_TargetPath;
    property TargetDirectory: WideString read Get_TargetDirectory;
    property Modified: WordBool read Get_Modified write Set_Modified;
    property Active: WordBool read Get_Active write Set_Active;
    property Notes: WideString read Get_Notes write Set_Notes;
    property Params: WideString read Get_Params write Set_Params;
    property Switches: WideString read Get_Switches write Set_Switches;
    property LangGens: WideString read Get_LangGens write Set_LangGens;
  published
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
    property Server: TProjectDocumentProperties read GetServerProperties;
{$ENDIF}
  end;

{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
// *********************************************************************//
// OLE Server Properties Proxy Class
// Server Object    : TProjectDocument
// (This object is used by the IDE's Property Inspector to allow editing
//  of the properties of this server)
// *********************************************************************//
 TProjectDocumentProperties = class(TPersistent)
  private
    FServer:    TProjectDocument;
    function    GetDefaultInterface: IProjectDocument;
    constructor Create(AServer: TProjectDocument);
  protected
    function Get_Application: IApplication;
    function Get_Modified: WordBool;
    procedure Set_Modified(pVal: WordBool);
    function Get_Active: WordBool;
    procedure Set_Active(pVal: WordBool);
    function Get_TargetPath: WideString;
    function Get_TargetDirectory: WideString;
    function Get_Notes: WideString;
    procedure Set_Notes(const pVal: WideString);
    function Get_Params: WideString;
    procedure Set_Params(const pVal: WideString);
    function Get_Switches: WideString;
    procedure Set_Switches(const pVal: WideString);
    function Get_LangGens: WideString;
    procedure Set_LangGens(const pVal: WideString);
  public
    property DefaultInterface: IProjectDocument read GetDefaultInterface;
  published
    property Modified: WordBool read Get_Modified write Set_Modified;
    property Active: WordBool read Get_Active write Set_Active;
    property Notes: WideString read Get_Notes write Set_Notes;
    property Params: WideString read Get_Params write Set_Params;
    property Switches: WideString read Get_Switches write Set_Switches;
    property LangGens: WideString read Get_LangGens write Set_LangGens;
  end;
{$ENDIF}


// *********************************************************************//
// The Class CoApplication provides a Create and CreateRemote method to          
// create instances of the default interface IApplication exposed by              
// the CoClass Application. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoApplication = class
    class function Create: IApplication;
    class function CreateRemote(const MachineName: string): IApplication;
  end;


// *********************************************************************//
// OLE Server Proxy class declaration
// Server Object    : TApplication
// Help String      : Application Class
// Default Interface: IApplication
// Def. Intf. DISP? : No
// Event   Interface: 
// TypeFlags        : (2) CanCreate
// *********************************************************************//
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
  TApplicationProperties= class;
{$ENDIF}
  TApplication = class(TOleServer)
  private
    FIntf:        IApplication;
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
    FProps:       TApplicationProperties;
    function      GetServerProperties: TApplicationProperties;
{$ENDIF}
    function      GetDefaultInterface: IApplication;
  protected
    procedure InitServerData; override;
    function Get_Visible: WordBool;
    procedure Set_Visible(pVal: WordBool);
    function Get_ActiveDocument: IProjectDocument;
    function Get_MRUDocument: WideString;
  public
    constructor Create(AOwner: TComponent); override;
    destructor  Destroy; override;
    procedure Connect; override;
    procedure ConnectTo(svrIntf: IApplication);
    procedure Disconnect; override;
    procedure Quit(SaveChanges: WordBool);
    procedure ScreenRefresh;
    procedure Resize(Width: Integer; Height: Integer);
    procedure Activate;
    function OpenProject(const Path: WideString): IProjectDocument;
    function CreateProject(const Path: WideString): IProjectDocument;
    property DefaultInterface: IApplication read GetDefaultInterface;
    property ActiveDocument: IProjectDocument read Get_ActiveDocument;
    property MRUDocument: WideString read Get_MRUDocument;
    property Visible: WordBool read Get_Visible write Set_Visible;
  published
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
    property Server: TApplicationProperties read GetServerProperties;
{$ENDIF}
  end;

{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
// *********************************************************************//
// OLE Server Properties Proxy Class
// Server Object    : TApplication
// (This object is used by the IDE's Property Inspector to allow editing
//  of the properties of this server)
// *********************************************************************//
 TApplicationProperties = class(TPersistent)
  private
    FServer:    TApplication;
    function    GetDefaultInterface: IApplication;
    constructor Create(AServer: TApplication);
  protected
    function Get_Visible: WordBool;
    procedure Set_Visible(pVal: WordBool);
    function Get_ActiveDocument: IProjectDocument;
    function Get_MRUDocument: WideString;
  public
    property DefaultInterface: IApplication read GetDefaultInterface;
  published
    property Visible: WordBool read Get_Visible write Set_Visible;
  end;
{$ENDIF}


procedure Register;

resourcestring
  dtlServerPage = 'ActiveX';

  dtlOcxPage = 'ActiveX';

implementation

uses ComObj;

class function CoProjectDocument.Create: IProjectDocument;
begin
  Result := CreateComObject(CLASS_ProjectDocument) as IProjectDocument;
end;

class function CoProjectDocument.CreateRemote(const MachineName: string): IProjectDocument;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_ProjectDocument) as IProjectDocument;
end;

procedure TProjectDocument.InitServerData;
const
  CServerData: TServerData = (
    ClassID:   '{206DDA10-728E-4C72-81CC-36FB3BE80916}';
    IntfIID:   '{C7242A99-D06C-4A9E-818D-5D3180EB8D99}';
    EventIID:  '';
    LicenseKey: nil;
    Version: 500);
begin
  ServerData := @CServerData;
end;

procedure TProjectDocument.Connect;
var
  punk: IUnknown;
begin
  if FIntf = nil then
  begin
    punk := GetServer;
    Fintf:= punk as IProjectDocument;
  end;
end;

procedure TProjectDocument.ConnectTo(svrIntf: IProjectDocument);
begin
  Disconnect;
  FIntf := svrIntf;
end;

procedure TProjectDocument.DisConnect;
begin
  if Fintf <> nil then
  begin
    FIntf := nil;
  end;
end;

function TProjectDocument.GetDefaultInterface: IProjectDocument;
begin
  if FIntf = nil then
    Connect;
  Assert(FIntf <> nil, 'DefaultInterface is NULL. Component is not connected to Server. You must call ''Connect'' or ''ConnectTo'' before this operation');
  Result := FIntf;
end;

constructor TProjectDocument.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
  FProps := TProjectDocumentProperties.Create(Self);
{$ENDIF}
end;

destructor TProjectDocument.Destroy;
begin
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
  FProps.Free;
{$ENDIF}
  inherited Destroy;
end;

{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
function TProjectDocument.GetServerProperties: TProjectDocumentProperties;
begin
  Result := FProps;
end;
{$ENDIF}

function TProjectDocument.Get_Application: IApplication;
begin
    Result := DefaultInterface.Application;
end;

function TProjectDocument.Get_Modified: WordBool;
begin
    Result := DefaultInterface.Modified;
end;

procedure TProjectDocument.Set_Modified(pVal: WordBool);
begin
  DefaultInterface.Set_Modified(pVal);
end;

function TProjectDocument.Get_Active: WordBool;
begin
    Result := DefaultInterface.Active;
end;

procedure TProjectDocument.Set_Active(pVal: WordBool);
begin
  DefaultInterface.Set_Active(pVal);
end;

function TProjectDocument.Get_TargetPath: WideString;
begin
    Result := DefaultInterface.TargetPath;
end;

function TProjectDocument.Get_TargetDirectory: WideString;
begin
    Result := DefaultInterface.TargetDirectory;
end;

function TProjectDocument.Get_Notes: WideString;
begin
    Result := DefaultInterface.Notes;
end;

procedure TProjectDocument.Set_Notes(const pVal: WideString);
  { Warning: The property Notes has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.Notes := pVal;
end;

function TProjectDocument.Get_Params: WideString;
begin
    Result := DefaultInterface.Params;
end;

procedure TProjectDocument.Set_Params(const pVal: WideString);
  { Warning: The property Params has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.Params := pVal;
end;

function TProjectDocument.Get_Switches: WideString;
begin
    Result := DefaultInterface.Switches;
end;

procedure TProjectDocument.Set_Switches(const pVal: WideString);
  { Warning: The property Switches has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.Switches := pVal;
end;

function TProjectDocument.Get_LangGens: WideString;
begin
    Result := DefaultInterface.LangGens;
end;

procedure TProjectDocument.Set_LangGens(const pVal: WideString);
  { Warning: The property LangGens has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.LangGens := pVal;
end;

procedure TProjectDocument.Save;
begin
  DefaultInterface.Save;
end;

function TProjectDocument.SaveAll: WordBool;
begin
  Result := DefaultInterface.SaveAll;
end;

function TProjectDocument.Build: WordBool;
begin
  Result := DefaultInterface.Build;
end;

function TProjectDocument.Run(out ExitCode: Integer): WordBool;
begin
  Result := DefaultInterface.Run(ExitCode);
end;

procedure TProjectDocument.Clean;
begin
  DefaultInterface.Clean;
end;

function TProjectDocument.AddFile(const FilePath: WideString): WordBool;
begin
  Result := DefaultInterface.AddFile(FilePath);
end;

function TProjectDocument.RemoveFile(const FilePath: WideString): WordBool;
begin
  Result := DefaultInterface.RemoveFile(FilePath);
end;

function TProjectDocument.GetFileFirst(out FileName: WideString): WordBool;
begin
  Result := DefaultInterface.GetFileFirst(FileName);
end;

function TProjectDocument.GetFileNext(out FileName: WideString): WordBool;
begin
  Result := DefaultInterface.GetFileNext(FileName);
end;

{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
constructor TProjectDocumentProperties.Create(AServer: TProjectDocument);
begin
  inherited Create;
  FServer := AServer;
end;

function TProjectDocumentProperties.GetDefaultInterface: IProjectDocument;
begin
  Result := FServer.DefaultInterface;
end;

function TProjectDocumentProperties.Get_Application: IApplication;
begin
    Result := DefaultInterface.Application;
end;

function TProjectDocumentProperties.Get_Modified: WordBool;
begin
    Result := DefaultInterface.Modified;
end;

procedure TProjectDocumentProperties.Set_Modified(pVal: WordBool);
begin
  DefaultInterface.Set_Modified(pVal);
end;

function TProjectDocumentProperties.Get_Active: WordBool;
begin
    Result := DefaultInterface.Active;
end;

procedure TProjectDocumentProperties.Set_Active(pVal: WordBool);
begin
  DefaultInterface.Set_Active(pVal);
end;

function TProjectDocumentProperties.Get_TargetPath: WideString;
begin
    Result := DefaultInterface.TargetPath;
end;

function TProjectDocumentProperties.Get_TargetDirectory: WideString;
begin
    Result := DefaultInterface.TargetDirectory;
end;

function TProjectDocumentProperties.Get_Notes: WideString;
begin
    Result := DefaultInterface.Notes;
end;

procedure TProjectDocumentProperties.Set_Notes(const pVal: WideString);
  { Warning: The property Notes has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.Notes := pVal;
end;

function TProjectDocumentProperties.Get_Params: WideString;
begin
    Result := DefaultInterface.Params;
end;

procedure TProjectDocumentProperties.Set_Params(const pVal: WideString);
  { Warning: The property Params has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.Params := pVal;
end;

function TProjectDocumentProperties.Get_Switches: WideString;
begin
    Result := DefaultInterface.Switches;
end;

procedure TProjectDocumentProperties.Set_Switches(const pVal: WideString);
  { Warning: The property Switches has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.Switches := pVal;
end;

function TProjectDocumentProperties.Get_LangGens: WideString;
begin
    Result := DefaultInterface.LangGens;
end;

procedure TProjectDocumentProperties.Set_LangGens(const pVal: WideString);
  { Warning: The property LangGens has a setter and a getter whose
    types do not match. Delphi was unable to generate a property of
    this sort and so is using a Variant as a passthrough. }
var
  InterfaceVariant: OleVariant;
begin
  InterfaceVariant := DefaultInterface;
  InterfaceVariant.LangGens := pVal;
end;

{$ENDIF}

class function CoApplication.Create: IApplication;
begin
  Result := CreateComObject(CLASS_Application) as IApplication;
end;

class function CoApplication.CreateRemote(const MachineName: string): IApplication;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Application) as IApplication;
end;

procedure TApplication.InitServerData;
const
  CServerData: TServerData = (
    ClassID:   '{ADC6D969-C5D5-49E4-A75C-F58156BDDBC9}';
    IntfIID:   '{EE3E904C-6AA0-401A-BCB5-A936E3B717F6}';
    EventIID:  '';
    LicenseKey: nil;
    Version: 500);
begin
  ServerData := @CServerData;
end;

procedure TApplication.Connect;
var
  punk: IUnknown;
begin
  if FIntf = nil then
  begin
    punk := GetServer;
    Fintf:= punk as IApplication;
  end;
end;

procedure TApplication.ConnectTo(svrIntf: IApplication);
begin
  Disconnect;
  FIntf := svrIntf;
end;

procedure TApplication.DisConnect;
begin
  if Fintf <> nil then
  begin
    FIntf := nil;
  end;
end;

function TApplication.GetDefaultInterface: IApplication;
begin
  if FIntf = nil then
    Connect;
  Assert(FIntf <> nil, 'DefaultInterface is NULL. Component is not connected to Server. You must call ''Connect'' or ''ConnectTo'' before this operation');
  Result := FIntf;
end;

constructor TApplication.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
  FProps := TApplicationProperties.Create(Self);
{$ENDIF}
end;

destructor TApplication.Destroy;
begin
{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
  FProps.Free;
{$ENDIF}
  inherited Destroy;
end;

{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
function TApplication.GetServerProperties: TApplicationProperties;
begin
  Result := FProps;
end;
{$ENDIF}

function TApplication.Get_Visible: WordBool;
begin
    Result := DefaultInterface.Visible;
end;

procedure TApplication.Set_Visible(pVal: WordBool);
begin
  DefaultInterface.Set_Visible(pVal);
end;

function TApplication.Get_ActiveDocument: IProjectDocument;
begin
    Result := DefaultInterface.ActiveDocument;
end;

function TApplication.Get_MRUDocument: WideString;
begin
    Result := DefaultInterface.MRUDocument;
end;

procedure TApplication.Quit(SaveChanges: WordBool);
begin
  DefaultInterface.Quit(SaveChanges);
end;

procedure TApplication.ScreenRefresh;
begin
  DefaultInterface.ScreenRefresh;
end;

procedure TApplication.Resize(Width: Integer; Height: Integer);
begin
  DefaultInterface.Resize(Width, Height);
end;

procedure TApplication.Activate;
begin
  DefaultInterface.Activate;
end;

function TApplication.OpenProject(const Path: WideString): IProjectDocument;
begin
  Result := DefaultInterface.OpenProject(Path);
end;

function TApplication.CreateProject(const Path: WideString): IProjectDocument;
begin
  Result := DefaultInterface.CreateProject(Path);
end;

{$IFDEF LIVE_SERVER_AT_DESIGN_TIME}
constructor TApplicationProperties.Create(AServer: TApplication);
begin
  inherited Create;
  FServer := AServer;
end;

function TApplicationProperties.GetDefaultInterface: IApplication;
begin
  Result := FServer.DefaultInterface;
end;

function TApplicationProperties.Get_Visible: WordBool;
begin
    Result := DefaultInterface.Visible;
end;

procedure TApplicationProperties.Set_Visible(pVal: WordBool);
begin
  DefaultInterface.Set_Visible(pVal);
end;

function TApplicationProperties.Get_ActiveDocument: IProjectDocument;
begin
    Result := DefaultInterface.ActiveDocument;
end;

function TApplicationProperties.Get_MRUDocument: WideString;
begin
    Result := DefaultInterface.MRUDocument;
end;

{$ENDIF}

procedure Register;
begin
  RegisterComponents(dtlServerPage, [TProjectDocument, TApplication]);
end;

end.
