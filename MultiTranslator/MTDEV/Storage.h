/********************************************************************
created:	2004/07/06
created:	6:7:2004   23:39
filename: 	X:\MultiTranslator\sources\mtdev\storage.h
file path:	X:\MultiTranslator\sources\mtdev
file base:	storage
file ext:	h
author:		Fadeyev R.V.

purpose:	Реализация сохранения настроек программы
*********************************************************************/
#pragma once

#include "BaseStructs.h"
#include "CobjectEx.h"	//Один из родителей

#define READ_ENVIRONMENT(default_value) {*Value = GetProperty("Environment",__FUNCTION__,default_value); return S_OK;}
#define WRITE_ENVIRONMENT(aParam) {SetProperty("Environment",__FUNCTION__,Value, aParam);return S_OK;}

#define READ_ENVIRONMENT_INT(default_value) {(*(long*)Value) = GetProperty("Environment",__FUNCTION__,(long)default_value); return S_OK;}
#define WRITE_ENVIRONMENT_INT(aParam) {SetProperty("Environment",__FUNCTION__,(long)Value, aParam);return S_OK;}

#define READ_ENVIRONMENT_BSTR(default_value) {_bstr_t x = GetProperty("Environment",__FUNCTION__,default_value).as_cstr(); *Value = x.Detach();  return S_OK;}
#define WRITE_ENVIRONMENT_BSTR(aParam) {SetProperty("Environment",__FUNCTION__,(cstr)_bstr_t(Value), aParam);return S_OK;}

class CStorage : public IStorageLink,public CCObjectEx
{
private:
	static CStorage * m_pInstance; 
	void GetRegistryPath(cstr szSection, cstr szFunctionName, OUT string_smart & strRegSection, OUT string_smart & strRegEntry);
	variant		 GetProperty(cstr szSection, cstr szFunctionName, const variant & default_value);
	void		 SetProperty(cstr szSection,cstr szFunctionName, const variant & value, IStorageLink::EParam nParam);
public:
	CStorage(void);
	virtual ~CStorage(void);

	//IUnknown
	DEFINE_ADD_REF_STUB
	DEFINE_RELEASE_STUB

	BEGIN_QUERY_INTERFACE 
		COM_INTERFACE_SUPPORT(mtdev_lib::IPreferencesDataProvider)
	END_QUERY_INTERFACE


	//mtdev_lib::IPreferencesProvider
	//---- General ----
	override HRESULT __stdcall get_General_MDIStyle (mtdev_lib::EPreferences_MDIStyle * Value ) READ_ENVIRONMENT_INT(mtdev_lib::Tabs)
	override HRESULT __stdcall put_General_MDIStyle (mtdev_lib::EPreferences_MDIStyle Value) WRITE_ENVIRONMENT_INT(pmGeneral_MDIStyle)

	override HRESULT __stdcall get_General_StartupAction (mtdev_lib::EPreferences_StartupAction * Value) READ_ENVIRONMENT_INT(mtdev_lib::DoNothing)
	override HRESULT __stdcall put_General_StartupAction (mtdev_lib::EPreferences_StartupAction Value) WRITE_ENVIRONMENT_INT(pmGeneral_StartupAction)

	override HRESULT __stdcall get_General_MRUSize (long * Value) READ_ENVIRONMENT(8)
	override HRESULT __stdcall put_General_MRUSize (long Value) WRITE_ENVIRONMENT(pmGeneral_MRUSize)

	override HRESULT __stdcall get_General_DetectChangesOutside (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_General_DetectChangesOutside (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmGeneral_DetectChangesOutside)

	override HRESULT __stdcall get_General_AllowEditReadOnlyFiles (VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_General_AllowEditReadOnlyFiles (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmGeneral_AllowEditReadOnlyFiles)

	override HRESULT __stdcall get_General_OpenFileUsingActiveDocDir (VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_General_OpenFileUsingActiveDocDir (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmGeneral_OpenFileUsingActiveDocDir)

	override HRESULT __stdcall get_General_AutosaveFiles (VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_General_AutosaveFiles (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmGeneral_AutosaveFiles)

	override HRESULT __stdcall get_General_AutosaveEachNMinutes (long * Value) READ_ENVIRONMENT(15)
	override HRESULT __stdcall put_General_AutosaveEachNMinutes (long Value) WRITE_ENVIRONMENT(pmGeneral_AutosaveEachNMinutes)

	override HRESULT __stdcall get_General_SaveAllBeforeRun  (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
    override HRESULT __stdcall put_General_SaveAllBeforeRun (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmGeneral_SaveAllFilesBeforeRun)

	override HRESULT __stdcall get_General_TearOffMenus (VARIANT_BOOL * Value ) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_General_TearOffMenus (VARIANT_BOOL Value ) WRITE_ENVIRONMENT(pmGeneral_TearOffMenus)

	//---- Find and replace ----
	override HRESULT __stdcall get_Find_ShowTipNotMsgBox (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Find_ShowTipNotMsgBox (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmFind_ShowTipNotMsgBox)

	override HRESULT __stdcall get_Find_PutTheText (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Find_PutTheText (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmFind_PutTheText)

	//---- Editor ----
	override HRESULT __stdcall get_Editor_UseDragDrop (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_UseDragDrop (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_UseDragDrop)

	override HRESULT __stdcall get_Editor_KeepUndoAfterSave (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_KeepUndoAfterSave (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_KeepUndoAfterSave)

	override HRESULT __stdcall get_Editor_CreateBackupFiles (VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_Editor_CreateBackupFiles (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_CreateBackupFiles)

	override HRESULT __stdcall get_Editor_ShowSelectionMargin (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_ShowSelectionMargin (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowSelectionMargin)

	override HRESULT __stdcall get_Editor_ShowFoldMargin(VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_ShowFoldMargin (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowFoldMargin)

	override HRESULT __stdcall get_Editor_ShowNumberMargin(VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_Editor_ShowNumberMargin (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowNumberMargin)

	override HRESULT __stdcall get_Editor_ShowVertSB (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_ShowVertSB (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowVertSB)

	override HRESULT __stdcall get_Editor_ShowHorzSB (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_ShowHorzSB (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowHorzSB)

	override HRESULT __stdcall get_Editor_ShowRightMargin (VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_Editor_ShowRightMargin (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowRightMargin)

	override HRESULT __stdcall get_Editor_ShowRightMarginAt (long * Value) READ_ENVIRONMENT(80)
	override HRESULT __stdcall put_Editor_ShowRightMarginAt (long Value) WRITE_ENVIRONMENT(pmEditor_ShowRightMarginAt )

	override HRESULT __stdcall get_Editor_ShowWhiteSpaces (VARIANT_BOOL * Value) READ_ENVIRONMENT(FALSE)
	override HRESULT __stdcall put_Editor_ShowWhiteSpaces (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowWhiteSpaces )
	
	override HRESULT __stdcall get_Editor_ShowIndentGuides(VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Editor_ShowIndentGuides(VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmEditor_ShowIndentGuides)

	//---- Auto-update ----
	override HRESULT __stdcall get_Update_UseAutoUpdate (VARIANT_BOOL * Value) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_Update_UseAutoUpdate (VARIANT_BOOL Value) WRITE_ENVIRONMENT(pmUpdate_UseAutoUpdate)

	override HRESULT __stdcall get_Update_CheckMode (mtdev_lib::EPreferences_AutoupdateMode * Value) READ_ENVIRONMENT_INT(mtdev_lib::CheckEveryStart)
	override HRESULT __stdcall put_Update_CheckMode (mtdev_lib::EPreferences_AutoupdateMode Value) WRITE_ENVIRONMENT_INT(pmUpdate_CheckMode)

	override HRESULT __stdcall get_Update_CheckEveryNDays (long * Value) READ_ENVIRONMENT(15)
	override HRESULT __stdcall put_Update_CheckEveryNDays (long Value) WRITE_ENVIRONMENT(pmUpdate_CheckEveryNDays)

	//---- Bug Report ----
	override HRESULT __stdcall get_BugReport_UseBugReport (VARIANT_BOOL * Value ) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_BugReport_UseBugReport (VARIANT_BOOL Value ) WRITE_ENVIRONMENT(pmBugReport_UseBugReport)

	override HRESULT __stdcall get_BugReport_Address (BSTR * Value ) READ_ENVIRONMENT_BSTR(ApplicationData::APP_EMAIL)
	override HRESULT __stdcall put_BugReport_Address (BSTR Value ) WRITE_ENVIRONMENT_BSTR(pmBugReport_Address)

	override HRESULT __stdcall get_BugReport_IncludeOSInfo (VARIANT_BOOL * Value ) READ_ENVIRONMENT(TRUE)
	override HRESULT __stdcall put_BugReport_IncludeOSInfo (VARIANT_BOOL Value ) WRITE_ENVIRONMENT(pmBugReport_IncludeOSInfo)

	//IStorage
	override string_smart GetLastOpenedProject() {return GetProperty("Environment",__FUNCSIG__,"").as_cstr();}
	override void SetLastOpenedProject(string_adapter value) {SetProperty("Environment",__FUNCSIG__,value.c_str(),pmLastOpenedProject);}
};
