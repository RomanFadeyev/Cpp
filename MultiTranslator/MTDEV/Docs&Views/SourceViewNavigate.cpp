/********************************************************************
	created:	2002/02/02
	created:	2:2:2002   17:08
	filename:	C:\Projects\SC\Source\SourceViewNavigate.cpp
	file path:	C:\Projects\SC\Source

	file base:	SourceViewNavigate
	file ext:	cpp
	author: 	Fadeyev R.V.
	
	purpose:	Реализация класса-обертки длдя управления диалогами
				поиска, замены, перемещения
*********************************************************************/
#include "stdafx.h"
#include "SourceView.h" //Нужно, так как класс вложен в SourceView

#include <Scintilla.h>
#include <GUIService.h>	//для guiMsgAttention

#include "Names.h"
#include "BaseStructs.h"

static const char_t MSG_TEXT_NOT_FOUND_s[]			=_T("Cannot find the string '%s'");
static const char_t MSG_DECLARATION_NOT_FOUND_s[]	=_T("Cannot find the decalartion of '%s'");
static const char_t MSG_BK_NOT_FOUND[]				=_T("No more bookmarks found");

/////////////////////////////////////////////////////////////////////////////
// CSourceView::CNavigation
/////////////////////////////////////////////////////////////////////////////

CSourceView::CNavigation::CNavigation(CSourceView & aOwner)
	:m_rMessenger(aOwner.m_Messenger),m_rImage(aOwner.m_Image)
{
	//m_NotifyWnd.Tune(true,false,DT_LEFT,RGB(200,200,250),RGB(6,60,60));
	//m_NotifyWnd.Tune(true,false,DT_LEFT,RGB(255,255,255),RGB(255,0,0));
}
void CSourceView::CNavigation::ShowWarning(const string_smart & strText)
{
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	
	if (pStorage && !pStorage->Find_ShowTipNotMsgBox){
		guiMsgInformation(0,strText.c_str());
		return;
	}

	if (m_spUtils){
	}
	else {
		_com_error err(m_spUtils.CreateInstance(__uuidof(mtdev_lib::Utils)));
		if (!SUCCEEDED(err.Error()))
		{
			LOG_EXCEPTION(err.ErrorMessage(),&err);
			return ;
		}
	}

	m_spUtils->ShowPopupTip(strText.c_str(),"",mtdev_lib::msgcWarning,(OLE_HANDLE)m_rMessenger.GetWnd());

	/*CWnd *pViewWnd= CWnd::FromHandle(m_rMessenger.GetWnd());
	CRect ViewRect;	pViewWnd->GetClientRect(&ViewRect);

	ViewRect.top=ViewRect.bottom-m_NotifyWnd.FindAutoHeight()-2;
	ViewRect.left=m_rMessenger.GetMarginsWidth()+1;
	m_NotifyWnd.SetPosition(pViewWnd,ViewRect);
	m_NotifyWnd.Show(strText.c_str());*/
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::IsFindNextAvailable()
{
	return (!m_FindInfo.out_strText.isempty());
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::FindNextRoutine(bool bShowNotFound/*=true*/)
{ 
 	long dwStart= m_FindInfo.in_dwInitialSelStart;
	long dwEnd	= m_FindInfo.in_dwInitialSelEnd;
	
	if (!IsFindNextAvailable())
		return false;

	if (!m_FindInfo.out_bInSelection)	//ищем по всему тексту
	{
		dwStart=m_rMessenger.GetCurPos();
		dwEnd= (m_FindInfo.out_bSearchDown) ? m_rMessenger.GetTextLen() :0;
	}
	else	//Ищем в выделенном
	{	
		if (m_FindInfo.out_bSearchDown)
			dwEnd^=dwStart^=dwEnd^=dwStart;
	}
	long dwFlags =(m_FindInfo.out_bWholeWord ? SCFIND_WHOLEWORD : 0) |
		(m_FindInfo.out_bMatchCase ? SCFIND_MATCHCASE : 0) |
		(m_FindInfo.out_bAsRegexp  ? SCFIND_REGEXP    : 0);
	//m_rMessenger.EnsureRangeVisible(dwStart,dwEnd);

	m_rMessenger.Perform(SCI_SETTARGETSTART,dwStart);
	m_rMessenger.Perform(SCI_SETTARGETEND,  dwEnd  );
	m_rMessenger.Perform(SCI_SETSEARCHFLAGS,dwFlags);

	long dwPosFind = m_rMessenger.Perform(SCI_SEARCHINTARGET, 
		m_FindInfo.out_strText.length(), 
		m_FindInfo.out_strText.c_str());
	if ((dwPosFind==-1) && (m_FindInfo.out_bWrapAround)){
		m_rMessenger.Perform(SCI_SETTARGETSTART,0);
		m_rMessenger.Perform(SCI_SETTARGETEND,  m_FindInfo.in_dwInitialSelStart);
		dwPosFind = m_rMessenger.Perform(SCI_SEARCHINTARGET, 
									m_FindInfo.out_strText.length(), 
									m_FindInfo.out_strText.c_str());
	}

	if (dwPosFind!=-1){
		m_rMessenger.EnsureRangeVisible(dwPosFind,dwPosFind+m_FindInfo.out_strText.length());
		m_rMessenger.GotoPos(dwPosFind);
		m_rMessenger.SelectRange(dwPosFind,dwPosFind+m_FindInfo.out_strText.length());
		return true;
	}
	else 
	{
		if (bShowNotFound)
			ShowWarning(Format(MSG_TEXT_NOT_FOUND_s,m_FindInfo.out_strText.c_str()));
		return false;
	} 
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::Find(bool bFindNext)
{
	m_FindInfo.in_dwInitialSelStart = m_rMessenger.GetSelStart();
	m_FindInfo.in_dwInitialSelEnd = m_rMessenger.GetSelEnd();

	if (!bFindNext)
	{
		long nStart,nPos;
		m_FindInfo.in_bInSelection=m_rMessenger.IsSelected();
		m_FindInfo.in_strText=m_rImage.GetWordAt(m_rMessenger.GetCurPos(),nStart,nPos);
		
		//Если есть выделение, смотрим, а не выделенео ли всего лишь одно слово
		if (m_FindInfo.in_bInSelection){
			if (m_rMessenger.GetSelText()==m_FindInfo.in_strText)
				m_FindInfo.in_bInSelection=false;
			else
				m_FindInfo.in_strText="";	//Если есть выделение, текста по умолчанию быть не может
		}

		IStorageLink * pStorage = singleton_cache::GetStorageLink();
		if (pStorage && !pStorage->Find_PutTheText)	//Опция "не вставлять текст"
			m_FindInfo.in_strText="";				//

		if (!i_objects::GetInterface<IFindTextLink>()->Run(m_FindInfo))
			return false;
	}
	return FindNextRoutine();
}
//------------------------------------------------------------------------
void CSourceView::CNavigation::SetTextToFind(cstr szText)
{
	m_FindInfo.out_strText=szText;
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::ReplaceNextRoutine(bool bShowNotFound/*=true*/)
{
	string_smart s	= m_rMessenger.GetSelText();
	bool bFound = (m_ReplaceInfo.out_bMatchCase)
					? s.cmp(m_ReplaceInfo.out_strText)
					: s.cmpi(m_ReplaceInfo.out_strText);
	if (!bFound)	//Текст не выделен
		return FindNextRoutine(bShowNotFound);
	else{
		if 	(std::find(m_lstReplacedPositions.begin(),m_lstReplacedPositions.end(),m_rMessenger.GetSelStart())!=m_lstReplacedPositions.end())
			return false; //Пошли по второму кругу

		//Запоминаем, где мы что измнили
		m_lstReplacedPositions.push_back(m_rMessenger.GetSelStart());
		m_rMessenger.Perform(SCI_REPLACESEL,			//Текст найден, осалось только поменять
						m_ReplaceInfo.out_strReplaceText.length(), 
						m_ReplaceInfo.out_strReplaceText.c_str());
		FindNextRoutine(false);//Перебираемся к следующему элементу
		return true;
	}
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::Replace()
{ 
	long dwStart= m_rMessenger.GetSelStart();
	long dwEnd	= m_rMessenger.GetSelEnd();

	m_ReplaceInfo.in_dwInitialSelStart = m_rMessenger.GetSelStart();
	m_ReplaceInfo.in_dwInitialSelEnd = m_rMessenger.GetSelEnd();
	m_FindInfo.in_dwInitialSelStart = m_ReplaceInfo.in_dwInitialSelStart;
	m_FindInfo.in_dwInitialSelEnd = m_ReplaceInfo.in_dwInitialSelEnd;

	long nWordStart, nWordEnd;
	m_ReplaceInfo.in_bInSelection=m_rMessenger.IsSelected();
	m_ReplaceInfo.in_strText=m_rImage.GetWordAt(m_rMessenger.GetCurPos(),nWordStart,nWordEnd);	
	
	//Если есть выделение, смотрим, а не выделенео ли всего лишь одно слово
	if (m_ReplaceInfo.in_bInSelection){
		if (m_rMessenger.GetSelText()==m_ReplaceInfo.in_strText)
			m_ReplaceInfo.in_bInSelection=false;
		else
			m_ReplaceInfo.in_strText="";	//Если есть выделение, текста по умолчанию быть не может
	}

	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	if (pStorage && !pStorage->Find_PutTheText)	//Опция "не вставлять текст"
		m_FindInfo.in_strText="";				//

	//Здесь необходимо учитывать, что ReplaceDialog должен быть открыт только 
	//в модальном режиме, иначе использование функции обратного может привести к 
	//неприятностям
	m_ReplaceInfo.in_pfCallback.assign(this,ReplaceCallback);
	i_objects::GetInterface<IReplaceTextLink>()->Run(m_ReplaceInfo);
	return true;
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::ReplaceCallback(IReplaceTextLink::vbdReplace::operations lOp, HWND hwnd)
{
	m_FindInfo.out_bAsRegexp	= m_ReplaceInfo.out_bAsRegexp;
	m_FindInfo.out_bInSelection	= m_ReplaceInfo.out_bInSelection;
	m_FindInfo.out_bMatchCase	= m_ReplaceInfo.out_bMatchCase;
	m_FindInfo.out_bSearchDown	= m_ReplaceInfo.out_bSearchDown;
	m_FindInfo.out_bWholeWord	= m_ReplaceInfo.out_bWholeWord;
	m_FindInfo.out_bWrapAround	= m_ReplaceInfo.out_bWrapAround;
	m_FindInfo.out_strText		= m_ReplaceInfo.out_strText.c_str(); // c_str для копирования!!!

	bool result=false;
	switch (lOp)
	{
	case IReplaceTextLink::vbdReplace::opFindNext:
		result=FindNextRoutine();
		break;
	case IReplaceTextLink::vbdReplace::opReplace:
		m_FindInfo.out_bInSelection	= false; //Нельзя искать инкрементно в выделенном тексте
		m_lstReplacedPositions.clear();
		result=ReplaceNextRoutine();
		break;
	case IReplaceTextLink::vbdReplace::opReplaceAll:
		m_lstReplacedPositions.clear();
		for (int i=0; (result) || (i==0);i++) {
			long nOldPos = m_rMessenger.GetSelStart();
			result=ReplaceNextRoutine(i==0);

			//Опасность бесконечного цикла
			long nNewPos = m_rMessenger.GetSelStart();
			result=result && (nNewPos!=nOldPos);
		}
		break;
	}
	return result;
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::GotoLine()
{
	long out_Line;
	if (i_objects::GetInterface<IGotoLineLink>()->Run(m_rMessenger.GetCurLine(),m_rMessenger.GetLineCount()-1,out_Line))
	{
		m_rMessenger.GotoLine(min(out_Line,m_rMessenger.GetLineCount()-1));
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::GotoNextBookmark()
{
	long dwCurLine = m_rMessenger.GetCurLine();
	long dwNextLine = m_rMessenger.MarkerNext(dwCurLine+1,MARKER_BOOKMARK);
	if (dwNextLine  < 0)
		dwNextLine = m_rMessenger.MarkerNext(0,MARKER_BOOKMARK);
	if ((dwNextLine > 0) && (dwNextLine!=dwCurLine)) {
		m_rMessenger.GotoLine(dwNextLine);
		return true;
	}
	else{
		ShowWarning(MSG_BK_NOT_FOUND);
		return false;
	}
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::GotoPrevBookmark()
{
	long dwCurLine = m_rMessenger.GetCurLine();
	long dwPrevLine = m_rMessenger.MarkerPrev(dwCurLine-1,MARKER_BOOKMARK);
	if (dwPrevLine  < 0)
		dwPrevLine = m_rMessenger.MarkerPrev(m_rMessenger.GetLineCount(),MARKER_BOOKMARK);
	if ((dwPrevLine > 0) && (dwPrevLine!=dwCurLine)) {
		m_rMessenger.GotoLine(dwPrevLine);
		return true;
	}
	else{
		ShowWarning(MSG_BK_NOT_FOUND);
		return false;
	}
}
//------------------------------------------------------------------------
bool CSourceView::CNavigation::FindInFiles()
{
	IFindInFilesLink * pLink=i_objects::GetInterface<IFindInFilesLink>();
	ASSERT_RETURN_NULL(pLink);

	long nWordStart, nWordEnd;
	string_smart strText((m_rMessenger.IsSelected()) 
		? m_rMessenger.GetSelText()
		: m_rImage.GetWordAt(m_rMessenger.GetCurPos(),nWordStart, nWordEnd));

	return pLink->Run(strText.c_str(),i_objects::GetInterface<IOutputLink>(IOutputLink::idOutputFindInFiles));
}

//------------------------------------------------------------------------
bool CSourceView::CNavigation::FindDeclaration(const string_smart & strText, const ITEMLOCATION & aMetAt)
{
	LOG_PROTECT_CODE_BEGIN
	
	IParserLink * pParserIface = i_objects::GetInterface<IParserLink> ();
	ASSERT_RETURN_FALSE(pParserIface);
		
	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_FALSE(pDocMngrLink);

	//Разбор
	ITEMLOCATION aFoundAt;
	IParserLink::ps_res pr = pParserIface->FindDeclaration(strText.c_str(), aMetAt, aFoundAt);
	if (pr==IParserLink::psrOk)
	{
		ASSERT_RETURN_FALSE(!aFoundAt.strFileName.isempty());

		//Приводим к документу исходников (полученный документ должен быть с интерфейсом ISourceDocLink )
		ISourceDocLink * pDoc = dynamic_cast <ISourceDocLink *>(
				pDocMngrLink->OpenDocument(aFoundAt.strFileName.c_str(),NULL,true));

		if (pDoc){
			ISourceViewLink *pView = pDoc->GetViewLink();
			pView->GotoPos(aFoundAt.nStartPos);
			pView->SelectRange(aFoundAt.nStartPos,aFoundAt.nEndPos);
		
			return true;
		}
	}
	
	if (pr==IParserLink::psrFail)
		ShowWarning(Format(MSG_DECLARATION_NOT_FOUND_s,strText.c_str()));

	//Здесь остается третий вариант - psrImpossible, это для случаев, когда
	//разбор данного выражения не возможен в принципе. Например, мы ищем ключевое слово

	LOG_PROTECT_CODE_END
	return false;
}
//------------------------------------------------------------------------
