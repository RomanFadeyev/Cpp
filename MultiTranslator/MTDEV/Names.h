/*************************************************************************
    created:    2001/12/25
    created:    25:12:2001   21:10
    filename:   $(Projects)\Sources\MTDEv\Names.h
    file path:  $(Projects)\Sources\MTDEv
    file base:  Names
    file ext:   h
    author:     Фадеев Р.В. (Fadeyev R.V.)
    
    purpose:    Имена, константы и т.д.
*************************************************************************/

#if !defined(Names_H__AUTO_GEN__INCLUDED_)
#define Names_H__AUTO_GEN__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define chT_SPACE		_T(" ")
#define chT_LBRACKET	_T("{\n")
#define chT_RBRACKET	_T("}\n")
#define chT_ENDL		_T("\n")
#define chT_DQUOTE		_T("\"")

#define EXT_SOURCES     _T(".grm")
#define EXT_PROJECTS    _T(".pjt")
#define EXT_MAKE		_T(".mak")
#define EXT_CLW			_T(".clw")
#define EXT_DSK			_T(".dsk")
#define EXT_SDP			_T(".sdp")	//Syntax Diagram Placement
#define EXT_STP			_T(".stp")	//Syntax Tree Placement

#define CHAR_CONST char * const
 
//==========================Настройки Registry===========================
CHAR_CONST szProfileUserInterface       = "UI";
CHAR_CONST szProfileEditor              = "Editor";
CHAR_CONST szProfilePlugins				= "Plugins";
CHAR_CONST szProfilePlugins_CodeGenerators = "Plugins\\Code Generators";
CHAR_CONST szProfileEditor_EditedFiles  = "Editor\\Edited Files";
CHAR_CONST szProfileRecentProjects      = "Projects\\Recently";
CHAR_CONST szProfileDocMngr             = "DocManager";
CHAR_CONST szProfileWorkSpace           = "WorkSpace";
CHAR_CONST szProfileOutputScreen        = "Output";
CHAR_CONST szProfileBarBreakpoints		= "Breakpoints";

CHAR_CONST szFind                       = "F&R\\Find";
CHAR_CONST szFindRecently               = "F&R\\Find\\Recently";
CHAR_CONST szFindReplace                = "F&R\\Replace";
CHAR_CONST szReplaceRecentlyFind        = "F&R\\Replace\\Recently\\Find";
CHAR_CONST szReplaceRecentlyReplace     = "F&R\\Replace\\Recently\\Replace";
CHAR_CONST szFindInFiles                = "F&R\\FindInFiles";
CHAR_CONST szFindInFilesRecently        = "F&R\\FindInFiles\\Recently";
CHAR_CONST szFindInFilesRecentlyTypes   = "F&R\\FindInFiles\\RecentlyTypes";
CHAR_CONST szSyntaxDiagram		        = "SyntaxDiagram";

CHAR_CONST szParamParsingReportType		= "Parsing Report Type";
CHAR_CONST szParamParsingReportToFile	= "Parsing Report To File";
CHAR_CONST szParamParsingStopAtMessage	= "Parsing Stop Debug At Message";
CHAR_CONST szParamIsDelegated			= "Delegated";
CHAR_CONST szParamLastDir               = "LastDir";
CHAR_CONST szParamLookIn				= "Look In";
CHAR_CONST szParamLookInSubFolders		= "Look In Subfolders";
CHAR_CONST szParamMatchWholeWord		= "Match Whole Word";
CHAR_CONST szParamMatchCase				= "Match Case";
CHAR_CONST szParamFileTypes				= "File Types";
CHAR_CONST szParamRecentProjectName     = "Project%d";
CHAR_CONST szParamRecentItem            = "Item%d";
CHAR_CONST szParamSortItemsMode         = "SortItemsMode"; //szProfileWorkSpace
CHAR_CONST szActiveTab                  = "ActiveTab";      //szProfileWorkSpace

CHAR_CONST  szParamVisible              = "Visible";        //szOutputScreen
CHAR_CONST  szParamOEM                  = "OEM-ANSI";
CHAR_CONST  szParamAltConversion        = "AltConversion";
CHAR_CONST  szParamAutoClose            = "AutoClose";
CHAR_CONST  szParamAutoClear            = "AutoClear";
CHAR_CONST  szParamTextColor            = "TextColor";
CHAR_CONST  szParamBackColor            = "BackColor";

//==========================Настройки редактора===========================
CHAR_CONST szHiLightExtension           = "xml";
CHAR_CONST szHilightEditorClass			= "HilightEditor";
CHAR_CONST szSyntaxTreeClassName		= "SyntaxTree";
CHAR_CONST szSyntaxDiagramsClassName    = "SyntaxDiagram";

//========================================================================

CHAR_CONST barnInfo             = "Information";
CHAR_CONST barnOutput           = "Output";
CHAR_CONST barnWatch            = "Watch";
CHAR_CONST barnScreen           = "Screen";
CHAR_CONST barnWorkspace        = "Workspace";
CHAR_CONST barnFullScreen       = "Full Screen";
CHAR_CONST barnBreakpoints      = "Breakpoints";

//Расположение Bar (Bar Placement Sequence)
const CHAR_CONST  bplsHorizontalBottom[2]= {barnOutput,barnWatch};
//Default UserTools
const CHAR_CONST  usrTools[2]= {"Notepad\nnotepad.exe","Windows Explorer\nexplorer.exe"};

//Название ключей в файле проекта и MAK-файле
CHAR_CONST TAG_PROJECT_SIGN     =  "Parser project";
CHAR_CONST TAG_PROJECT_PARAMS   =  "Parameters:";
CHAR_CONST TAG_PROJECT_NOTES    =  "Notes:";
CHAR_CONST TAG_PROJECT_SRC_LANG =  "Source Language:";
CHAR_CONST TAG_PROJECT_DST_LANG =  "Target Language:";
CHAR_CONST TAG_PROJECT_PARAMS_BROWSE_DIR    =  "ParamBrowseDir:";
CHAR_CONST TAG_PROJECT_DSK      =  "Deskview:";
CHAR_CONST TAG_PROJECT_POSNS    =  "Positions:";
CHAR_CONST TAG_PROJECT_BKMKS    =  "Boormarks:";
CHAR_CONST TAG_PROJECT_BPS      =  "Breakpoints:";
CHAR_CONST TAG_PROJECT_LAST_MODIFIED=  "LastModified:";
CHAR_CONST TAG_PROJECT_ACTIVE_DOC=  "Active Document:";

//
CHAR_CONST szKnowledgeFileExtension = "kb";

#endif //!defined(Names_H__AUTO_GEN__INCLUDED_)
