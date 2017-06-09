/********************************************************************
created:		2002/02/25
created:		25:2:2002	12:55
filename:		$(Projects)\Sources\MTDEv\Docs&Views\ProjectDoc.cpp
file path:		$(Projects)\Sources\MTDEv\Docs&Views

file base:		ProjectDoc
file ext:		cpp
author:			Fadeyev	R.V.

purpose:		Реализация документа проекта (Project)
*********************************************************************/

#include "stdafx.h"
#include "ProjectDoc.h"
#include "BaseStructs.h"
#include "Names.h"
#include "EditionRestrictions.h"

#include <ModuleNames.h>
#include <GrmcExports.h>

using namespace grmc_exports;

//Статический счетчик документов, нужен	для	получения уникального имени	объекта
long CProjectDoc::g_nID=0;			

#define PARSE_REPORT_FULL  1
#define PARSE_REPORT_SHORT 2
int  CProjectDoc::m_nParsingReportType=PARSE_REPORT_SHORT;
bool CProjectDoc::m_bParsingReportToFile=false;
bool CProjectDoc::m_bParsingStopAtMessage=false;



//Добавляем	к названию пункта меню название	проекта
#define	MENU_ADD_PROJECT_NAME(pCmdUI)\
	if (pCmdUI->m_pMenu){\
		static string_smart	strOriginal(CCmdUIEx(pCmdUI).GetMenuItemText());\
		pCmdUI->SetText(string_smart(strOriginal," <",GetName(),">").c_str());\
	}

//////////////////////////////////////////////////////////////////////////
// CProjectDoc::CProjectDocLink
//////////////////////////////////////////////////////////////////////////

//Добавляет	файл в проект. Derived from IProjectDoc
bool CProjectDoc::AddFile(cstr szFileName){
	return FileAdd(szFileName);
}
//------------------------------------------------------------------------
IDocumentLink* CProjectDoc::AddNewFile()
{
	INewDocumentWizardLink * pNewProjectItemLink = i_objects::GetInterface<INewDocumentWizardLink>();
	ASSERT_RETURN_NULL(pNewProjectItemLink);

	return pNewProjectItemLink->Run(this);
}
//------------------------------------------------------------------------
//Derived from IProjectDoc
bool CProjectDoc::AddRule(){
	return i_objects::GetInterface<IAddRuleLink>()->Run();
}
//------------------------------------------------------------------------
//Удаляет файл из проекта. Derived from IProjectDoc
bool CProjectDoc::RemoveFile(cstr szFileName){
	return FileRemove(szFileName);
}
//------------------------------------------------------------------------
//Возвращает путь к	проекту. Derived from IProjectDoc
string_smart CProjectDoc::GetPath() const{
	return m_strPath.c_str();
}
//------------------------------------------------------------------------
//Возвращает путь к	скомпилированному исполняемому файлу. Derived from IProjectDoc
string_smart CProjectDoc::GetExePath() const{
	
	return 	 string_smart(	include_backslash(GetOutputDirectoryFull()),
							GetName(),
							(GetSwitches().pos(szSwitch_SFX)==-1)
								? ".bin"
								: ".exe"
						);
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetOutputDirectory() const
{
	return m_strOutputDirectory.c_str();
}
//------------------------------------------------------------------------
void CProjectDoc::SetOutputDirectory(string_adapter szDirectory)
{
	if (m_strOutputDirectory==szDirectory)
		return;
	m_strOutputDirectory=szDirectory;
	SetModify(true);
}
//------------------------------------------------------------------------
//Директория промежуточных файлов, может быть относительный путь
string_smart CProjectDoc::GetIntermediateDirectory() const
{
	return m_strIntermediateDirectory.c_str();
}
//------------------------------------------------------------------------
void CProjectDoc::SetIntermediateDirectory(string_adapter szDirectory)
{
	if (m_strIntermediateDirectory==szDirectory)
		return;
	m_strIntermediateDirectory=szDirectory;
	SetModify(true);
}
//------------------------------------------------------------------------
//Возвращает директорию, где будут создаваться исполняемые файлы. Derived from IProjectDoc
string_smart CProjectDoc::GetOutputDirectoryFull() const {
	if (m_strOutputDirectory.isempty())
		return include_backslash(m_strWorkDir);
	
	return get_abs_path(m_strOutputDirectory,m_strWorkDir);
}
//------------------------------------------------------------------------
//Возвращает директорию, где будут создаваться исполняемые файлы. Derived from IProjectDoc
string_smart CProjectDoc::GetIntermediateDirectoryFull() const {
	if (m_strIntermediateDirectory.isempty())
		return include_backslash(m_strWorkDir);
	
	return get_abs_path(m_strIntermediateDirectory,m_strWorkDir);
}
//------------------------------------------------------------------------
//Возвращает название проекта. Derived from IProjectDoc
string_smart CProjectDoc::GetName() const 
{
	//Возвращает заголовок проекта.	Аналогия GetTitle, но лучше	пользоаться	этой
	//т.к. стандартные имена устанавливаются после открытия	файла, а нужно 
	//зачастую иметь имена во время	открытия
	return m_strTitle.c_str();
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetExt() const
{
	if (GetPath().isempty()){
		CString str;
		GetDocTemplate()->GetDocString(str,CDocTemplate::filterExt);
		return (LPCTSTR) str;
	}
	return extract_fileext(GetPath());
}
//------------------------------------------------------------------------
bool CProjectDoc::IsReadOnly() const
{
	return m_bReadOnly;
}
//------------------------------------------------------------------------
//Проверяет, входит	ди в состав	проекта	указанный файл. Derived from IProjectDoc
bool CProjectDoc::IsFilePresent(const char*	szFileName) const {
	return FileFind(szFileName)!=m_lstSources.end();
}
//------------------------------------------------------------------------
//Устанавливает	флаг изменения документа. Derived from IProjectDoc
void CProjectDoc::SetModify(bool bModify){
	SetModifiedFlag(bModify);
}
//------------------------------------------------------------------------
//Устанавливает	флаг изменения документа. Derived from IProjectDoc
void CProjectDoc::SetActive(bool bActive){
	(bActive) ?	Activate(true) : Deactivate();
}
//------------------------------------------------------------------------
//Derived from IProjectDoc
bool CProjectDoc::GetActive() const {
	IDocMngrLink * pDocMngrLink	= GetDocManagerInterface();
	if (pDocMngrLink)
		return pDocMngrLink->GetActiveProject()==const_cast<CProjectDoc*>(this);
	else
		return false;
}
//------------------------------------------------------------------------
ref_cnt_ptr<COM::IProjectDocument> CProjectDoc::GetComInterface2()
{
	if (!m_pCoClass)
	{
		m_pCoClass = new CComObject<COM::ProjectDocument>;
		m_pCoClass->LinkToOwner(this);
	}
	return m_pCoClass;
}
//------------------------------------------------------------------------
//Возвращает атрибут принадлежности	файла к	какой-либо группе файлов. Derived from IProjectDoc
int	CProjectDoc::GetFileType(cstr szSource) const {
	return (is_file_of_group(szSource,EXT_SOURCES))
		? fileSource 
		: fileOther;	
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetParseLogPath() const
{
	return change_fileext(GetPath(),".rpt");
}
//------------------------------------------------------------------------
//Закрывает	документ. Derived from IProjectDoc
bool CProjectDoc::Close()
{
	return OnCloseDocumentInternal();
}
//------------------------------------------------------------------------
//Сохраняет	документ. Derived from IProjectDoc
bool CProjectDoc::Save()
{
	bool res = (OnSaveDocument(m_strPath.c_str())!=FALSE);
	SaveDSK();
	return res;
}
//------------------------------------------------------------------------
bool CProjectDoc::SaveAs(cstr szNewFileName)
{
	if (DoSave(szNewFileName)){
		bool bHasNotLoaded = m_strPath.isempty();

		//Реинициализируем внутренние переменные
		m_strPath = szNewFileName;
		m_strWorkDir= extract_filedir(szNewFileName);
		m_strTitle	= extract_filename(szNewFileName);

		//Проект еще не был создан и сохранен
		if (bHasNotLoaded)
			//Открываем	заново файл	проекта
			if (!OnOpenDocument(szNewFileName))
				return false;
		
		SaveDSK();
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
void CProjectDoc::SaveAll()
{
	Save();

	IDocMngrLink * pDocMngrLink	= GetDocManagerInterface();
	ASSERT_RETURN_VOID(pDocMngrLink);
	
	//Сохраняем	все	открытые документы исходников
	for	(cstr szFile = GetFileFirst();szFile; szFile = GetFileNext()){
		if (IDocumentLink * pDocLink =	pDocMngrLink->FindDocument(szFile))
			if (pDocLink->GetObject()->IsModified())
				pDocLink->GetObject()->DoFileSave();
	}
}
//------------------------------------------------------------------------
//Получение	строки заметок. Derived from IProjectDoc
string_smart CProjectDoc::GetNotes() const {
	return m_strNotes.c_str();
}
//------------------------------------------------------------------------
//Модифицирует строку заметок. Derived from IProjectDoc
void CProjectDoc::SetNotes(cstr	szNotes){
	if (m_strNotes.cmp(szNotes)) return;
	m_strNotes=szNotes;
	SetModify(true);
}
//------------------------------------------------------------------------
//Получение	строки параметров. Derived from IProjectDoc
string_smart CProjectDoc::GetParams() const {
	return m_strParams.c_str();
}
//------------------------------------------------------------------------
//Модификация строки параметров. Derived from IProjectDoc
void CProjectDoc::SetParams(cstr szParams){
	if (m_strParams.cmp(szParams)) return;
	m_strParams=szParams;
	SetModify(true);
}
//------------------------------------------------------------------------
//Возвращает ключи компиляции
string_smart CProjectDoc::GetSwitches() const {
	return m_strSwitches.c_str();
}
//------------------------------------------------------------------------
//Модифицирует ключи компиляции
void CProjectDoc::SetSwitches(cstr szSwitches){
	if (m_strSwitches==szSwitches) return;
	string_smart strNewSwitches(szSwitches,"/DUMMY");	//Пробуем так...
	if (m_strSwitches==strNewSwitches) return;

	m_strSwitches=string_smart(szSwitches,"/DUMMY").c_str();
	//DUMMY - отмечаем, что было изменение параметров,
	//и даже если строка параметров была пустая, теперь она запишется в проект

	SetModify(true);
}
//------------------------------------------------------------------------
void CProjectDoc::GetBuildSteps(OUT list_build_step & PreBuildSteps, OUT list_build_step & PostBuildSteps)
{
	PreBuildSteps = m_lstPreBuildSteps;
	PostBuildSteps= m_lstPostBuildSteps;
}
//------------------------------------------------------------------------
void CProjectDoc::SetBuildSteps(const list_build_step & PreBuildSteps, const list_build_step & PostBuildSteps)
{
	if ((m_lstPreBuildSteps==PreBuildSteps) && (m_lstPostBuildSteps==PostBuildSteps))
		return;

	m_lstPreBuildSteps = PreBuildSteps;
	m_lstPostBuildSteps = PostBuildSteps;
	SetModify(true);
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetSourceLanguage() const
{
	return m_strSourceLanguage.c_str();
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetTargetLanguage() const 
{
	return m_strTargetLanguage.c_str();
}
//------------------------------------------------------------------------
void CProjectDoc::SetSourceLanguage(cstr szName)
{
	if (m_strSourceLanguage==szName) return;
	m_strSourceLanguage = szName;
	SetModify(true);
}
//------------------------------------------------------------------------
void CProjectDoc::SetTargetLanguage(cstr szName)
{
	if (m_strTargetLanguage==szName) return;
	m_strTargetLanguage = szName;
	SetModify(true);
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetInputTranslationDir() const
{
	return m_strInputTranslationDir.c_str();
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetOutputTranslationDir() const
{
	return m_strOutputTranslationDir.c_str();
}
//------------------------------------------------------------------------
void CProjectDoc::SetInputTranslationDir(string_adapter szName)
{
	if (m_strInputTranslationDir==szName) return;
	m_strInputTranslationDir=szName;
	SetModify(true);
}
//------------------------------------------------------------------------
void CProjectDoc::SetOutputTranslationDir(string_adapter szName) 
{
	if (m_strOutputTranslationDir==szName) return;
	m_strOutputTranslationDir=szName;
	SetModify(true);
}
//------------------------------------------------------------------------
string_smart CProjectDoc::GetCompilerInfoFilename() const
{
	return m_strCompilerInfoFilename.c_str();
}
//------------------------------------------------------------------------
void CProjectDoc::SetCompilerInfoFilename(string_adapter szFilename)
{
	m_strCompilerInfoFilename=szFilename;
	SetModify(true);
}
//------------------------------------------------------------------------
void CProjectDoc::GetLibs(OUT string_list & lst) const {
	lst=m_lstLibs;
}
//------------------------------------------------------------------------
void CProjectDoc::GetLangGens(OUT string_list & lst) const {
	lst=m_lstLangGens;
}
//------------------------------------------------------------------------
void CProjectDoc::SetLangGens(const string_list & lst) {
	if (m_lstLangGens==lst) return;
	m_lstLangGens=lst;
	SetModify(true);
}
//------------------------------------------------------------------------
CProjectDoc::parse_report_type CProjectDoc::GetParseReportType() const {
	if  (m_nParsingReportType & PARSE_REPORT_FULL) {
		if (m_bParsingReportToFile)	return parse_report_type::prFullToFile;
		else return parse_report_type::prFull;
	}
	else if (m_nParsingReportType & PARSE_REPORT_SHORT){
		if (m_bParsingReportToFile)	return parse_report_type::prShortToFile;
		else return parse_report_type::prShort;
	}
	return parse_report_type::prNone;
}
//------------------------------------------------------------------------
bool CProjectDoc::GetParseStopAtMessage() const {
	return m_bParsingStopAtMessage;
}
//------------------------------------------------------------------------
//Возвращает первый	файл в списке файлов проекта. Derived from IProjectDoc
cstr  CProjectDoc::GetFileFirst() const {
	m_itFile=const_cast<CProjectDoc*> (this)->m_lstSources.begin();
	return GetFileNext();
}
//------------------------------------------------------------------------
//Возвращает следующий файл	по списку. Derived from IProjectDoc
cstr  CProjectDoc::GetFileNext() const {
	if (m_itFile==m_lstSources.end())
		return NULL;
	return (*m_itFile++).c_str();
}
//------------------------------------------------------------------------
void CProjectDoc::GetFiles(OUT string_list & lstFileList, int nFileType) const
{
	lstFileList.erase();
	for	(list_sources::const_iterator i=m_lstSources.begin();i!=m_lstSources.end();++i)
		if (GetFileType((*i).c_str())==nFileType)
			lstFileList.items().add((*i).c_str());
}
//------------------------------------------------------------------------
bool CProjectDoc::PreBuild()
{
	//Смотрим сколько у нас входных и сколько выходных направлений.
	//Если их >1, то спрашиваем пользователя
	string_list lstInputs,lstOutputs;
	singleton_cache::GetParserLink()->FindTranslationDirections(*this, lstInputs,lstOutputs);

	bool bNeedAskInput=true;
	bool bNeedAskOutput=true;

	//Если их нет совсем, сбрасываем предыдушие настройки
	if (lstInputs.items().count()==0){
		m_strInputTranslationDir.clear();
		bNeedAskInput=false;
	}
	if (lstOutputs.items().count()==0){
		m_strOutputTranslationDir.clear();
		bNeedAskOutput=false;
	}

	if (bNeedAskInput && (lstInputs.items().count()==1))
		m_strInputTranslationDir=lstInputs.items()[0];
	if (bNeedAskOutput && (lstOutputs.items().count()==1))
		m_strOutputTranslationDir=lstOutputs.items()[0];

	if ((bNeedAskInput) &&
	   (!m_strInputTranslationDir.isempty()) &&
	   (lstInputs.items().index_of(m_strInputTranslationDir,false)!=-1))
	   bNeedAskInput=false;

	if ((bNeedAskOutput) &&
	   (!m_strOutputTranslationDir.isempty()) &&
	   (lstOutputs.items().index_of(m_strOutputTranslationDir,false)!=-1))
	   bNeedAskOutput=false;

	if (bNeedAskInput || bNeedAskOutput){
		LOG_PROTECT_CODE_BEGIN
		mtdev_lib::IChooseCompilationChemaPtr spChooseCS(__uuidof(mtdev_lib::ChooseCompilationChema));
		for (size_t i=0;i<lstInputs.items().count();++i)
			spChooseCS->AddInput(lstInputs.items()[i]);
		for (size_t i=0;i<lstOutputs.items().count();++i)
			spChooseCS->AddOutput(lstOutputs.items()[i]);

		//запускаем форму
		_bstr_t aChosenInputDir=m_strInputTranslationDir.c_str();
		_bstr_t aChosenOutputDir=m_strOutputTranslationDir.c_str();
		if 	(spChooseCS->Show(aChosenInputDir.GetAddress(),aChosenOutputDir.GetAddress())==FALSE)
			return false;
		SetInputTranslationDir(aChosenInputDir);
		SetOutputTranslationDir(aChosenOutputDir);
		LOG_PROTECT_CODE_END
	}

	//Сохраняем даты компиляции для всех файлов
	for	(cstr szFile = GetFileFirst();szFile; szFile = GetFileNext()){
		filedata_map::iterator i = m_mapFileData.find(szFile);
		if (i==m_mapFileData.end())
			i = m_mapFileData.insert(make_pair(szFile,CFileSourceState())).first;

		get_file_time(szFile,NULL,NULL,&(*i).second.m_TimeBuild);	
	}
	return true;
}
//------------------------------------------------------------------------
//Запускает	процесс	компиляции самого себя. Derived from IProjectDoc
void CProjectDoc::Build()
{
	//--- начало компиляции --
	IBuilderLink * pBuilderLink		=GetBuilderInterface();
	IDocMngrLink * pDocMngrLink	= GetDocManagerInterface();
	ASSERT_RETURN_VOID(pBuilderLink	&& pDocMngrLink);

	SaveAll();
	if (PreBuild())
	  pBuilderLink->Build(this);
}
//------------------------------------------------------------------------
//Удаление всех	файлов компилятора
//Derived from IProjectDoc
void CProjectDoc::Clean()
{
	cstr pszExts[] = {".exe",".bin",".lst",".stc"};
	
	for	(int i=0; i<arrlength(pszExts);i++)
		remove(string_smart(
				GetOutputDirectoryFull(),
				GetName(),
				pszExts[i]).c_str());
				
	remove(".lst");	// Странная	ошибка моего компилятора
	CFileFind FF;
	if (FF.FindFile(string_smart(GetOutputDirectoryFull(),"*.imd").c_str()))
	{
		BOOL f;
		do
		{
			f=FF.FindNextFile();
			remove(FF.GetFileName());
		} while(f);
	}
}
//------------------------------------------------------------------------
//Derived from IProjectDoc 
IDebugManageLink* CProjectDoc::GetDebugManageLink(cstr szSource) const
{
	ISourceDocLink * pIDoc=GetSourceInterface(szSource); 
	return (pIDoc) ? pIDoc->GetDebugManageLink() : NULL;
}
//------------------------------------------------------------------------
bool CProjectDoc::IsBreakpointExists(cstr szSource, size_t dwLine, BREAKPOINT & bp) const
{
	filedata_map::const_iterator i = m_mapFileData.find(szSource);
	if (i!=m_mapFileData.end()){
		const CFileSourceState & state = (*i).second;
		for (CFileSourceState::breakpoint_vector::const_iterator j=state.m_Breakpoints.begin(); j!=state.m_Breakpoints.end();++j)
			if  ((*j).GetLine()==dwLine){
				bp=*j;
				return true;
			}
	}
	return false;
}
//------------------------------------------------------------------------
//Запускает	процесс	выполнения самого себя. Derived from IProjectDoc
void CProjectDoc::Run()
{
	IBuilderLink * pBuilderLink	= GetBuilderInterface();
	ASSERT_RETURN_VOID(pBuilderLink);

	switch (IsNeedCompile()){
		case IDYES:
			SaveAll();
			if (PreBuild())
				pBuilderLink->Build(this,IBuilderLink::functor_build(this, BuildWaitToRun));
			break;
		case IDNO:
			RunInternal();
			break;
	}
}
//------------------------------------------------------------------------
//Добавляет себя в базу знаний транслятора
void CProjectDoc::AddToKnowledgeBase()			
{
	IBuilderLink * pBuilderLink	= GetBuilderInterface();
	ASSERT_RETURN_VOID(pBuilderLink);

	m_strOldCompilerInfoFilename=m_strCompilerInfoFilename;
	if (m_strCompilerInfoFilename.isempty()){
		m_strCompilerInfoFilename=change_fileext(GetExePath(),".info");
	}

	switch (IsNeedCompile(m_strCompilerInfoFilename!=m_strOldCompilerInfoFilename)){
		case IDYES:
			SaveAll();
			if (PreBuild())
				pBuilderLink->Build(this,IBuilderLink::functor_build(this, BuildWaitToAddToKnowledgeBase));
			break;
		case IDNO:
			AddToKnowledgeBaseInternal();
			break;
	}
}
//------------------------------------------------------------------------
//Запускает	процесс	отладки. Derived from IProjectDoc
void CProjectDoc::Debug()
{
	IBuilderLink * pBuilderLink	= GetBuilderInterface();
	ASSERT_RETURN_VOID(pBuilderLink);

	switch (IsNeedCompile()){
		case IDYES:
			SaveAll();
			if (PreBuild())
				pBuilderLink->Build(this,IBuilderLink::functor_build(this, BuildWaitToDebug));
			break;
		case IDNO:
			DebugInternal();
			break;
	}
}
//------------------------------------------------------------------------
int	CProjectDoc::IsNeedCompile(bool bForce)
{
	IDocMngrLink * pDocMngrLink	= GetDocManagerInterface();
	ASSERT_RETURN(pDocMngrLink,IDCANCEL);
	
	//Перво-наперво, проверяем,	а есть ли такой	файл
	if (!is_file_exist(GetExePath())) 
		return (guiMsgConfirm(0,Format(string_res(IDS_RUN_PROJECT_NO_MODULE_s,HRESOURCES).c_str(),GetExePath().c_str()).c_str()))
			? IDYES
			: IDCANCEL;
	
	//не надо ли еще раз скомпилировать
	//Если есть	хотя бы	один измененный	исходник
	bool bIsModified=false;
	string_list sources; GetFiles(sources,IProjectDocLink::fileSource);
	for	(size_t i=0;(i<sources.items().count()) && (!bIsModified); i++)
	{
		string_smart strFileName = sources.items()[i];
		if (IDocumentLink * pDocLink =pDocMngrLink->FindDocument(strFileName.c_str()))
			bIsModified=pDocLink->GetObject()->IsModified()!=FALSE;
	}

	//если нет срели открытых, надо сравнить даты закрытых документов
	//ведь документ могли изменить, сохранить и закрыть
	if (!bIsModified)
	{
		for	(size_t i=0;(i<sources.items().count()) && (!bIsModified); i++)
		{
			cstr szFile=sources.items()[i];
			if (GetFileType(szFile)==IProjectDocLink::fileSource)
			{
				filedata_map::iterator i = m_mapFileData.find(szFile);
				if (i!=m_mapFileData.end()) {
					__time64_t LastModified=(*i).second.m_TimeBuild;
					if (get_file_time(szFile,NULL,NULL,&LastModified)){
						bIsModified = LastModified != (*i).second.m_TimeBuild;
					}
				}
			}
		}
	}

	if (bIsModified || bForce) 
		return (guiMsgYesNoCancel(0,FormatRes(IDS_RUN_PROJECT_WAS_CHANGED_s,HRESOURCES,GetPath().c_str())));
	else
		return IDNO;
}
//------------------------------------------------------------------------
void CProjectDoc::BuildWaitToRun(bool Succesful)
{
	IRunnerLink	* pRunnerLink	=GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);

	if (Succesful){
		RunInternal();
		SaveDSK();
	}
}
//------------------------------------------------------------------------
void CProjectDoc::RunInternal()
{
	ISetParamsLink * pSetParamsLink	= i_objects::GetInterface<ISetParamsLink>();

	if ((m_strParamsBrowseDir.isempty()) ||	(!is_dir_exist(m_strParamsBrowseDir)))
		m_strParamsBrowseDir=m_strWorkDir.c_str();

	if ((pSetParamsLink) &&	(m_bShowParamsDialog))
	{
		string_smart strNewParams= m_strParams.c_str();
		string_smart strNewParamsBrowseDir=	m_strParamsBrowseDir.c_str();
		
		if (pSetParamsLink->Run(strNewParams,strNewParamsBrowseDir,m_bShowParamsDialog))
			if ((strNewParams!=m_strParams.c_str())	|| (strNewParamsBrowseDir!=m_strParamsBrowseDir.c_str()))
			{
				m_strParams=strNewParams.c_str();
				m_strParamsBrowseDir=strNewParamsBrowseDir.c_str();
				SetModify(true);
			}
	}

	if (singleton_cache::GetStorageLink()->General_SaveAllBeforeRun)
		SaveAll();

	IRunnerLink	* pRunnerLink	= GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);
	pRunnerLink->Run(this);
}
//------------------------------------------------------------------------
void CProjectDoc::BuildWaitToDebug(bool	Succesful)
{
	IRunnerLink	* pRunnerLink	=GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);

	if (Succesful)
		DebugInternal();
}
//------------------------------------------------------------------------
void CProjectDoc::DebugInternal()
{
	ISetParamsLink * pSetParamsLink	= i_objects::GetInterface<ISetParamsLink>();

	if ((m_strParamsBrowseDir.isempty()) ||	(!is_dir_exist(m_strParamsBrowseDir)))
		m_strParamsBrowseDir=m_strWorkDir.c_str();

	if ((pSetParamsLink) &&	(m_bShowParamsDialog))
	{
		string_smart strNewParams= m_strParams.c_str();
		string_smart strNewParamsBrowseDir=	m_strParamsBrowseDir.c_str();
		
		if (pSetParamsLink->Run(strNewParams,strNewParamsBrowseDir,m_bShowParamsDialog)){
			if ((strNewParams!=m_strParams.c_str())	|| (strNewParamsBrowseDir!=m_strParamsBrowseDir.c_str()))
			{
				m_strParams=strNewParams.c_str();
				m_strParamsBrowseDir=strNewParamsBrowseDir.c_str();
				SetModify(true);
			}
		}
		//Пользователь раздумал работать, останавливаем процесс
		else{
			return;
		}
	}

	IRunnerLink	* pRunnerLink	= GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);
	
	//Принудительное сохранение всех и вся
	if (!pRunnerLink->IsBusy()){
		if (singleton_cache::GetStorageLink()->General_SaveAllBeforeRun)
			SaveAll();
	}

	//Запускаем отладку но с остановкой после инициализации
	pRunnerLink->Debug(this,true);
	if (pRunnerLink->IsBusy()){
		_ASSERTE(pRunnerLink->IsPaused());

		//Проверяем все точки останова
		for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
			CheckBreakpointsUsed((*i).c_str());

		//Продолжаем выполнение
		pRunnerLink->Resume();
	}
}
//------------------------------------------------------------------------
void CProjectDoc::BuildWaitToAddToKnowledgeBase(bool Succesful)
{
	if (Succesful){
		AddToKnowledgeBaseInternal();
	}
}
//------------------------------------------------------------------------
void CProjectDoc::AddToKnowledgeBaseInternal()
{
	IKnowledgeBaseLink * pKBKink = i_objects::GetInterface<IKnowledgeBaseLink>();
	ASSERT_RETURN_VOID(pKBKink);
	pKBKink->Add(this);
	m_strCompilerInfoFilename = m_strOldCompilerInfoFilename;
}
//------------------------------------------------------------------------
//Проверяет у указанного документа исходных текстов допустимость всех точек останова
void CProjectDoc::CheckBreakpointsUsed(cstr szFilePath)
{
	_ASSERTE(!(m_nOpState & opChangingBreakpoints));

	IRunnerLink	* pRunnerLink	= GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);

	IRunnerLink::EActivityType AType;
	IProjectDocLink * pProcessedProject;
	//Нужно, чтобы шла отладка и отлаживался этот проект
	if (!((pRunnerLink->IsBusy(AType,pProcessedProject)) && 
		(AType==IRunnerLink::atDebug)&& (pProcessedProject==this)))
		return;

	//надо просмотреть все точки останова для указанного модуля
	if (!StoreSourceFileState(szFilePath)) return; //если нет открытого документа

	//чтобы избежать рекурсивных вызовов
	CProjectAutoState AutoState	(*this,opChangingBreakpoints); 
	CFileSourceState  & state = m_mapFileData[szFilePath];
	for (CFileSourceState::breakpoint_vector::iterator i=state.m_Breakpoints.begin();
		 i!=state.m_Breakpoints.end();++i)
		(*i).Enable(pRunnerLink->IsBreakpointUsed(szFilePath,(*i).GetLine()));
}
//------------------------------------------------------------------------
//Дает информацию о точке останова для указанного модуля в указанной строке
bool CProjectDoc::GetBreakPoint(cstr szSource, long dwLine, BREAKPOINT & Breakpoint)
{
	//Пытаемся найти открытый 
	ISourceDocLink * pDocIface = GetSourceInterface(szSource);
	if (pDocIface){
		IDebugManageLink * pDebugLink =  pDocIface->GetDebugManageLink();
		_ASSERTE(pDebugLink);
		if (pDebugLink)
			return pDebugLink->GetBreakPoint(dwLine,Breakpoint);
	}
	//Открытого документа нет
	else  if (m_mapFileData.find(szSource)!=m_mapFileData.end())
	{		
		Breakpoint.m_strModuleName=szSource;	//На всякий случай
		CFileSourceState  & state = m_mapFileData[szSource];
		//Ищем, нет ли точки останова по указанной строке
		for (dml_vb_i j=state.m_Breakpoints.begin(); j!=state.m_Breakpoints.end();++j)
			if ((*j).GetLine()==dwLine){
				Breakpoint=(*j);
				return true;
			}
	}
	else
		_ASSERTE(false);
	return false;
}
//------------------------------------------------------------------------
//Дает список всех точек останова, присутствующих в проекте
void CProjectDoc::GetBreakPoints(IDebugManageLink::vector_breakpoints & Breakpoints)
{
	Breakpoints.clear();
	//Все документы, входящие в проект
	for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
	{
		//надо просмотреть все точки останова для указанного модуля
		StoreSourceFileState((*i).c_str());
		if (m_mapFileData.find((*i).c_str())!=m_mapFileData.end()){
			CFileSourceState  & state = m_mapFileData[(*i).c_str()];
			for (dml_vb_i j=state.m_Breakpoints.begin(); j!=state.m_Breakpoints.end();++j){
				_ASSERTE((*i).cmpi((*j).GetModuleName()));
				Breakpoints.push_back((*j));
			}
		}
	}
}
//------------------------------------------------------------------------
//Установка точки останова для указанного модуля (значение имени модуля внутри структуры BREAKPOINT игнорируется)
void CProjectDoc::SetBreakPoint(cstr szSource, BREAKPOINT & Breakpoint)
{
	//Пытаемся найти открытый 
	ISourceDocLink * pDocIface = GetSourceInterface(szSource);
	if (pDocIface){
		IDebugManageLink * pDebugLink =  pDocIface->GetDebugManageLink();
		_ASSERTE(pDebugLink);
		if (pDebugLink)
			pDebugLink->SetBreakPoint(Breakpoint);
	}
	//Открытого документа нет
	else 
	{
		if (m_mapFileData.find(szSource)==m_mapFileData.end())
			m_mapFileData[szSource]=CFileSourceState();

		Breakpoint.m_strModuleName=szSource;	//На всякий случай
		CFileSourceState  & state = m_mapFileData[szSource];
		//Ищем, нет ли точки останова по указанной строке, если есть выкидываем
		for (dml_vb_i j=state.m_Breakpoints.begin(); j!=state.m_Breakpoints.end();++j)
			if ((*j).GetLine()==Breakpoint.GetLine()){
				state.m_Breakpoints.erase(j);
				break;
			}
		state.m_Breakpoints.push_back(Breakpoint);
	}
}
//------------------------------------------------------------------------
//Снятие точки останова для указанного модуля в указанной строке
void CProjectDoc::ResetBreakPoint(cstr szSource, long dwLine)
{
	//Пытаемся найти открытый документ
	ISourceDocLink * pDocIface = GetSourceInterface(szSource);
	if (pDocIface){
		IDebugManageLink * pDebugLink =  pDocIface->GetDebugManageLink();
		_ASSERTE(pDebugLink);
		if (pDebugLink)
			pDebugLink->ResetBreakPoint(dwLine);
	}
	//Открытого документа нет, убираем точку останова у себя в записях
	else if (m_mapFileData.find(szSource)!=m_mapFileData.end()){
		CFileSourceState  & state = m_mapFileData[szSource];
		for (dml_vb_i j=state.m_Breakpoints.begin(); j!=state.m_Breakpoints.end();++j){
			if ((*j).GetLine()==dwLine){
				state.m_Breakpoints.erase(j);
				break;
			}
		}
	}
	else 
		_ASSERTE(false);
}

/////////////////////////////////////////////////////////////////////////////
// CProjectDoc::CProjectProps
/////////////////////////////////////////////////////////////////////////////
void CProjectDoc::CProjectProps::ReadFromArchive(CArchive &	ar)
{
	size_t	nLen	= (size_t)ar.GetFile()->GetLength();
	char * szBuff	= (char	*) malloc(nLen+1);
	nLen=ar.Read(szBuff,nLen);szBuff[nLen]='\0';

	char * szSectStart		=szBuff;
	char * szSectEnd		=NULL;

	string_parse Parser;
//	Parser.AddSkipRanges("\"","\"");
//	Parser.AddSkipRanges("//","\n");

	while ((szSectStart) &&	(*szSectStart))	
	{
		szSectStart	= const_cast<char*>(strltrim(szSectStart));
		szSectEnd		= const_cast<char*>(strpbrk	(szSectStart,"\n{"));
		if (!szSectEnd)	break; //получаем ключ без значения

		bool bBlockMode	= *szSectEnd=='{'; //Блочный вариант
		*szSectEnd				= '\0';

		char * strKey=strrtrim(szSectStart); //Ключ	

		szSectStart=const_cast<char*>(strltrim(szSectEnd+1));
		if (((!bBlockMode) && (*szSectStart!='{')) ||	//После	ключа сразу	должно идти	значение
			(!*strKey))																//Пустая строка
			continue;

		if (Parser.findpair(szSectStart,"{","}",szSectStart,szSectEnd)){ //Иначе не	найдено	значение для ключа
			*szSectEnd='\0';
			insert(strKey,strtrim(szSectStart+1));
			szSectStart=szSectEnd+1;
		}
		else
			szSectStart=NULL;
	} 
	ASSERT(Parser.isempty()); 
	free(szBuff);
}


/////////////////////////////////////////////////////////////////////////////
// CProjectDoc
IMPLEMENT_DYNCREATE(CProjectDoc, CDocumentEx)

//Имя объекта в	системе	ProjectDocumentXX...
#pragma warning( disable : 4355) //using "this"in member intialization list
CProjectDoc::CProjectDoc(): m_nOpState(opNone)
	#ifndef _NO_COM
	, m_pCoClass(NULL)
	#endif
{
	m_bAutoDelete=false; //	Отсутствие представлений не	означает закрытия документа
	m_bShowParamsDialog=true; //По умолчанию показываем
	
	//Подгружаем стандартные библиотеки
	string_smart strFileName;
	if (find_full_filepath(module_names::szCrtLibName,string_list(),strFileName)){
		m_lstLibs.items().add(strFileName);
	}

	g_nID++;
	LoadSettings();
}
//------------------------------------------------------------------------
CProjectDoc::~CProjectDoc()
{
	SaveSettings();
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CProjectDoc, CDocumentEx)
	ON_COMMAND(ID_FILE_PROJECT_CLOSE, cmdOnClose)
	ON_COMMAND(ID_FILE_PROJECT_SAVE, cmdOnSave)
	ON_COMMAND(ID_DEBUG_START_DEBUG, cmdOnDebugStart)
	ON_COMMAND(ID_DEBUG_START_RUN, cmdOnRunStart)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_START_RUN, cmdupOnRunStart)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_START_DEBUG, cmdupOnDebugStart)
	ON_COMMAND(ID_DEBUG_STOP, cmdOnDebugStop)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STOP,	cmdupOnDebugStop)
	ON_COMMAND(ID_BUILD_STOP_BUILD,	cmdOnBuildStop)
	ON_UPDATE_COMMAND_UI(ID_BUILD_STOP_BUILD, cmdupOnBuildStopBuild)
	ON_COMMAND(ID_BUILD_CLEAN, cmdOnClean)
	ON_COMMAND(ID_DEBUG_STEPINTO, cmdOnDebugStepInto)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPINTO, cmdupOnDebugStepInto)
	ON_COMMAND(ID_DEBUG_STEPOVER, cmdOnDebugStepOver)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPOVER, cmdupOnDebugStepOver)
	ON_COMMAND(ID_PARSING_COMPLETE_REPORT, cmdOnParsingCompleteReport)
	ON_UPDATE_COMMAND_UI(ID_PARSING_COMPLETE_REPORT, cmdupOnParsingCompleteReport)
	ON_COMMAND(ID_PARSING_REPORT_TO_FILE, cmdOnParsingReportToFile)
	ON_UPDATE_COMMAND_UI(ID_PARSING_REPORT_TO_FILE, cmdupOnParsingReportToFile)
	ON_COMMAND(ID_PARSING_SHORT_REPORT, cmdOnParsingShortReport)
	ON_UPDATE_COMMAND_UI(ID_PARSING_SHORT_REPORT, cmdupOnParsingShortReport)
	ON_COMMAND(ID_DEBUG_PAUSE_ON_PARSE, cmdOnDebugPauseOnParse)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_PAUSE_ON_PARSE, cmdupOnDebugPauseOnParse)
	
	//Команды, которые изменяют текст  меню
	ON_COMMAND(ID_BUILD_BUILD_PROJECT, cmdOnBuild)
	ON_UPDATE_COMMAND_UI(ID_BUILD_BUILD_PROJECT, cmdupOnBuild)
	ON_COMMAND(ID_PROJECT_SETTINGS,	cmdOnSettings)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SETTINGS, cmdupOnSettings)
	ON_COMMAND(ID_PROJECT_UNLOAD, cmdOnUnloadProject)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_UNLOAD,	cmdupOnUnloadProject)	

	ON_COMMAND(ID_PROJECT_ADD_RULE,	cmdOnAddRule)
	ON_COMMAND(ID_PROJECT_ADD_FILES, cmdOnAddFiles)
	ON_COMMAND(ID_PROJECT_ADD_CODE_TEMPLATE, cmdOnAddCodeTemplate)
	ON_COMMAND(ID_TOOLS_STATISTIC, cmdOnStatistic)	
	//ON_UPDATE_COMMAND_UI(ID_TOOLS_STATISTIC, cmdupOnStatistic)	
	ON_COMMAND(ID_TOOLS_SYNTAX_DIAGRAMS, cmdOnSyntaxDiagrams)
	ON_COMMAND(ID_PROJECT_ADD_TO_KNOWLEDGEBASE, cmdOnAddToKnowledgeBase)
	ON_COMMAND(ID_PROJECT_BUILD_SYNTAX_DIAGRAM, cmdOnProjectBuildSyntaxDiagram)
	ON_COMMAND(ID_PROJECT_BUILD_SYNTAX_TREE, cmdOnProjectBuildSyntaxTree)
	ON_COMMAND(ID_PROJECT_BUILD_SYNTAX_DIAGRAM, cmdOnProjectBuildSyntaxDiagram)
	ON_COMMAND(ID_PROJECT_BUILD_SYNTAX_TREE, cmdOnProjectBuildSyntaxTree)
	ON_COMMAND(ID_PROJECT_EXPORT_TO_XBNF, cmdOnExportToXBNF)
	ON_COMMAND(ID_PROJECT_NEW_FILE, cmdOnProjectNewSource)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectDoc diagnostics
#ifdef _DEBUG
void CProjectDoc::AssertValid()	const
{
	__super::AssertValid();
}
//------------------------------------------------------------------------
void CProjectDoc::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif //_DEBUG
//------------------------------------------------------------------------
IDocMngrLink* CProjectDoc::GetDocManagerInterface() const
{
	IDocMngrLink* pDocMngrLink=i_objects::GetInterface<IDocMngrLink>();
	ASSERT(pDocMngrLink);
	return pDocMngrLink;
}
//------------------------------------------------------------------------
IBuilderLink* CProjectDoc::GetBuilderInterface() const
{
	IBuilderLink * pBuildLink =	i_objects::GetInterface<IBuilderLink>();
	ASSERT(pBuildLink );
	return pBuildLink ;
}
//------------------------------------------------------------------------
IRunnerLink* CProjectDoc::GetRunnerInterface() const
{
	IRunnerLink	* pRunLink = i_objects::GetInterface<IRunnerLink>();
	ASSERT(pRunLink	);
	return pRunLink	;
}
//------------------------------------------------------------------------
ISourceDocLink*	CProjectDoc::GetSourceInterface(cstr szFilePath) const 
{
	//Небольшая оптимизация поиска
	if (m_LastAccessedDoc.IsAppropriate(szFilePath)){
		_ASSERTE(m_LastAccessedDoc.m_pSourceLink->GetObject());
		_ASSERTE(_CrtCheckMemory());				
		return m_LastAccessedDoc.m_pSourceLink;
	}
	
	IDocMngrLink * pDocMngrLink;
	//Мы обрабатываем только события своих документов
	if (IsFilePresent(szFilePath) && (pDocMngrLink=	GetDocManagerInterface())){
		ISourceDocLink* pDocLink = dynamic_cast<ISourceDocLink*> (pDocMngrLink->FindDocument(szFilePath));
		if (pDocLink)
			m_LastAccessedDoc.Assign(pDocLink);
		return pDocLink;
	}
	else
		return NULL;
}
//------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
void CProjectDoc::evOnOpenDocument(evdOpenDocumentInfo &Info)
{
	ASSERT(Info.in_strDocumentPath.c_str());
	RestoreSourceFileState(Info.in_strDocumentPath.c_str());	

	//Отлавливаем изменение точек останова
	_ASSERTE(!(m_nOpState & opChangingBreakpoints));

	RestoreSourceFileState(Info.in_strDocumentPath.c_str());
	
	//Проверяем точки останова
	for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
		CheckBreakpointsUsed((*i).c_str());
}
//------------------------------------------------------------------------
void CProjectDoc::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	ASSERT(Info.in_strDocumentPath.c_str());

	StoreSourceFileState(Info.in_strDocumentPath.c_str());	
	
	m_LastAccessedDoc.Clear();	//на всякий случай очищаем при любых обстоятельствах
}
//------------------------------------------------------------------------
void CProjectDoc::evOnChangeDocument(evdChangeDocumentInfo	&Info)
{
	if ((Info.in_nChangeType==evdChangeSourceDocInfo::changeBreakpoints) && 
		!(m_nOpState & opChangingBreakpoints))

		//ПРоверяем установленную точку
		CheckBreakpointsUsed(Info.in_strDocumentPath.c_str());
}
//------------------------------------------------------------------------
void CProjectDoc::RaiseChangeEvent(evdChangeProjectInfo	&Info)
{
	//В	процессе загрузки/создания не считается. Еще не	было события открытия
	if (!(m_nOpState & opCreating) && !(m_nOpState & opOpening))
		i_objects::event<ARGS_1(evdChangeDocumentInfo&)>(IDocumentLink::IEventSink::evOnChangeDocument,Info);
}
//------------------------------------------------------------------------
void CProjectDoc::RaiseOpenEvent(evdOpenProjectInfo	& Info)
{
	i_objects::event<ARGS_1(evdOpenDocumentInfo &)>(IDocumentLink::IEventSink::evOnOpenDocument,Info);
}
//------------------------------------------------------------------------
void CProjectDoc::RaiseCloseEvent(evdCloseProjectInfo &	Info)
{
	i_objects::event<ARGS_1(evdCloseDocumentInfo &)>(IDocumentLink::IEventSink::evOnCloseDocument,Info);
}
//------------------------------------------------------------------------
CProjectDoc::cit_file CProjectDoc::FileFind(cstr szFileName) const 
{
	return find(m_lstSources.begin(),m_lstSources.end(),szFileName);
}
//------------------------------------------------------------------------
bool CProjectDoc::FileAdd(cstr szFileName)
{		
	if (!szFileName)		//Предложить выбор пользователю
	{

		IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();
		ASSERT_RETURN_FALSE(pDocMngrLink);

		string_list	lstFiles;
		if (!pDocMngrLink->SelectFile(lstFiles,NULL,NULL,NULL,EXT_SOURCES,true,true,true,false,true))
			return false;

		for	(size_t	i=0;i<lstFiles.items().count();i++){
			//Если файла нет - его нужно создать, так как в диалоге
			//мы указали опцию "спрашивать создание"
			cstr szFileName = lstFiles.items()[i];
			if (!is_file_exist(szFileName))
				save_file(szFileName,"");
			FileAdd(szFileName);	//Рекурсивный вызов
		}
		return true;
	}
	else
	{
		ASSERT(*szFileName);	//Проверка на допустимость
		if (!*szFileName)				//
			return false;			//

		if (IsFilePresent(szFileName))
			return false;
		m_lstSources.push_back(szFileName);

		SetModifiedFlag(true);	//Здесь	формируется	первое событие изменения состояние
		//... а	вот	и второе
		evdChangeProjectInfo Info(GetName().c_str(),GetPath().c_str(),*this);
		Info.in_nChangeType=evdChangeProjectInfo::changeAddFile;
		Info.in_strIncludedFile=szFileName;
		RaiseChangeEvent(Info);
		return true;
	}
}
//------------------------------------------------------------------------
bool CProjectDoc::FileRemove(cstr szFileName)
{
	if (!IsFilePresent(szFileName))
		return false;
	m_lstSources.remove(szFileName);
	m_LastAccessedDoc.Clear();
		
	SetModifiedFlag(true);//Здесь формируется первое событие изменения состояние

	//... а	вот	и второе, с	точным указанием причины
	evdChangeProjectInfo Info(GetName().c_str(),GetPath().c_str(),*this);
	Info.in_nChangeType=evdChangeProjectInfo::changeRemoveFile;
	Info.in_strIncludedFile=szFileName;
	RaiseChangeEvent(Info);

	return true;
}
//------------------------------------------------------------------------
bool CProjectDoc::FileSaveModified(cstr	szFileName)
{
	//Получаем управляющего
	CDocManager	* pDocMngr = AfxGetApp()->m_pDocManager;
	ASSERT_RETURN_FALSE(pDocMngr);

	//Теперь перебираем	все	шаблоны, а в них - документы, пытаясь
	//найти	открытый документ с	заданным именем	
	for	(POSITION posTmpl =	pDocMngr->GetFirstDocTemplatePosition(); posTmpl;)
	{
		CDocTemplate* pDocTempl	= pDocMngr->GetNextDocTemplate(posTmpl);
		for	(POSITION posDoc = pDocTempl->GetFirstDocPosition(); posDoc;)		
		{
			CDocument *	pDoc=pDocTempl->GetNextDoc(posDoc);
			if (lstrcmpi(pDoc->GetPathName(),szFileName)==0)//Нашли	!
				return (pDoc->SaveModified()!=FALSE);
		}
	}
	return true; //...и	не был открыт
}
//------------------------------------------------------------------------
bool CProjectDoc::FileClose(cstr szFileName)
{
	if (!IsFilePresent(szFileName))
		return false;

	//Получаем управляющего
	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_FALSE(pDocMngrLink);
	IDocumentLink * pDoc=pDocMngrLink->FindDocument(szFileName);
	if (pDoc)
		return pDoc->Close();

	return true; //или закрыт, или и не был открыт
}
//------------------------------------------------------------------------
void CProjectDoc::LoadSettings(){
	if (g_nID==1){
		m_nParsingReportType=AfxGetApp()->GetProfileInt(szProfileWorkSpace,szParamParsingReportType,m_nParsingReportType);
		m_bParsingReportToFile=AfxGetApp()->GetProfileInt(szProfileWorkSpace,szParamParsingReportToFile,m_bParsingReportToFile)!=0;
		m_bParsingStopAtMessage=AfxGetApp()->GetProfileInt(szProfileWorkSpace,szParamParsingStopAtMessage,m_bParsingStopAtMessage)!=0;
	}
}
//------------------------------------------------------------------------
void CProjectDoc::SaveSettings(){
	AfxGetApp()->WriteProfileInt(szProfileWorkSpace,szParamParsingReportType,m_nParsingReportType);
	AfxGetApp()->WriteProfileInt(szProfileWorkSpace,szParamParsingReportToFile,m_bParsingReportToFile);
	AfxGetApp()->WriteProfileInt(szProfileWorkSpace,szParamParsingStopAtMessage,m_bParsingStopAtMessage);
}
//------------------------------------------------------------------------
static void	WriteStringLn(CArchive& ar, string_adapter szString1, 
										string_adapter szString2=STR_NULL,
										string_adapter szString3=STR_NULL,
										string_adapter szString4=STR_NULL)
{
	ar.WriteString(szString1); 
	if (szString2)
		ar.WriteString(szString2); 
	if (szString3)
		ar.WriteString(szString3);
	if (szString4)
		ar.WriteString(szString4);

	ar.WriteString("\n");
}
//------------------------------------------------------------------------
void CProjectDoc::LoadDSK()
{
	IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngrLink);
	CDocManager	* pDocMngr = pDocMngrLink->GetObject();
	ASSERT_RETURN_VOID(pDocMngr);

	LOG_PROTECT_CODE_BEGIN
	//Вычитываем DSK-файл
	property_set_section DSK;
	if (!DSK.load_from_file(change_fileext(m_strPath,EXT_DSK).c_str()))
		return;

	m_mapFileData.clear();

	//Получение	списка открытых	файлов
	string_list	strDsk		= DSK.get(TAG_PROJECT_DSK).c_str();

	string_list	strPoses	= DSK.get(TAG_PROJECT_POSNS).c_str();
	string_list	strBkmks	= DSK.get(TAG_PROJECT_BKMKS).c_str();
	string_list	strBps		= DSK.get(TAG_PROJECT_BPS).c_str();
	string_list strLastModf = DSK.get(TAG_PROJECT_LAST_MODIFIED).c_str();
 
	for	(size_t	i=0; i<strPoses.items().count();i++){
		string_split szS(strPoses.items()[i],true, "::");
		CFileSourceState & state=m_mapFileData[szS.get_left_()];
		sscanf(szS.get_right_().c_str(),"%d%d%d",&state.m_Pos.dwPos,&state.m_Pos.dwFVL,&state.m_Pos.dwFCV);
	}

	//Закладки
	for	(size_t	i=0; i<strBkmks.items().count();i++){
		string_split szS(strBkmks.items()[i],true, "::");
		CFileSourceState & state=m_mapFileData[szS.get_left_()];
		state.m_Bookmarks.push_back(szS.get_right_as_int());
	}

	//Точки	останова
	for	(size_t	i=0; i<strBps.items().count();i++){
		string_split szS(strBps.items()[i],true, "::");
		CFileSourceState & state=m_mapFileData[szS.get_left_()];
		BREAKPOINT bp(NULL,szS.get_left_()); 
		bp.strCondition.reserve(255);
		sscanf(szS.get_right_().c_str(),"%d%d%d%s",&bp.dwLine,&bp.bEnabled,&bp.dwHitCount, bp.strCondition.c_str());		
		state.m_Breakpoints.push_back(bp);
	}
	//Время посленей модификации
	for	(size_t	i=0; i<strLastModf.items().count();i++){
		string_split szS(strLastModf.items()[i],true, "::");
		CFileSourceState & state=m_mapFileData[szS.get_left_()];
		sscanf(szS.get_right_().c_str(),"%I64u",&state.m_TimeBuild);		
	}

	//Открываем	указанные файлы
	for	(size_t	i=0; i<strDsk.items().count();i++){
		string_smart s(strDsk.items().get_at(i).trim());
		if (is_file_exist(s))
			pDocMngr->OpenDocumentFile(s.c_str());
	}
  
	string_smart strActive = DSK.get(TAG_PROJECT_ACTIVE_DOC).trim();
	if (is_file_exist(strActive))
		pDocMngr->OpenDocumentFile(strActive.c_str());

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CProjectDoc::SaveDSK()
{
	IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_VOID(pDocMngrLink);
	
	const cstr szOB	= property_set_section::m_cszOpenBracket;
	const cstr szCB	= property_set_section::m_cszCloseBracket;
	
	LOG_PROTECT_CODE_BEGIN
	//Сохраняем	состояние открытых документов. Пути	сохраняются	абсолютные
	string_smart strDsk(TAG_PROJECT_DSK,szOB);
	for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
		if (pDocMngrLink->FindDocument((*i).c_str())) //есть среди открытых
			strDsk.append("\n",(*i).c_str(),NULL);
	strDsk.append("\n",szCB,NULL);			

	//Сохраняем активный документ 
	ISourceDocLink * pActiveDocLink = pDocMngrLink->GetActiveSourceDoc();
	if (pActiveDocLink) {
		strDsk.append("\n",TAG_PROJECT_ACTIVE_DOC,szOB,NULL); 
		strDsk.append("\n",pActiveDocLink->GetPath().c_str(),NULL);
		strDsk.append("\n",szCB,NULL);
	}
 
	//Получаем разнообразные состояния исходных	файлов
	for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
		StoreSourceFileState((*i).c_str());

	//Сохраняем	позиции	курсоров в файлах
	strDsk.append("\n",TAG_PROJECT_POSNS,szOB,NULL);
	for	(filedata_map::iterator	i =	m_mapFileData.begin(); i!=m_mapFileData.end();++i){
		CFileSourceState & state=(*i).second;
		strDsk.append("\n",(*i).first.c_str(),Format(" :: %d %d	%d",state.m_Pos.dwPos,state.m_Pos.dwFVL,state.m_Pos.dwFCV).c_str(),NULL);
	}
	strDsk.append("\n",szCB,NULL);			

	//Сохраняем	закладки в файлах
	strDsk.append("\n",TAG_PROJECT_BKMKS,szOB,NULL);
	for	(filedata_map::iterator	i =	m_mapFileData.begin(); i!=m_mapFileData.end();++i)
	{
		CFileSourceState & state=(*i).second;
		if (state.m_Bookmarks.size())
			for	(CFileSourceState::bookmark_vector::iterator j=	state.m_Bookmarks.begin();j!=state.m_Bookmarks.end();++j)
				strDsk.append("\n",(*i).first.c_str(),"	:: ", Format("%d",*j).c_str(),NULL);
	}
	strDsk.append("\n",szCB,NULL);			

	//Сохраняем	точки останова в файлах
	//фОРМАТ СОХРАНЕНИЯ: line(%d) enabled(%d) pass_count(%d) condition(%s)
	strDsk.append("\n",TAG_PROJECT_BPS,	szOB,NULL);
	for	(filedata_map::iterator	i =	m_mapFileData.begin(); i!=m_mapFileData.end();++i)
	{
		CFileSourceState & state=(*i).second;
		if (state.m_Breakpoints.size())
			for	(CFileSourceState::breakpoint_vector::iterator j= state.m_Breakpoints.begin();j!=state.m_Breakpoints.end();++j)
				strDsk.append("\n",(*i).first.c_str()," :: ",
				  Format("%d %d %d %s",(*j).dwLine,	(long)(*j).bEnabled,(*j).dwHitCount,(*j).strCondition.c_str()).c_str(),NULL);
							
	}
	strDsk.append("\n",szCB,NULL);			

	//Сохраняем	время последней компиляции
	strDsk.append("\n",TAG_PROJECT_LAST_MODIFIED, szOB,NULL);
	for	(filedata_map::iterator	i =	m_mapFileData.begin(); i!=m_mapFileData.end();++i)
	{
		CFileSourceState & state=(*i).second;
		if (state.m_TimeBuild!=0){
			string_smart strFileData = Format("%s :: %I64u",(*i).first.c_str(), state.m_TimeBuild);
			strDsk.append("\n",strFileData.c_str(),NULL);
		}
	}
	strDsk.append("\n",szCB,NULL);			

	save_file(change_fileext(m_strPath,EXT_DSK),strDsk);
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
bool CProjectDoc::RestoreSourceFileState(cstr szFilePath)
{
	CProjectAutoState AutoState	(*this,opChangingBreakpoints);
	//Мы обрабатываем только события своих документов
	//Получаем указатель на	документ
	ISourceDocLink * pIDoc=GetSourceInterface(szFilePath); 
	if (pIDoc){
		ISourceViewLink	*pIView=pIDoc->GetViewLink();		
		ASSERT_RETURN_FALSE(pIView);		
		//Итак,	если это документ исходника, сохраняем данные
		filedata_map::iterator i = m_mapFileData.find(szFilePath);
		if (i!=m_mapFileData.end()){
			CFileSourceState & state = (*i).second;
			pIView->SetViewPos(state.m_Pos.dwPos,state.m_Pos.dwFVL,state.m_Pos.dwFCV);
			for	(CFileSourceState::bookmark_vector::iterator j=	state.m_Bookmarks.begin();j!=state.m_Bookmarks.end();++j)
				pIView->SetBookmark(*j);
			for	(CFileSourceState::breakpoint_vector::iterator j= state.m_Breakpoints.begin();j!=state.m_Breakpoints.end();++j)
				pIView->SetBreakPoint(*j);
		}
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
bool CProjectDoc::StoreSourceFileState(cstr	szFilePath)
{
	//Мы обрабатываем только события своих документов
	//Получаем указатель на	документ
	ISourceDocLink * pIDoc=GetSourceInterface(szFilePath); 
	if (pIDoc){
		ISourceViewLink	*pIView=pIDoc->GetViewLink();		
		ASSERT_RETURN_FALSE(pIView);		

		CFileSourceState  state;
		filedata_map::iterator i = m_mapFileData.find(szFilePath);

		pIView->GetViewPos(state.m_Pos.dwPos,state.m_Pos.dwFVL,state.m_Pos.dwFCV);
		pIView->GetBookmarks(state.m_Bookmarks);		
		pIView->GetBreakPoints(state.m_Breakpoints);
		if (i!=m_mapFileData.end())
			state.m_TimeBuild = (*i).second.m_TimeBuild;

		m_mapFileData[szFilePath]=state;
		return true;
	}	
	return false;
}
//------------------------------------------------------------------------
// CProjectDoc serialization
void CProjectDoc::Serialize(CArchive& ar)
{
	LOG_PROTECT_CODE_BEGIN
	CProjectProps Props;
	if (ar.IsStoring())
	{
		WriteStringLn(ar,TAG_PROJECT_SIGN);
		WriteStringLn(ar,"# Created by ",ApplicationData::APP_COMMENT,string_smart("; Version ",ApplicationData::APP_VERSION));
		WriteStringLn(ar,"# ",ApplicationData::APP_COPYRIGHT);
		WriteStringLn(ar,"# ","** Do not edit this file manually! **");
		WriteStringLn(ar,"");

		//Сохраняем	секцию исходных	файлов
		WriteStringLn(ar,TAG_PROJECT_SOURCES,"\n{");
		for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
			WriteStringLn(ar,"  ",get_relative_path((*i),m_strWorkDir));
		WriteStringLn(ar,"}");

		//Сохраняем параметры командной	строки
		if (!m_strParams.isempty()){
			WriteStringLn(ar,TAG_PROJECT_PARAMS,"\n{");
			WriteStringLn(ar,"  ",m_strParams,"\n}");
		}

		//Сохраняем каталог параметров
		if (!m_strParamsBrowseDir.isempty()){
			WriteStringLn(ar,TAG_PROJECT_PARAMS_BROWSE_DIR,"\n{");
			WriteStringLn(ar,"  ",m_strParamsBrowseDir,"\n}");
		}

		//Сохраняем заметки
		if (!m_strNotes.isempty()){
			WriteStringLn(ar,TAG_PROJECT_NOTES,"\n{");
			WriteStringLn(ar,m_strNotes,"\n}");
		}

		//Сохраняем ключи 
		if (!m_strSwitches.isempty()){
			WriteStringLn(ar,TAG_PROJECT_SWITCHES,"\n{");
			WriteStringLn(ar,"  ",m_strSwitches,"\n}");
		}

		//Сохраняем Output Directory
		if (!m_strOutputDirectory.isempty()){
			WriteStringLn(ar,TAG_PROJECT_OUTPUT_DIR,"\n{");
			WriteStringLn(ar,"  ",m_strOutputDirectory,"\n}");
		}
		if (!m_strIntermediateDirectory.isempty()){
			WriteStringLn(ar,TAG_PROJECT_INTERMEDIATE_DIR,"\n{");
			WriteStringLn(ar,"  ",m_strIntermediateDirectory,"\n}");
		}

		//Сохраняем имя транслируемого языка
		if (!m_strSourceLanguage.isempty()){
			WriteStringLn(ar,TAG_PROJECT_SRC_LANG,"\n{");
			WriteStringLn(ar,"  ",m_strSourceLanguage,"\n}");
		}

		//Сохраняем имя выходного языка
		if (!m_strTargetLanguage.isempty()){
			WriteStringLn(ar,TAG_PROJECT_DST_LANG,"\n{");
			WriteStringLn(ar,"  ",m_strTargetLanguage,"\n}");
		}

		//Сохраняем имя InputTranslationDir
		if (!m_strInputTranslationDir.isempty()){
			WriteStringLn(ar,TAG_PROJECT_L_IN_DIR,"\n{");
			WriteStringLn(ar,"  ",m_strInputTranslationDir,"\n}");
		}

		//Сохраняем имя OutputTranslationDir
		if (!m_strOutputTranslationDir.isempty()){
			WriteStringLn(ar,TAG_PROJECT_L_OUT_DIR,"\n{");
			WriteStringLn(ar,"  ",m_strOutputTranslationDir,"\n}");
		}

		//Сохраняем список генераторов кода на ЯП
		if (!m_lstLangGens.isempty()){
			WriteStringLn(ar,TAG_PROJECT_LANG_GEN,"\n{");
			for	(size_t i=0;i<m_lstLangGens.items().count();++i)
				WriteStringLn(ar,"  ",m_lstLangGens.items()[i]);
			WriteStringLn(ar,"}");
		}
		//Сохраняем список PreBuildSteps
		if (m_lstPreBuildSteps.size()!=0){
			WriteStringLn(ar,TAG_PROJECT_PREBUILD_STEPS,"\n{");
			int index =1;
			for (list_build_step::iterator i = m_lstPreBuildSteps.begin();
				 i!=m_lstPreBuildSteps.end();++i,index++)
			{
				string_smart strLine ("\"",
					(*i).m_strDescription.c_str(),"\" | \"",
					(*i).m_strCommandLine.c_str(),"\"");

				WriteStringLn(ar,"  ",strLine);
			}
			WriteStringLn(ar,"}");
		}
		//Сохраняем список PostBuildSteps
		if (m_lstPostBuildSteps.size()!=0){
			WriteStringLn(ar,TAG_PROJECT_POSTBUILD_STEPS,"\n{");
			int index =1;
			for (list_build_step::iterator i = m_lstPostBuildSteps.begin();
				 i!=m_lstPostBuildSteps.end();++i,index++)
			{
				string_smart strLine ("\"",
					(*i).m_strDescription.c_str(),"\" | \"",
					(*i).m_strCommandLine.c_str(),"\"");

				WriteStringLn(ar,"  ",strLine);
			}
			WriteStringLn(ar,"}");
		}
	}
	else
	{
		CString	strIn;
		for	(; strIn.IsEmpty();ar.ReadString(strIn));  
		if (strIn.CompareNoCase(TAG_PROJECT_SIGN)!=0)
		{
			guiMsgAttention(0,Format(string_res(IDS_PROJECT_FILE_BAD_s,HRESOURCES).c_str(),
				LPCTSTR(ar.GetFile()->GetFileName())).c_str());
			return;
		} 
		Props.ReadFromArchive(ar);
		m_strParams				= Props.get(TAG_PROJECT_PARAMS).c_str();
		m_strParamsBrowseDir	= Props.get(TAG_PROJECT_PARAMS_BROWSE_DIR).c_str();		
		m_strNotes				= Props.get(TAG_PROJECT_NOTES).c_str();
		m_strSwitches			= Props.get(TAG_PROJECT_SWITCHES).c_str();
		if (!Props.is_exists(TAG_PROJECT_SWITCHES))
			m_strSwitches=string_smart(szSwitch_SFX," ",szSwitch_DEBUG_INFO," ",szSwitch_OPTIMIZATION," ");

		//Читаем Output Directory
		m_strOutputDirectory=Props.get(TAG_PROJECT_OUTPUT_DIR).c_str();
		if (!Props.is_exists(TAG_PROJECT_OUTPUT_DIR))
			m_strOutputDirectory=".\\";
		m_strIntermediateDirectory=Props.get(TAG_PROJECT_INTERMEDIATE_DIR).c_str();
		if (!Props.is_exists(TAG_PROJECT_INTERMEDIATE_DIR))
			m_strIntermediateDirectory=m_strOutputDirectory;

		//Читаем названия входных и выходных языков
		m_strSourceLanguage = Props.get(TAG_PROJECT_SRC_LANG).c_str();
		m_strTargetLanguage = Props.get(TAG_PROJECT_DST_LANG).c_str();

		//InputTranslationDir + OutputTranslationDir
		m_strInputTranslationDir = Props.get(TAG_PROJECT_L_IN_DIR);
		m_strOutputTranslationDir= Props.get(TAG_PROJECT_L_OUT_DIR);

		_ASSERT(m_strParams.c_str()	&& m_strParamsBrowseDir.c_str()	&& m_strNotes.c_str() && m_strSwitches.c_str());
		
		//Получение	списка исходных	файлов
		string_list	lstSources	= Props.get(TAG_PROJECT_SOURCES);

		ASSERT(!m_strWorkDir.isempty());				
		m_lstSources.clear();
		string_cstr strBaseDir=include_backslash(m_strWorkDir);
		for	(size_t i=0; i<lstSources.items().count();i++)
			if (!lstSources.items().get_at(i).trim().isempty())
				m_lstSources.push_back(get_abs_path(lstSources.items()[i],strBaseDir));

		string_parse strLine;
		strLine.AddSkipRanges("\"","\"");

		//Получение списка PreBuildSteps
		string_list lstPreBuildSteps = Props.get(TAG_PROJECT_PREBUILD_STEPS);
		for (size_t i = 0; i<lstPreBuildSteps.items().count();++i){
			strLine = lstPreBuildSteps.items()[i];
			string_smart strDesc; string_smart strCommand;
			if (strLine.split("|",strDesc,strCommand))
			{
				strDesc.trim().frames_delete('"','"');
				strCommand.trim().frames_delete('"','"');
				if (!strDesc.isempty() || !strCommand.isempty())
					m_lstPreBuildSteps.push_back(build_step(strDesc,strCommand));
			}
		}

		//Получение списка PostBuildSteps
		string_list lstPostBuildSteps = Props.get(TAG_PROJECT_POSTBUILD_STEPS);
		for (size_t i = 0; i<lstPostBuildSteps.items().count();++i){
			strLine = lstPostBuildSteps.items()[i];
			string_smart strDesc; string_smart strCommand;
			if (strLine.split("|",strDesc,strCommand))
			{
				strDesc.trim().frames_delete('"','"');
				strCommand.trim().frames_delete('"','"');
				if (!strDesc.isempty() || !strDesc.isempty())
					m_lstPostBuildSteps.push_back(build_step(strDesc,strCommand));
			}
		}

		//Получение списка генераторов кода на ЯП
		m_lstLangGens = Props.get(TAG_PROJECT_LANG_GEN);
	} 
	LOG_PROTECT_CODE_END
}
 
/////////////////////////////////////////////////////////////////////////////
// CProjectDoc commands
//------------------------------------------------------------------------
BOOL CProjectDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CProjectAutoState AutoState	(*this,opSaving);//В деструкторе автоматически снимется
	BOOL res=false;
	LOG_PROTECT_CODE_BEGIN
	res=__super::OnSaveDocument(lpszPathName);
	LOG_PROTECT_CODE_END
	return res;
}
//------------------------------------------------------------------------
BOOL CProjectDoc::OnNewDocument()
{  
	CProjectAutoState AutoState	(*this,opCreating);//В деструкторе автоматически снимется
	m_bAutoDelete=true;

	INewProjectLink * pNewProjectLink = i_objects::GetInterface<INewProjectLink >();
	_ASSERT(pNewProjectLink);

	if (!__super::OnNewDocument() || !pNewProjectLink) {
		AutoState.Rollback(); 
		OnCloseDocument();
		return FALSE;
	}

	string_smart strProjectDir,strProjectName;
	string_smart strLastProjectDir="C:\\";

	IStorageLink * pStorageLink = singleton_cache::GetStorageLink();
	if (pStorageLink){
		string_smart s = pStorageLink->GetLastOpenedProject();
		if (!s.isempty())
			strLastProjectDir = extract_filedir(s);
	}
	
	INewProjectWizardLink* pWizardLink;
	if (!pNewProjectLink->Run("",
							strLastProjectDir,
							strProjectName,
							strProjectDir,
							pWizardLink
							))
	{
		AutoState.Rollback(); 
		OnCloseDocument(); 
		return false;
	}

	string_smart strTargetDir(include_backslash(strProjectDir),strProjectName);

	if (!force_dirs(strTargetDir)){
		guiMsgError(0,FormatRes(IDS_CANT_CREATE_DIR_s,HRESOURCES,strTargetDir.c_str()));	
		AutoState.Rollback(); 
		OnCloseDocument(); 
		return FALSE;
	}

	IDocMngrLink* pDocMngrLink=singleton_cache::GetDocMngrLink();
	ASSERT_RETURN_FALSE(pDocMngrLink);
	pDocMngrLink->SetRecentlyDir(strTargetDir);

	CString	strProjectFileName,strProjectExt; 
	GetDocTemplate()->GetDocString(strProjectExt,CDocTemplate::filterExt);
	strProjectFileName=include_backslash(strTargetDir).c_str();
	strProjectFileName+=exclude_backslash(strProjectName).c_str();
	strProjectFileName+=strProjectExt;
	
	//Не даем переписывать проекты, от греха подальше, криво, потому как спрашивать надо было раньше...
	//надо переделать форму диалога
	if (is_file_exist(strProjectFileName)){
		guiMsgError(0,FormatRes(IDS_PROJECT_FILE_ALREADY_EXISTS_s,HRESOURCES,(LPCTSTR)strProjectFileName));
		AutoState.Rollback(); 
		OnCloseDocument();
		return FALSE;
	}

	//Назначаем	рабочую	директорию
	m_strWorkDir = strTargetDir;//Переприсваивание должно быть в конце,	когда  strTargetDir	уже	не нужен

	//Сохраняем	файл проекта
	if (!OnSaveDocument(strProjectFileName)){
		AutoState.Rollback(); SetModifiedFlag(false); //нужно?
		OnCloseDocument();
		return FALSE;
	}

	//Назначаем	рабочую	директорию
	m_strWorkDir= extract_filedir(strProjectFileName);
	m_strPath	= strProjectFileName;
	m_strTitle	= extract_filename(strProjectFileName);

	//Запускаем	мастер
	if (pWizardLink)
	{
		if (pWizardLink->Run(this))
			if (IsModified())
				OnSaveDocument(strProjectFileName);	
	}
	

	AutoState.Rollback();//Нужно снять состояние, чтобы	процедйра открытия могла свободно возбуждать события
	//Открываем	заново файл	проекта
	if (!OnOpenDocument(strProjectFileName))
		return FALSE;

	m_bAutoDelete=false;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CProjectDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	bool res=false;
	CProjectAutoState AutoState	(*this,opOpening);
	CStatusProcessMessage msg(FormatResCSTR(IDS_FILE_LOADING_s,HRESOURCES,extract_filenameext(lpszPathName).c_str()));
	m_bAutoDelete=true;

	LOG_PROTECT_CODE_BEGIN
	//Назначаем	рабочую	директорию
	m_strWorkDir= extract_filedir(lpszPathName);
	m_strPath		= lpszPathName;
	m_strTitle		= extract_filename(lpszPathName);

	if (!__super::OnOpenDocument(lpszPathName)){
		m_strWorkDir.erase();
		m_strPath.erase();
		m_strTitle.erase();
		return FALSE;
	}

	IStorageLink * pStorageLink = singleton_cache::GetStorageLink();
	if (pStorageLink)
		pStorageLink->SetLastOpenedProject(m_strWorkDir);
	
	ASSERT(!IsModified());

	//Структура	данных о проекте + интерфейс к проекту IProjectDocLink
	evdOpenProjectInfo Info(GetName().c_str(),GetPath().c_str(),*this);
	RaiseOpenEvent(Info);

	//Присваиваем имя проекта базовому классу. Ибо MFC это не всегда делает
	//например,	при	создании нового	проекта	базовые	переменные пустые
	m_strPathName=m_strPath.c_str(); //Не надо SetPathName - он	тянет кучу событий
	res=true;
	LOG_PROTECT_CODE_END
 
	AutoState.Rollback();//Даем	зеленый	свет событиям
	if (res){		
		LOG_PROTECT_CODE(LoadDSK());			//Загружаем	состояние проекта
		Activate(true);	//Активизируем проект. Последний открытый проект всегда	становится активным
	}

	m_bAutoDelete=!res;		//Если открылся, удалять не	надо

	return res;
}
//------------------------------------------------------------------------
bool CProjectDoc::OnCloseDocumentInternal()
{
	LOG_PROTECT_CODE_BEGIN
	if (m_strPath.isempty()) //Документ	может закрываться, не открывшись, см OnNewDocument,	OnOpenDocument
	{
		ASSERT(m_lstSources.size()==0);
		ASSERT(m_bAutoDelete); //Если документ не был должным образом открыт, он должен	закрываться	сам
		__super::OnCloseDocument();
		return true;
	}

	//Надо проверить, не нужно ли документ сохранить
	if (!SaveModified()) 
		return false;

	//Надо проверить, ни идет ли компляция нас самих в этот момент...
	IBuilderLink * pBuilderLink	= GetBuilderInterface();
	if ((pBuilderLink) && (pBuilderLink->IsBusy()) && (pBuilderLink->GetBuildingProject()==this)){
		if (guiMsgConfirm(0,Format(string_res(IDS_CLOSE_PROJECT_BUILD_IN_PROGRESS,HRESOURCES).c_str()).c_str()))
			pBuilderLink->Stop();
		else
			return false;
	}

	//Надо проверить, а не идет ли отладка в данный момент
	IRunnerLink * pRunnerLink = GetRunnerInterface();
	IRunnerLink::EActivityType nActivityType;
	IProjectDocLink * pActiveProjectLink;
	if ((pRunnerLink) && (pRunnerLink->IsBusy(nActivityType,pActiveProjectLink)) && (pActiveProjectLink==this))
	{
		if (guiMsgConfirm(0,FormatRes(IDS_CLOSE_PROJECT_DEBUG_IN_PROGRESS,HRESOURCES).c_str()))
			pRunnerLink->Stop();
		else
			return false;
	}
	LOG_PROTECT_CODE_END

	LOG_PROTECT_CODE_BEGIN
		SaveDSK(); //Сохраняем состояние проекта
	LOG_PROTECT_CODE_END

	LOG_PROTECT_CODE_BEGIN
	//Закрываем	все	окна
	IDocMngrLink * pDocMngrLink = singleton_cache::GetDocMngrLink();
	for	(list_sources::iterator	i=m_lstSources.begin();i!=m_lstSources.end();++i)
	{
		IDocumentLink * pDoc=pDocMngrLink->FindDocument((*i).c_str());
		if (pDoc && !pDoc->Close())
			return false; //Документы	не захотели	закрываться
	}
	m_LastAccessedDoc.Clear();//на всякий случай
	LOG_PROTECT_CODE_END

	LOG_PROTECT_CODE_BEGIN
	ASSERT(GetPathName()==m_strPath.c_str());
	//AutoState	должен отключиться до delete this, поэтому ставим доп скобки;
	{
		CProjectAutoState AutoState	(*this,opClosing);
		//Деактивизируем обязательно!!!
		Deactivate();
		//Структура	данных о проекте + интерфейс к проекту IProjectDocLink
		evdCloseProjectInfo	Info(GetName().c_str(),GetPath().c_str(),*this);
		RaiseCloseEvent(Info);

		__super::OnCloseDocument();
		ASSERT_VALID(this);
	}
	LOG_PROTECT_CODE_END
	
	delete this;
	return true;
}
//------------------------------------------------------------------------
void CProjectDoc::OnCloseDocument()	
{
	OnCloseDocumentInternal();
}
//------------------------------------------------------------------------
void CProjectDoc::SetModifiedFlag(BOOL bModified /*= TRUE*/)
{
	bool bJustModified	= bModified!=IsModified();
	__super::SetModifiedFlag(bModified);

	if (!bJustModified)	
		return;

	evdChangeProjectInfo Info(GetName().c_str(),GetPath().c_str(),*this);
	if (bModified)
		Info.in_nChangeType=evdChangeProjectInfo::changeModify;
	else
		Info.in_nChangeType=evdChangeProjectInfo::changeSave;

	RaiseChangeEvent(Info);
}
//------------------------------------------------------------------------
POSITION CProjectDoc::GetFirstViewPosition() const
{
	return NULL;			// У нас как бы	нет	Views
}
//------------------------------------------------------------------------
BOOL CProjectDoc::SaveModified() 
{
	if	(m_strPath.isempty())		//Документ может закрываться, не открывшись, см	OnNewDocument, OnOpenDocument
		return true;

	ASSERT(GetPathName()==m_strPath.c_str());

	if (!IsModified())
		return TRUE;			// ok to continue

	string_smart strText=Format(string_res(IDS_PROJECT_WAS_CHANGED_s,HRESOURCES).c_str(),LPCTSTR(GetTitle()));
	switch (guiMsgYesNoCancel(0,strText.c_str()))
	{
	case IDYES:
		return OnSaveDocument(m_strPath.c_str());
	case IDNO:
		return true;
	case IDCANCEL:
		return false;
	default:
		ASSERT(false);
	}

	return TRUE;	// keep	going
}
//------------------------------------------------------------------------
void CProjectDoc::Activate(bool	bForceActiation)
{
	static int nRecrsn=0;	//Без этой проверки	возможен пинг-понг
	if (nRecrsn) return;	//между	DocManager и нами

	nRecrsn++;
	LOG_PROTECT_CODE_BEGIN

		//Получаем указатель на	DocManager-интерфейс
		IDocMngrLink * pDocMngrLink	= GetDocManagerInterface();
	if (pDocMngrLink) 
		if ((bForceActiation) || (!pDocMngrLink->GetActiveProject()))
		{
			pDocMngrLink->SetActiveProject(this);

			//Рассылаем	событие	о смене	активного проекта
			evdChangeProjectInfo Info(GetName().c_str(),GetPath().c_str(),*this);
			Info.in_nChangeType=evdChangeProjectInfo::changeActivate;
			RaiseChangeEvent(Info);
		}

	LOG_PROTECT_CODE_END
	nRecrsn--;
}
//------------------------------------------------------------------------
void CProjectDoc::Deactivate()
{
	static int nRecrsn=0;
	if (nRecrsn) return;

	nRecrsn++;
	LOG_PROTECT_CODE_BEGIN

	//Получаем указатель на	DocManager-интерфейс
	IDocMngrLink * pDocMngrLink	= GetDocManagerInterface();
	if (pDocMngrLink)
		if (pDocMngrLink->GetActiveProject()==this)		
		{
			//Ищем кого-нибудь другого на наше место место
			CDocTemplate * pDocTempl = GetDocTemplate();
			CDocument *pDoc=this;
			for	(POSITION pos =	pDocTempl->GetFirstDocPosition(); (pos)	&& (pDoc==this);)	
				pDoc=pDocTempl->GetNextDoc(pos);

			pDocMngrLink->SetActiveProject(((pDoc) && (pDoc!=this))
				? static_cast<CProjectDoc*>(pDoc)
				: NULL);

			//Рассылаем	событие	о смене	активного проекта
			evdChangeProjectInfo Info(GetName().c_str(),GetPath().c_str(),*this);
			Info.in_nChangeType=evdChangeProjectInfo::changeDeactivate;
			RaiseChangeEvent(Info);
		}

	LOG_PROTECT_CODE_END
	nRecrsn--;
}
//------------------------------------------------------------------------
BOOL CProjectDoc::OnCmdMsg(UINT	nID, int nCode,	void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//Избавляемся от обработки		ID_FILE_CLOSE,ID_FILE_SAVE,ID_FILE_SAVE_AS
	if (oneof((UINT)nID, (UINT)ID_FILE_CLOSE,(UINT)ID_FILE_SAVE,(UINT)ID_FILE_SAVE_AS))
		return false;

	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
//------------------------------------------------------------------------
void CProjectDoc::OnIdle()
{
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnClose() 
{
	Close();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnSave() 
{
	Save();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnUnloadProject()
{	
	Close();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnUnloadProject(CCmdUI *pCmdUI)
{
	//Добавляем	к названию пункта название проекта
	MENU_ADD_PROJECT_NAME(pCmdUI);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnBuild()
{ 
	Build();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnClean()
{
	Clean();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnBuild(CCmdUI *pCmdUI)
{
	IBuilderLink * pBuilderLink		=GetBuilderInterface();
	pCmdUI->Enable((pBuilderLink) && (!pBuilderLink->IsBusy()));

	//Добавляем	к названию пункта название проекта
	MENU_ADD_PROJECT_NAME(pCmdUI);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnBuildStop()
{
	IBuilderLink * pBuilderLink	=GetBuilderInterface();
	if (pBuilderLink) 
		pBuilderLink->Stop();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnBuildStopBuild(CCmdUI *pCmdUI)
{
	IBuilderLink * pBuilderLink		=GetBuilderInterface();
	pCmdUI->Enable((pBuilderLink) && (pBuilderLink->IsBusy()));
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnDebugStart()
{
	IRunnerLink	* pRunnerLink	= GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);

	//Идет отладка и сейчас она ждет пользователя
	if (pRunnerLink->IsBusy() && pRunnerLink->IsPaused())
		pRunnerLink->Resume();
	else
		Debug(); //запускаем отладку
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnDebugStart(CCmdUI *pCmdUI)
{
	//Добавляем	к названию пункта название проекта
	MENU_ADD_PROJECT_NAME(pCmdUI);

	IRunnerLink	* pRunnerLink		=GetRunnerInterface();
	IBuilderLink * pBuilderLink		=GetBuilderInterface();
	
	pCmdUI->Enable((pRunnerLink) &&	 (!pRunnerLink->IsBusy() || pRunnerLink->IsPaused()) &&
				   (pBuilderLink) && (!pBuilderLink->IsBusy()));
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnRunStart()
{
	Run();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnRunStart(CCmdUI *pCmdUI)
{
	//Добавляем	к названию пункта название проекта
	MENU_ADD_PROJECT_NAME(pCmdUI);

	IRunnerLink	* pRunnerLink		=GetRunnerInterface();
	IBuilderLink * pBuilderLink		=GetBuilderInterface();
	pCmdUI->Enable((pRunnerLink) &&	 (!pRunnerLink->IsBusy()) &&
				   (pBuilderLink) && (!pBuilderLink->IsBusy()));
}		
//------------------------------------------------------------------------
void CProjectDoc::cmdOnDebugStop()
{
	IRunnerLink	* pRunnerLink	  =GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);
	pRunnerLink->Stop();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnDebugStop(CCmdUI *pCmdUI)
{
	IRunnerLink	* pRunnerLink	  =GetRunnerInterface();
	pCmdUI->Enable((pRunnerLink) &&	(pRunnerLink->IsBusy()));
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnDebugStepInto()
{
	IRunnerLink	* pRunnerLink		=GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);
	pRunnerLink->ResumeStepIn();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnDebugStepInto(CCmdUI *pCmdUI)
{
	IRunnerLink	* pRunnerLink		=GetRunnerInterface();
	pCmdUI->Enable((pRunnerLink) &&	pRunnerLink->IsPaused());
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnDebugStepOver()
{
	IRunnerLink	* pRunnerLink		=GetRunnerInterface();
	ASSERT_RETURN_VOID(pRunnerLink);
	pRunnerLink->ResumeStepOver();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnDebugStepOver(CCmdUI *pCmdUI)
{
	IRunnerLink	* pRunnerLink		=GetRunnerInterface();
	pCmdUI->Enable((pRunnerLink) &&	pRunnerLink->IsPaused());
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnAddRule()
{
	AddRule();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnAddFiles()
{
	AddFile(NULL);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnAddCodeTemplate()
{
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnSettings(CCmdUI *pCmdUI)
{
	//Добавляем	к названию пункта название проекта
	MENU_ADD_PROJECT_NAME(pCmdUI);
}
//------------------------------------------------------------------------	
void CProjectDoc::cmdOnSettings()
{
	IProjPropsLink*	pProjPropsLink = i_objects::GetInterface<IProjPropsLink>();
	ASSERT_RETURN_VOID(pProjPropsLink);
		
	pProjPropsLink->Run(this);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnStatistic()
{
	//Получаем указатель на	Statistic-интерфейс
	IStatisticLink * pStatistic=i_objects::GetInterface<IStatisticLink>();
	ASSERT_RETURN_VOID(pStatistic);
	
	pStatistic->Run(this);
}
/*void CProjectDoc::cmdupOnStatistic(CCmdUI *pCmdUI){
	
}*/
//------------------------------------------------------------------------
void CProjectDoc::cmdOnSyntaxDiagrams()
{
	//Получаем указатель на	SyntaxDiagrams-интерфейс
	ISyntaxDiagramsLink	* pSyntaxDiagrams=i_objects::GetInterface<ISyntaxDiagramsLink>();
	ASSERT_RETURN_VOID(pSyntaxDiagrams);
	pSyntaxDiagrams->Run(this,ISyntaxDiagramsLink::stDiagram);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnParsingCompleteReport(){
	m_nParsingReportType=m_nParsingReportType ^ PARSE_REPORT_FULL;
	if (m_nParsingReportType & PARSE_REPORT_FULL) 
		m_nParsingReportType=m_nParsingReportType & ~PARSE_REPORT_SHORT;
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnParsingCompleteReport(CCmdUI *pCmdUI){
	pCmdUI->SetCheck((m_nParsingReportType & PARSE_REPORT_FULL)!=0);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnParsingShortReport(){
	m_nParsingReportType=m_nParsingReportType ^ PARSE_REPORT_SHORT;
	if (m_nParsingReportType & PARSE_REPORT_SHORT) 
		m_nParsingReportType=m_nParsingReportType & ~PARSE_REPORT_FULL;
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnParsingShortReport(CCmdUI *pCmdUI){
	pCmdUI->SetCheck((m_nParsingReportType & PARSE_REPORT_SHORT)!=0);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnParsingReportToFile(){
	m_bParsingReportToFile=!m_bParsingReportToFile;
	if (m_bParsingReportToFile){
		IMainFrameLink	* pMainFrm=i_objects::GetInterface<IMainFrameLink>();
		ASSERT_RETURN_VOID(pMainFrm);{
			string_smart s = GetParseLogPath().frames_wrap_on('\'','\'');
			s = Format(string_res(IDS_PARSE_REPORT_WILL_TO_FILE_s,HRESOURCES).c_str(),s.c_str());
			pMainFrm->ShowTip(s.insert(_T(' '),0).c_str());
		}
	}
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnParsingReportToFile(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(m_bParsingReportToFile);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnDebugPauseOnParse(){
	m_bParsingStopAtMessage=!m_bParsingStopAtMessage;
}
//------------------------------------------------------------------------
void CProjectDoc::cmdupOnDebugPauseOnParse(CCmdUI *pCmdUI){
	pCmdUI->SetCheck(m_bParsingStopAtMessage);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnAddToKnowledgeBase()
{
	AddToKnowledgeBase();
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnExportToXBNF()
{
	//Запуск разбора Разбор полученных результатов
	IParserLink::syntax_info SInfo;
	IParserLink * pParserIface = i_objects::GetInterface<IParserLink>();
	ASSERT_RETURN_VOID(pParserIface);

	IDocMngrLink * pDocMngrLink = GetDocManagerInterface();
	ASSERT_RETURN_VOID(pDocMngrLink);

	string_smart s;
	IParserLink::syntax_info si;

	pParserIface->ParseWholeProject	(*this, si, true);
	pParserIface->AssembleToXBNF(si,s); 
	string_smart strFileName = get_temp_filename(".txt",Format("XBNF(%s) ",this->GetName().c_str()).c_str());
	s.replace("\r\n","\n");
	if (!save_file(strFileName.c_str(),s.c_str()))
		guiMsgError(0,string_resf(IDS_CANT_WRITE_TO_FILE_s,HRESOURCES,strFileName.c_str()).c_str());
	else{
		IDocumentLink * pDoc = pDocMngrLink->OpenDocument(strFileName.c_str());
	}
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnProjectBuildSyntaxDiagram()
{
	IDocMngrLink * pDocMngrLink = GetDocManagerInterface();
	ASSERT_RETURN_VOID(pDocMngrLink);

	//Получаем указатель на	SyntaxDiagrams-интерфейс
	ISyntaxDiagramsLink	* pSyntaxDiagrams=i_objects::GetInterface<ISyntaxDiagramsLink>();
	ASSERT_RETURN_VOID(pSyntaxDiagrams);
	pSyntaxDiagrams->Run(this,ISyntaxDiagramsLink::stDiagram);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnProjectBuildSyntaxTree()
{
	IDocMngrLink * pDocMngrLink = GetDocManagerInterface();
	ASSERT_RETURN_VOID(pDocMngrLink);

	//Получаем указатель на	SyntaxDiagrams-интерфейс
	ISyntaxDiagramsLink	* pSyntaxDiagrams=i_objects::GetInterface<ISyntaxDiagramsLink>();
	ASSERT_RETURN_VOID(pSyntaxDiagrams);
	pSyntaxDiagrams->Run(this,ISyntaxDiagramsLink::stTree);
}
//------------------------------------------------------------------------
void CProjectDoc::cmdOnProjectNewSource()
{
	AddNewFile();
}
