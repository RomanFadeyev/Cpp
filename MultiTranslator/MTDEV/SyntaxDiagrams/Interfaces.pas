{*******************************************************************************

  Описание внешних структур и данных, которые будут доступны
  для пользовательских программ

********************************************************************************}

unit Interfaces;

interface
  uses Windows,Messages;

const
  STClassName = 'SyntaxTree';
  SDClassName = 'SyntaxDiagram';  

  //Term Class
const
  TC_TERMINAL    = 1;
  TC_NONTERMINAL  = 2;

type
  HRULE = integer;
  THRuleArray = array [0..$FFFF] of HRULE;
  PHRuleArray = ^ THRuleArray;

const
  ttTERMINAL = 1;
  ttNON_TERMINAL = 2;
  ttSTD_TERMINAL = 3;  
type
  TSTTermInfo = packed record
    szName: pchar;
    nType : integer; //TERMINAL = 1, TC_NONTERMINAL =2
  end;

  TSTTermArray = array [0..$FFFF] of TSTTermInfo;
  pTSTTermArray = ^TSTTermArray;

  TSTVariantInfo = packed record
    szName: pchar;
    nTerms : integer;
    pTerms : pTSTTermArray;
  end;
  PSTVariantInfo = ^ TSTVariantInfo;
  TSTVariantArray = array [0..$FFFF] of TSTVariantInfo;
  pTSTVariantArray = ^TSTVariantArray;

  TSTRuleInfo = packed record
    szName     : pchar;
    nVariants  : integer;
    pVariants  : pTSTVariantArray;
  end;
  PSTRuleInfo = ^ TSTRuleInfo;

  //----------------------------------------------------------------------------
  //---------------------------- Messages --------------------------------------

  TSTMAddRule = packed record
    Msg         : Cardinal;
    hParentRule : HRULE;
    RuleStruct  : PSTRuleInfo;
    Result      : Longint; //1 - OK, 0 - Fail
  end;

  TSTMGetParentCount = packed record
    Msg         : Cardinal;
    hRule       : HRULE;
    ParentCount : integer;
    Result      : Longint; //1 - OK, 0 - Fail
  end;

  TSTMGetParents = packed record
    Msg         : Cardinal;
    hRule       : HRULE;
    hParentRules: PHRuleArray;
    Result      : Longint; //1 - OK, 0 - Fail
  end;

  TSTMBeginUpdate = packed record
    Msg         : Cardinal;
    WParam      : WPARAM;
    LParam      : LPARAM;
    Result      : Longint; //LockCount
  end;

  TSTMEndUpdate = TSTMBeginUpdate;

  TSTMSetRegKey = packed record
    Msg         : Cardinal;
    Key         : pchar;
    LParam      : LPARAM;
    Result      : Longint; //1 or 0
  end;

  TSTMSetScale= packed record
    Msg         : Cardinal;
    Scale       : Cardinal;
    LParam      : LPARAM;
    Result      : Longint; //1 or 0
  end;

  TSTMLoadData= packed record
    Msg         : Cardinal;
    FileName    : pchar;
    LParam      : LPARAM;
    Result      : LongBool; //1 or 0
  end;

  TSTMSaveData= packed record
    Msg         : Cardinal;
    FileName    : pchar;
    LParam      : LPARAM;
    Result      : LongBool; //1 or 0
  end;

  TSTMLoadPlacement = TSTMLoadData;
  TSTMSavePlacement = TSTMSaveData;

const
  STM_BASE     = WM_USER+$2000;
  STM_CLEAR    = STM_BASE+1;     // (0,0)-> void Очистка всего содержимого
  STM_ARRANGE  = STM_BASE+2;     // (0,0)-> HRULE
  STM_ADDRULE  = STM_BASE+3;     // (<not used>, pSTRuleInfo) -> HRULE
  STM_GETRULEPARENTCOUNT  = STM_BASE+4;     // (HRULE hRule, OUT Integer ParentCount) -> boolean
  STM_GETRULEPARENTS      = STM_BASE+5;     // (HRULE hRule, HRULE* Parents) -> boolean
  STM_BEGINUPDATE         = STM_BASE+6;     // (<none> <none>) -> LockCount
  STM_ENDUPDATE           = STM_BASE+7;     // (<none> <none>) -> LockCount
  STM_SETREGKEY           = STM_BASE+8;     // (pchar, none)
  STM_GETSCALE            = STM_BASE+9;     // (none, none)
  STM_SETSCALE            = STM_BASE+10;     //(integer (in percents), none)
  STM_LOADDATA            = STM_BASE+11;     //(pchar, none) -> boolean
  STM_SAVEDATA            = STM_BASE+12;     //(pchar, none) -> boolean
  STM_LOADPLACEMENT       = STM_BASE+13;     //(pchar, none) -> boolean
  STM_SAVEPLACEMENT       = STM_BASE+14;     //(pchar, none) -> boolean

implementation

end.
