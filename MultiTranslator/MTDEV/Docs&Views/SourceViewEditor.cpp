/********************************************************************
	created:	2002/01/26
	created:	26:1:2002	10:32
	filename:	C:\Projects\SC\Source\SourceViewEditor.cpp
	file path:	C:\Projects\SC\Source

	file base:	SourceViewEditor
	file ext:	cpp
	author: 	Fadeyev R.V.
	
	purpose:	Реализация класса-обертки для работы с "Scintilla"
*********************************************************************/
#include "stdafx.h"
#include "SourceView.h"

#include <ScintillaLib.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <BaseUtils.h>
#include <Names.h>
#include <ModuleNames.h>
#include <GrmCrtLibExports.h>

#define CASH_MOST_USED_WORDS_SIZE 100
HMODULE CSourceView::CEditor::m_hModule;

//Класс-предикат для поиска строки в массиве, у которой начало соответсвует заданному
class part_of
{
private:	
	string_smart m_strPart;
public:
	part_of(cstr szText): m_strPart(szText){}
	bool operator() (const string_smart & val){
		return	(val.length()>m_strPart.length()) &&
				(strncmp(m_strPart.c_str(),val.c_str(),m_strPart.length())==0);
	}
};

/////////////////////////////////////////////////////////////////////////////
// CSourceView::CEditorContents
/////////////////////////////////////////////////////////////////////////////
CSourceView::CEditorContents::CEditorContents (CSourceView & aOwner)
	:m_rMessenger(aOwner.m_Messenger),m_LookupDir(ldRandom)
{

	Invalidate();
}
//------------------------------------------------------------------------
void CSourceView::CEditorContents::GetImage(int dwPosition)
{
//	ASSERT(m_rMessenger);
	DetectTextSize();	//Во избежании всевозможных недоразумений
	switch (m_LookupDir)
	{
	case ldForward	: m_CurRange.cpMin=max(dwPosition-dwSlopSize,0);break;
	case ldBackward : m_CurRange.cpMin=max(dwPosition-dwBufferSize+dwSlopSize,0);break;
	case ldRandom	: m_CurRange.cpMin=max(dwPosition-dwBufferSize/2,0);break;
	default 		: ASSERT(false);
	}
	m_CurRange.cpMax=min(m_CurRange.cpMin+dwBufferSize,m_dwTextSize);	
	
	TextRange tr = {{m_CurRange.cpMin,m_CurRange.cpMax}, m_aImage};
	m_rMessenger.Perform(SCI_GETTEXTRANGE, 0, reinterpret_cast<long>(&tr));
	_RPT2(_CRT_WARN,"Getting image of editor text. Range [%d,%d]\n",m_CurRange.cpMin,m_CurRange.cpMax);
}
//------------------------------------------------------------------------
string_smart CSourceView::CEditorContents::GetTextRange(long dwStart, long dwEnd)
{ 
	if (dwStart==dwEnd)	//Полезная проверка
		return "";		//
	
	ASSERT_RETURN(dwStart<dwEnd,"");

	string_smart s;
	s.reserve(dwEnd-dwStart+1);
	long k,l,d;
	for (k=dwStart,l=0; k<dwEnd;){
		if (!IsInCache(k))
			GetImage(k);
		d=min(m_CurRange.cpMax,dwEnd)-k;
		memcpy(s.buffer()+l,m_aImage+(k-m_CurRange.cpMin),d);
		l+=d;k+=d;
		//s.buffer()[l]=(*this)[k];
	}
	ASSERT(dwEnd-dwStart==l);
	s.buffer()[l]='\0';
	return s;
}
//------------------------------------------------------------------------
string_smart CSourceView::CEditorContents::GetWordAt(long dwPos,long & nStart, long & nEnd)
{
	unsigned char ch = GetChar(dwPos);
	long dwOffs=!__iscsymf(ch);
	long i,j;
	for (i=dwPos-dwOffs;(i>=0) && (IsCharAlpha(GetChar(i))||(GetChar(i)=='_'));i--);
	if (i!=0) i++;//Избавляемся от лишнего символа

	for (j=dwPos+1-dwOffs;(j<DetectTextSize()) && (IsCharAlpha(GetChar(j))||(GetChar(j)=='_'));j++);
	
	ASSERT(i<=j);
	return GetTextRange(i,j);
}
//------------------------------------------------------------------------
string_smart CSourceView::CEditorContents::GetNearestLeftWord(long dwPos,long & nStart, long & nEnd)
{
	nStart=nEnd=dwPos;
	
	//Пропускаем текущее слово
	while ((__iscsymf(GetChar(dwPos))) && (dwPos>0))
		dwPos--;

	//Пропускаем знаки препинания
	while (((isspace (GetChar(dwPos))) || (ispunct (GetChar(dwPos))) || (isdigit(GetChar(dwPos)))) && (dwPos>0))
		dwPos--;

	//Теперь слово
	if (dwPos){
		nEnd=dwPos+1;
		while (__iscsymf(GetChar(dwPos)) && (dwPos>0))
			dwPos--;
		if (dwPos)		//Возвращаем один символ
			dwPos++;	//
		nStart=dwPos;
	}
	
	if (nEnd>nStart)
		return GetTextRange(nStart,nEnd);
	return "";
}
//------------------------------------------------------------------------
string_smart CSourceView::CEditorContents::GetExpressionAt(long dwPos, long & nStart, long & nEnd)
{
	//Для начала надо проверить, не врутри ли строки мы
	
	string_parse aText = m_rMessenger.GetLineText(m_rMessenger.GetLineFromPos(dwPos));
	aText.AddSkipRanges("/*","*/");
	aText.AddSkipRanges("//","\n");

	//Сдивгаем индексацию относительно начала строки
	nStart = m_rMessenger.OffsetToStartLinePos(dwPos);	
	long dwLineOffs = dwPos - nStart;
	
	//Для начала пытаемся выяснить, не находимся ли мы в строке
	str szLPos, szRPos;
	for (cstr szPtr = aText.c_str(); (szPtr!=NULL) && (aText.findpair(szPtr,"\"","\"",szLPos,szRPos));szPtr=szRPos+1)
	{
		//Итак, мы находимся в строке
		if (inrange(dwLineOffs,(long)(szLPos-aText.c_str()),(long)(szRPos-aText.c_str()))){
			nStart+=szLPos-aText.c_str();	//Позиция относительно текста (не строки)
			nEnd = nStart+szRPos-szLPos+1;	//
			return string_smart(szLPos,szRPos-szLPos+1).c_str();
		}
		//Это очень важно! В противном случае у нас будет бесконгечный цикл
		if (!szRPos)
			break;
	}
	
	return GetWordAt(dwPos,nStart, nEnd);
}

//------------------------------------------------------------------------
// CSourceView::CEditor class
//------------------------------------------------------------------------
CSourceView::CEditor::CEditor(CSourceView & aOwner):
			m_rOwner(aOwner),
			m_rMessenger(aOwner.m_Messenger),
			m_rImage(aOwner.m_Image),
			m_bModified(false),
			m_bIndentOpening(false),
			m_bIndentClosing(false),
			m_nIndentSize(4)
{
#ifndef USE_INTERNAL_HIGHLIGHTER
	if (!m_hModule)
		m_hModule = LoadLibrary(find_full_filepath(module_names::szSourceLexer).c_str());
	if (!m_hModule)
		LOG_ERROR(MSG_ERR_FileNotFound(module_names::szSourceLexer));
#else
	m_hModule= GetModuleHandle(NULL); //Чтобы хоть чем-то его заполнить
	VERIFY(lex_hilight::QueryReady(szHilightEditorClass));
#endif
}
//------------------------------------------------------------------------
CSourceView::CEditor::~CEditor(){
}
//------------------------------------------------------------------------
ISourceDocStylerLink * CSourceView::CEditor::GetDocStyler() const
{
	ISourceDocStylerLink * pLink = i_objects::GetInterface<ISourceDocStylerLink>();
	_ASSERTE(pLink!=NULL);
	return pLink;
}
//------------------------------------------------------------------------
cstr CSourceView::CEditor::GetEditorClassName(){
	return (m_hModule) ? _T(szHilightEditorClass) : NULL;
}
//------------------------------------------------------------------------
void CSourceView::CEditor::Initialize()
{
	ASSERT(m_rMessenger.GetWnd());

	ISourceDocLink * pDocument=m_rOwner.GetDocument();
	ASSERT(pDocument);

	//Инициализируем стиль документа
	m_strDocFileExt=pDocument->GetExt();
	m_pDocStyleDescriptor = GetDocStyler()->FindStyleDescriptor(m_strDocFileExt);
	_ASSERTE(m_pDocStyleDescriptor); //В принципе, такое быть может, хотя и редко
	
	string_smart strDocPath = pDocument->GetPath();

	//Загружаем информацию из библиотек
	//Недоработка: если список библиотек поменяют в ходе работы
	//изменения не учтутся
	if (is_file_of_group(strDocPath.c_str(), EXT_SOURCES))
	{
		IDocMngrLink * pDocMngrLink	= i_objects::GetInterface<IDocMngrLink>();
		ASSERT_RETURN_VOID(pDocMngrLink);

		IProjectDocLink * pProject = pDocMngrLink->FindParentProject(strDocPath.c_str());
		if (pProject){
			string_list lst_libs;
			pProject->GetLibs(lst_libs);
			for (size_t i=0; i<lst_libs.items().count(); i++){
				grmcrtlib_exports::lib_data ldata;
				if (ldata.load(lst_libs.items()[i])){
					for (size_t j=0; j<ldata.count();j++)
						m_lstKnownWords.push_back(ldata[j].m_szFuncName);
				}
			}
		}
	}
	//Настройка параметров
	LOG_PROTECT_CODE(Tune());
}
//------------------------------------------------------------------------
void CSourceView::CEditor::BeforeLoad()
{
	//Удаление текста
	m_rMessenger.Perform(SCI_CLEARALL);
	//Отключение Undo/Redo
	m_rMessenger.Perform(SCI_SETUNDOCOLLECTION, false);
}
//------------------------------------------------------------------------
void CSourceView::CEditor::AfterLoad()
{
	//Включение Undo/Redo
	m_rMessenger.Perform(SCI_CANCEL);
	m_rMessenger.Perform(SCI_SETUNDOCOLLECTION, true);
	m_rMessenger.Perform(SCI_EMPTYUNDOBUFFER);
	m_rMessenger.GotoPos(0);
	m_bModified=false;
}
//------------------------------------------------------------------------
void CSourceView::CEditor::BeforeSave()
{
	//Если Undo не надо держать после сохранения - удаляем историю
	if (!singleton_cache::GetStorageLink()->Editor_KeepUndoAfterSave){
		m_rMessenger.Perform(SCI_EMPTYUNDOBUFFER);
	}
}
//------------------------------------------------------------------------
void CSourceView::CEditor::AfterSave()
{
	m_bModified=false;
}
//------------------------------------------------------------------------
void CSourceView::CEditor::OnNotifyMsg(WPARAM wParam, LPARAM lParam)
{
	SCNotification * pNotifyInfo =(SCNotification* )(lParam);
	
	LOG_PROTECT_CODE_BEGIN
	switch (pNotifyInfo->nmhdr.code)
	{
	case SCN_MODIFIED:
		if (pNotifyInfo->modificationType & SC_MOD_CHANGEFOLD)
			FoldUpdate(pNotifyInfo->line,pNotifyInfo->foldLevelNow,pNotifyInfo->foldLevelPrev);
		if (pNotifyInfo->modificationType & (SC_MOD_DELETETEXT |SC_MOD_INSERTTEXT  ))
		{
			m_rImage.Invalidate();//Объявляем образ недействительным
			m_bModified=true;
		}
		break;
	case SCN_MARGINCLICK:
		if (pNotifyInfo->margin == mgnFold) 
			FoldClick(pNotifyInfo->position, pNotifyInfo->modifiers);
	case SCN_UPDATEUI:
		BraceMatch(); break;
	case SCN_CHARADDED:
		CharAdded(static_cast<char>(pNotifyInfo->ch));
		break;
/*case SCN_POSCHANGED:
		coRaiseEvent(NULL,gevnChangeCursorPos,
					 evdChangeCursorPos(m_rOwner,static_cast<size_t>(pNotifyInfo->position)),
					 NULL);
		break;
*/
	}
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::OnCmdMsg(WPARAM wParam, LPARAM lParam)
{
/*	switch (HIWORD(wParam))
	{
//	case SCEN_CHANGE: m_rImage.Invalidate(); break;//Объявляем образ недействительным
	}*/
}
//------------------------------------------------------------------------
//Вызывается при нажатии символа в редакторе
void CSourceView::CEditor::CharAdded(char ch) 
{
	//if (recording)
	//	return;
	long nSelStart= m_rMessenger.GetSelStart();
	long nSelEnd = m_rMessenger.GetSelEnd();

	if (ch<=32){ //Конец набора слова
		long nStart,nEnd;
		string_smart strWord = m_rImage.GetWordAt(m_rMessenger.GetCurPos()-((ch==13) ? 2 :1),nStart,nEnd);
		
		if (!strWord.isempty()){
			IParserLink * pParserIface = i_objects::GetInterface<IParserLink> ();
			if (pParserIface){
				//Выбираем только слова определенного класса
//				if (pParserIface->GetWordClass(strWord.c_str())==IParserLink::wcUserIndentifier){
					//Выкидываем повторения
					most_used_it i = std::remove(m_lstMostUsedWords.begin(),m_lstMostUsedWords.end(),strWord.c_str());
					m_lstMostUsedWords.erase(i,m_lstMostUsedWords.end());
					//Добавляем наше слово
					m_lstMostUsedWords.push_front(strWord); //strWord здесь теряется
					//Если размер кеша разросся, выкидываем часть слов
					while (m_lstMostUsedWords.size()>CASH_MOST_USED_WORDS_SIZE)
						m_lstMostUsedWords.pop_back();
//				}
			}
		}
	}
	//Пользователь набирает слово, попробуем ему подсказать
	else if (__iscsym(ch)){
		long nStart,nEnd;
		string_smart strWord = m_rImage.GetWordAt(m_rMessenger.GetCurPos(),nStart,nEnd);
		if (!strWord.isempty())
			if (!AutoTip(strWord)) //strWord здесь теряется
				m_rMessenger.HideTip();						
	}

	if ((nSelStart != nSelEnd) || (nSelStart <= 0))
		return;
		
	if (m_rMessenger.GetStyleAt(nSelStart-1) == 1)	//?????
		return;

	if (m_rMessenger.Perform(SCI_CALLTIPACTIVE)) 
	{
		/*	if (ch == ')') {
		braceCount--;
		if (braceCount < 1)
		SendEditor(SCI_CALLTIPCANCEL);
		} else if (ch == '(') {
		braceCount++;
		} else {
		ContinueCallTip();
		}*/
	}
	else if (m_rMessenger.Perform(SCI_AUTOCACTIVE)) {
		/*if (ch == '(') {
		braceCount++;
		StartCallTip();
		} else if (ch == ')') {
		braceCount--;
		} else if (!wordCharacters.contains(ch)) {
		SendEditor(SCI_AUTOCCANCEL);
		} else if (autoCCausedByOnlyOne) {
		StartAutoCompleteWord(true);
		}*/
	} 
	else if (ch == '(') {
		//braceCount = 1;
		//StartCallTip();
	} 
	else {
		//autoCCausedByOnlyOne = false;
			AutomaticIndentation(ch);
		//if (autoCompleteStartCharacters.contains(ch)) {
		//	StartAutoComplete();
		//	} else if (props.GetInt("autocompleteword.automatic") && wordCharacters.contains(ch)) {
		//		StartAutoCompleteWord(true);
		//		autoCCausedByOnlyOne = SendEditor(SCI_AUTOCACTIVE);
		//		}
	}
}
//------------------------------------------------------------------------
bool CSourceView::CEditor::AutoComplete()
{
	long nStart,nEnd;
	string_smart strBuffer = m_rImage.GetWordAt(m_rMessenger.GetCurPos(),nStart,nEnd);

	if (!strBuffer.isempty()){
		IParserLink * pParserIface = i_objects::GetInterface<IParserLink> ();
		ASSERT_RETURN_FALSE(pParserIface);

		ITEMLOCATION aLoc(m_rOwner.GetDocument()->GetPath().c_str(),nStart,nEnd);
		string_list  lstItems; 
		string_list	 lstMostUsed; 
		lstMostUsed.items().append(m_lstMostUsedWords.begin(),m_lstMostUsedWords.end());
		lstMostUsed.items().append(m_lstKnownWords.begin(),m_lstKnownWords.end());
			
		//Если парсер что-нибудь нашел, то выводим
		if (pParserIface->SuggestAutoComlete(strBuffer.c_str(),lstMostUsed,aLoc,lstItems)){
			ASSERT(!lstItems.isempty());

			string_smart strWords = lstItems.c_str();
			strWords.replace("\r\n"," ");
			strWords.replace("\n"," ");
			m_rMessenger.Perform(SCI_AUTOCSHOW, strBuffer.length(), strWords.c_str());
			return true;
		}
	}
	
	return false;
}
//------------------------------------------------------------------------
bool CSourceView::CEditor::AutoTip(string_smart & strLeftPart)
{
	most_used_it i = find_if(m_lstMostUsedWords.begin(),m_lstMostUsedWords.end(),part_of(strLeftPart.c_str()));
	string_smart s; 
	if (i!=m_lstMostUsedWords.end())
		s=(*i).c_str();
	else {
		i=find_if(m_lstKnownWords.begin(),m_lstKnownWords.end(),part_of(strLeftPart.c_str()));
		if (i!=m_lstKnownWords.end())
			s=(*i).c_str();
	}

	if (s.isempty())
		return false;


	if ((m_rMessenger.IsTipVisible()) && (m_rMessenger.GetCurTip()!=s))
	{
		m_rMessenger.HideTip();
		m_rMessenger.ShowTip(m_rMessenger.GetCurPos(),s.c_str());
	}
	else 
		m_rMessenger.ShowTip(m_rMessenger.GetCurPos(),s.c_str());

	return true;
}
//------------------------------------------------------------------------
bool CSourceView::CEditor::RangeIsAllWhitespace(long start, long end) 
{
	for (long i = start;i < end;i++) 
		if ((m_rImage[i] != ' ') && (m_rImage[i] != '\t'))
			return false;

	return true;
}
//------------------------------------------------------------------------
size_t CSourceView::CEditor::GetLinePartsInStyle(long line, int style1, int style2, string_smart sv[], size_t len) 
{
	string_smart s;
	size_t  part = 0;
	long thisLineStart = m_rMessenger.GetPosFromLine(line);
	long nextLineStart = m_rMessenger.GetPosFromLine(line + 1);
	
	for (long pos = thisLineStart; (pos < nextLineStart) && (part<len); pos++) {
		if ((m_rMessenger.GetStyleAt(pos) == style1) || (m_rMessenger.GetStyleAt(pos) == style2)) 
			s.cat(m_rImage[pos]);
		else if (!s.isempty()) 
			sv[part++] = s; //здесь s обнуляется
	}
	if (!s.isempty() && (part < len)) 
		sv[part++] = s;

	return part;
}
//------------------------------------------------------------------------
CSourceView::CEditor::IndentationStatus CSourceView::CEditor::GetIndentState(long nLine) 
{
	// C like language indentation defined by braces and keywords
	IndentationStatus indentState = isNone;
	string_smart controlWords[20];
	size_t  parts = GetLinePartsInStyle(nLine, m_StatementIndent.GetStyleNumber(), -1, controlWords, arrlength(controlWords));
	for (size_t  i = 0; i < parts; i++) 
		if (m_StatementIndent.IsIncludes(controlWords[i].c_str())){
			indentState = isKeyWordStart;
			break;
		}
		
	// Braces override keywords
	string_smart controlStrings[20];
	parts = GetLinePartsInStyle(nLine, m_BlockEnd.GetStyleNumber(), -1, controlStrings, arrhigh(controlStrings));
	for (size_t i= 0; i < parts; i++) {
		if (m_BlockEnd.IsIncludes(controlStrings[i].c_str()))
			return isBlockEnd;
		if (m_BlockStart.IsIncludes(controlStrings[i].c_str()))
			return isBlockStart;
	}
	return indentState;
}
//------------------------------------------------------------------------
long CSourceView::CEditor::IndentOfBlock(long nLine) 
{
	const int nStatementLookback = 20;
	if (nLine< 0)return 0;
	
	long nIndentBlock = m_rMessenger.GetLineIndentation(nLine);
	long backLine = nLine;
	IndentationStatus indentState = isNone;
	if (m_StatementIndent.IsEmpty() && m_BlockStart.IsEmpty() && m_BlockEnd.IsEmpty())
		indentState = isBlockStart;	// Don't bother searching backwards

	long lineLimit = max(nLine - nStatementLookback,0);

	while ((backLine >= lineLimit) && (indentState == 0)) {
		indentState = GetIndentState(backLine);
		if (indentState != isNone) {
			nIndentBlock = m_rMessenger.GetLineIndentation(backLine);
			if (indentState == isBlockStart) {
				if (!m_bIndentOpening)
					nIndentBlock += m_nIndentSize;
			}
			if (indentState == isBlockEnd) {
				if (m_bIndentClosing)
					nIndentBlock = max(nIndentBlock-m_nIndentSize,0);
			}
			if ((indentState == isKeyWordStart) && (backLine == nLine))
				nIndentBlock += m_nIndentSize;
		}
		backLine--;
	}
	return nIndentBlock;
}
//------------------------------------------------------------------------
void CSourceView::CEditor::SetLineIndentation(long nLine, long nIndent) 
{
	if (nIndent < 0)return;

	long nSelStart= m_rMessenger.GetSelStart();
	long nSelEnd = m_rMessenger.GetSelEnd();
	long posBefore = m_rMessenger.GetLineIndentPosition(nLine);
	m_rMessenger.SetLineIndentation(nLine, nIndent);
	long posAfter = m_rMessenger.GetLineIndentPosition(nLine);
	
	long posDifference =  posAfter - posBefore;
	if (posAfter > posBefore) {
		// Move selection on
		if (nSelStart >= posBefore) nSelStart += posDifference; 
		if (nSelEnd >= posBefore) nSelEnd += posDifference; 
	} 
	else if (posAfter < posBefore) {
		// Move selection back
		if (nSelStart >= posAfter) {
			if (nSelStart >= posBefore)	nSelStart += posDifference; 
			else nSelStart = posAfter; 
		}
		if (nSelEnd >= posAfter) {
			if (nSelEnd >= posBefore)	nSelEnd += posDifference; 
			else nSelEnd = posAfter; 
		}
	}
	m_rMessenger.SelectRange(nSelStart,nSelEnd);
}
//------------------------------------------------------------------------
void CSourceView::CEditor::AutomaticIndentation(char ch) 
{
	long nSelStart= m_rMessenger.GetSelStart();
//	long nSelEnd = m_rMessenger.GetSelEnd();
	
	long nCurLine = m_rMessenger.GetCurLine();
	long nLineStart = m_rMessenger.GetPosFromLine(nCurLine);
	long nIndentBlock = IndentOfBlock(nCurLine - 1);
	if (m_BlockEnd.IsSingleChar() && (ch == m_BlockEnd.GetSingleChar())) {	
		// Dedent maybe
		if ((!m_bIndentClosing) && (RangeIsAllWhitespace(nLineStart, nSelStart - 1))) 
			m_rMessenger.SetLineIndentation(nCurLine, nIndentBlock - m_nIndentSize);
	} 
	else if (!m_BlockEnd.IsSingleChar() && (ch == ' ')) {	// Dedent maybe
		if (!m_bIndentClosing && (GetIndentState(nCurLine) == isBlockEnd)) {
		}
	} 
	else if ((!m_BlockStart.IsEmpty()) && (ch == m_BlockStart.GetSingleChar())) {	
		// Dedent maybe if first on line and previous line was starting keyword
		if ((!m_bIndentOpening && (GetIndentState(nCurLine - 1) == isKeyWordStart))  && (RangeIsAllWhitespace(nLineStart, nSelStart - 1)))
			SetLineIndentation(nCurLine, nIndentBlock - m_nIndentSize);
	} 
	else if ((ch == '\r' || ch == '\n') && (nSelStart == nLineStart)) {
		// Dedent previous line maybe
		if (!m_bIndentClosing && !m_BlockEnd.IsSingleChar()) {	
			string_smart controlWords[1];
			if ((GetLinePartsInStyle(nCurLine-1, m_BlockEnd.GetStyleNumber(), -1, controlWords, arrhigh(controlWords))) &&
				(m_BlockEnd.IsIncludes(controlWords[0].c_str()))) 
				{
					// Check if first keyword on line is an ender
					SetLineIndentation(nCurLine-1, IndentOfBlock(nCurLine-2) - m_nIndentSize);
					// Recalculate as may have changed previous line
					nIndentBlock = IndentOfBlock(nCurLine - 1);
				}
		}
		SetLineIndentation(nCurLine, nIndentBlock);
	}
}
//------------------------------------------------------------------------
/**
 * Ищет наличие скобок слева или справа от курсора
 * Если скобка найдена, ищется ее пара
 * Возвр. true если обе скобки обнаружены
 **/
bool CSourceView::CEditor::BraceFindMatching(int &braceAtCaret, int &braceOpposite)
{
	braceAtCaret = braceAtCaret= -1;
	
	int  Brackets[3]= {0,0,0};

	long dwFirstLineVisible = m_rMessenger.GetFirstVisibleLine();
	long dwLastLineVisible	= m_rMessenger.GetLinesOnScreen()+dwFirstLineVisible+1;
	long dwFirstVisible 	= m_rMessenger.GetPosFromLine(dwFirstLineVisible);
	long dwLastVisble		= m_rMessenger.GetPosFromLine(dwLastLineVisible);

	if (dwLastVisble<0) dwLastVisble=m_rImage.DetectTextSize();

	m_rImage.SetLookupDirection(CEditorContents::ldRandom);//Поиск будет идти в обр направлении
														  //но половины буфера вполне хватит для поиска в пределах экрана		
	int  i=m_rImage.DetectCaretPos()-1;
	if ((i<dwFirstVisible) || (i>dwLastVisble)) //Курсор за пределами видимой области
		return false;

	// Ищем открывающуюся скобку
	for (; (i>=dwFirstVisible) && !((Brackets[0]==1) || (Brackets[1]==1) || (Brackets[2]==1));i--)
	{
		switch(m_rImage[i])
		{
		case '[': Brackets[0]++;break;
		case ']': Brackets[0]--;break;
		case '{': Brackets[1]++;break;
		case '}': Brackets[1]--;break;
		case '(': Brackets[2]++;break;
		case ')': Brackets[2]--;break;
		}
	}

	//Если скобка найдена, определяем ее пару
	if ((Brackets[0]==1) || (Brackets[1]==1) || (Brackets[2]==1))
	{
		braceAtCaret =i+1;
		int BracketIndex= (Brackets[0]==1) ? 0 :
							(Brackets[1]==1) ? 1:
								(Brackets[2]==1) ? 2: -1;
		ASSERT(BracketIndex!=-1);

		braceOpposite = m_rMessenger.Perform(SCI_BRACEMATCH, braceAtCaret);
	}
		
	return (braceAtCaret!=-1) && (braceOpposite!=-1);
}
//------------------------------------------------------------------------
void CSourceView::CEditor::BraceMatch()
{
	int braceAtCaret = -1,braceOpposite = -1;
	
	BraceFindMatching(braceAtCaret, braceOpposite);

	if ((braceAtCaret != -1) && (braceOpposite == -1)) 
	{
		m_rMessenger.Perform(SCI_BRACEBADLIGHT, braceAtCaret);
		m_rMessenger.Perform(SCI_SETHIGHLIGHTGUIDE);
	} 
	else 
	{
		m_rMessenger.Perform(SCI_BRACEHIGHLIGHT, braceAtCaret, braceOpposite);
	}
}
//------------------------------------------------------------------------
void CSourceView::CEditor::FoldExpand(long &dwLine, bool doExpand, bool Force, long dwVisLevels, long dwLevel)
{
	int dwLineMaxSubord = m_rMessenger.Perform(SCI_GETLASTCHILD, dwLine, dwLevel & SC_FOLDLEVELNUMBERMASK);
	dwLine++;
	while (dwLine <= dwLineMaxSubord) 
	{
		if ((Force) || (doExpand))
			m_rMessenger.Perform((dwVisLevels>0)||doExpand ?SCI_SHOWLINES :SCI_HIDELINES,dwLine, dwLine);

		int levelLine = dwLevel;
		if (levelLine == -1)
			levelLine = m_rMessenger.Perform(SCI_GETFOLDLEVEL, dwLine);
		if (levelLine & SC_FOLDLEVELHEADERFLAG) 
		{
			if (Force)
				m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, dwVisLevels > 1);
			else if ((doExpand) && (!m_rMessenger.Perform(SCI_GETFOLDEXPANDED, dwLine)))
				m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, 1);
			FoldExpand(dwLine, doExpand, Force, dwVisLevels - 1);
		} 
		else 
			dwLine++;
	}
}
//------------------------------------------------------------------------
void CSourceView::CEditor::FoldUpdate(long dwLine, long dwLevelNow, long dwLevelPrev)
{
	if (dwLevelNow & SC_FOLDLEVELHEADERFLAG) 
	{
		if (!(dwLevelPrev & SC_FOLDLEVELHEADERFLAG)) 
			m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, 1);
	} 
	else if (dwLevelPrev & SC_FOLDLEVELHEADERFLAG) 
	{
		if (!m_rMessenger.Perform(SCI_GETFOLDEXPANDED, dwLine)) 
			FoldExpand(dwLine, true, false, 0, dwLevelPrev);
			// Removing the fold from one that has been contracted so should expand
			// otherwise lines are left invisible with no way to make them visible
			
	}
}
//------------------------------------------------------------------------
bool CSourceView::CEditor::IsAllExpanded()
{
	int maxLine = m_rMessenger.GetLineCount();
	bool bExpanded = true;
	for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) 
	{
		if (m_rMessenger.Perform(SCI_GETFOLDLEVEL, lineSeek) & SC_FOLDLEVELHEADERFLAG) {
			bExpanded = (m_rMessenger.Perform(SCI_GETFOLDEXPANDED, lineSeek)!=0);
			break;
		}
	}
	return bExpanded;
}
//------------------------------------------------------------------------
void CSourceView::CEditor::ExpandAll(bool bExpand)
{
	m_rMessenger.Perform(SCI_COLOURISE, 0, -1);
	int maxLine = m_rMessenger.GetLineCount();

	for (long line = 0; line < maxLine; line++) 
	{
		int dwLevel = m_rMessenger.Perform(SCI_GETFOLDLEVEL, line);
		if ((dwLevel & SC_FOLDLEVELHEADERFLAG) &&(SC_FOLDLEVELBASE == (dwLevel & SC_FOLDLEVELNUMBERMASK))) 
		{
			m_rMessenger.Perform(SCI_SETFOLDEXPANDED, line, bExpand);
			if (bExpand) 
			{
				FoldExpand(line, true, false, 0, dwLevel);
				line--;
			} 
			else 
			{
				int dwLineMaxSubord = m_rMessenger.Perform(SCI_GETLASTCHILD, line, -1);
				m_rMessenger.Perform(SCI_SETFOLDEXPANDED, line);
				if (dwLineMaxSubord > line)
					m_rMessenger.Perform(SCI_HIDELINES, line + 1, dwLineMaxSubord);
			}
		}
	}
}
//------------------------------------------------------------------------
bool CSourceView::CEditor::IsCurExpanded()
{
	long dwLine=m_rMessenger.GetCurLine();
	long dwLevel=m_rMessenger.Perform(SCI_GETFOLDLEVEL, dwLine);
	if (!(dwLevel & SC_FOLDLEVELHEADERFLAG))
		dwLine=m_rMessenger.Perform(SCI_GETFOLDPARENT,dwLine);
	return m_rMessenger.Perform(SCI_GETFOLDEXPANDED, dwLine)!=0;
}
//------------------------------------------------------------------------
void CSourceView::CEditor::ExpandCurrent(bool bExpand)
{
	long dwLine=m_rMessenger.GetCurLine();
	long dwLevel=m_rMessenger.Perform(SCI_GETFOLDLEVEL, dwLine);

	if (dwLevel & (~SC_FOLDLEVELBASE))
	{
		if (!(dwLevel & SC_FOLDLEVELHEADERFLAG))
			dwLine=m_rMessenger.Perform(SCI_GETFOLDPARENT,dwLine);
		m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, bExpand);
		long dwLineMaxSubord = m_rMessenger.Perform(SCI_GETLASTCHILD, dwLine, -1);
		if (dwLineMaxSubord > dwLine)
			m_rMessenger.Perform(bExpand ? SCI_SHOWLINES:SCI_HIDELINES, dwLine + 1, dwLineMaxSubord);
	}
}
//------------------------------------------------------------------------
void CSourceView::CEditor::FoldClick(long dwPosition, long dwModifiers)
{
	long dwLine = m_rMessenger.GetLineFromPos(dwPosition);
	if ((dwModifiers & SCMOD_SHIFT) && (dwModifiers & SCMOD_CTRL)) 
		ExpandAll(!IsAllExpanded());
	else 
	{
		int levelClick = m_rMessenger.Perform(SCI_GETFOLDLEVEL, dwLine);
		if (levelClick & SC_FOLDLEVELHEADERFLAG) 
		{
			if (dwModifiers & SCMOD_SHIFT) 
			{
				// Ensure all children visible
				m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, 1);
				FoldExpand(dwLine, true, true, 100, levelClick);
			} 
			else if (dwModifiers & SCMOD_CTRL) 
			{
				if (m_rMessenger.Perform(SCI_GETFOLDEXPANDED, dwLine)) 
				{	// Contract this line and all children
					m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, long(false));
					FoldExpand(dwLine, false, true, 0, levelClick);
				} 
				else  
				{	// Expand this line and all children
					m_rMessenger.Perform(SCI_SETFOLDEXPANDED, dwLine, 1);
					FoldExpand(dwLine, true, true, 100, levelClick);
				}
			} 
			else 
			{	// Toggle this line
				m_rMessenger.Perform(SCI_TOGGLEFOLD, dwLine);
			}
		}
	}	
}
//------------------------------------------------------------------------
void CSourceView::CEditor::FormatSelection()
{
	if (!m_rMessenger.IsSelected()) return;

	str		pText,pText2;
	DWORD	Level=0;
	DWORD   BLevel=0;
	DWORD	bEndbleAddingSpace=false;
	DWORD	EQPos=0;
	DWORD   AddingSpaceType=0;
	CList   <DWORD,DWORD> aBPos;
	bool	DQuote=false;
	bool	SQuote=false;
	
	LOG_PROTECT_CODE_BEGIN
		
	long dwStart	= m_rMessenger.GetSelStart();
	long dwEnd		= m_rMessenger.GetSelEnd();
	string_smart strSrc=m_rMessenger.GetSelText();
	string_smart strDst; strDst.reserve((dwEnd-dwStart)*2);

	for (Level=0,pText=strSrc.buffer(),pText2=strDst.buffer();*pText;pText++,*pText2++)
	{
		for (;(*pText==' ') || (*pText=='\t');pText++);


		memset(pText2,'\t',Level);
		pText2+=Level;
		if ((bEndbleAddingSpace ) && (AddingSpaceType==1))
		{
			memset(pText2,' ',EQPos);
			pText2+=EQPos;
		}
		else if (BLevel>0)
		{
			memset(pText2,' ',aBPos.GetTail());
			pText2+=aBPos.GetTail();
		}

		LPTSTR ppStart=pText2;
		
		for (;(*pText!='\r') && (*pText!=0);pText++,pText2++)
		{
			*pText2=*pText;
			switch  (*pText)
			{
			case '\'':
				SQuote=!SQuote;
				break;
			case '\"':
				if ((DQuote) && (*(pText-1)=='\\'))
					;
				else
					DQuote=!DQuote;
				break;
			case '=':
				if ((!SQuote) && (!DQuote))
				{
					EQPos=pText2-ppStart;
					AddingSpaceType=1;
				}
				break;
			case '{':
				if ((!SQuote) && (!DQuote))
					Level++;
				break;
			case '}':
				if ((!SQuote) && (!DQuote))
					if (Level>0)
					{
						Level--;
						if (*(pText2-1)=='\t')
							*(--pText2)='}';
					}
				break;
			case '(':
				if ((!SQuote) && (!DQuote))
				{
					aBPos.AddTail(pText2-ppStart);
					BLevel++;
					AddingSpaceType=2;
				}

				break;
			case ')':
				if ((!SQuote) && (!DQuote))
					if (BLevel>0)
					{
						aBPos.RemoveTail();
						BLevel--;
						AddingSpaceType=2;
					}
				break;
			}
		}

		for (pText2--;(*pText2==' ') ||(*pText2=='\t');) // trim right
			pText2--;

		if (*pText2==',') 
			bEndbleAddingSpace=true;
		else
			bEndbleAddingSpace=false;
		
		*(++pText2)='\r'; *(++pText2)='\n';
		if (*pText==0) break;
		++pText;
		if (*pText==0) break;
	}
	*pText2=0;
	strDst.invalidate();
	m_rMessenger.ReplaceSelection(strSrc.c_str());

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::TabifySelection(bool bTabify)
{
	LOG_PROTECT_CODE_BEGIN
		
	string_smart strSrc=m_rMessenger.GetSelText();
	
	if (bTabify)
		strSrc.replace(strnchar(' ',m_rMessenger.Perform(SCI_GETTABWIDTH)).c_str(),"\t");
	else
		strSrc.replace("\t",strnchar(' ',m_rMessenger.Perform(SCI_GETTABWIDTH)).c_str());
	
	m_rMessenger.ReplaceSelection(strSrc.c_str());

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::Uppercase(bool bUpper)
{
	LOG_PROTECT_CODE_BEGIN
		
	if (!m_rMessenger.IsSelected())
		m_rMessenger.SelectRange(m_rMessenger.GetCurPos(),m_rMessenger.GetCurPos()+1);

	string_smart strSrc =  m_rMessenger.GetSelText();
	m_rMessenger.ReplaceSelection((bUpper) 
									? strSrc.toupper().c_str() 
									: strSrc.tolower().c_str());

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::IncreaseLineIndent(bool bIncrease)
{
	LOG_PROTECT_CODE_BEGIN
		
	long dwSelStart = m_rMessenger.GetSelStart();
	long dwSelEnd   = m_rMessenger.GetSelEnd();

	long dwOffsStart = m_rMessenger.OffsetToStartLinePos(dwSelStart);
	long dwOffsEnd   = m_rMessenger.OffsetToEndLinePos(dwSelEnd)-1;

	//Подправляем выделение	к границам строк
	m_rMessenger.SelectRange(dwOffsStart,dwOffsEnd);
							 

	string_list  strSrc =  m_rMessenger.GetSelText();
	string_smart strSpaces=strnchar(' ',m_rMessenger.Perform(SCI_GETTABWIDTH));
	
	if (bIncrease)
		dwSelStart++;
		
	for (size_t i=0; i<strSrc.items().count();i++){
		string_smart & rstrLine=strSrc.items().get_at(i);
		//-- Increase  --
		if (bIncrease)
		{
			rstrLine.insert('\t',0);
			dwSelEnd++;
		}
		//-- Decrease  --
		else if (!rstrLine.isempty())
		{
			if (rstrLine[0]=='\t')
			{
				rstrLine.del(0);
				if ((i==0) && (dwOffsStart<dwSelStart))
					dwSelStart--;
				dwSelEnd--;
			}
			else if (rstrLine.pos(strSpaces.c_str())==0)
			{
				rstrLine.del(0,strSpaces.length());
				dwSelEnd-=strSpaces.length();
				if (i==0)
					dwSelStart-=strSpaces.length();
			}
		}
	}
	strSrc.update_text();

	m_rMessenger.ReplaceSelection(strSrc.c_str());
	m_rMessenger.SelectRange(dwSelStart,dwSelEnd);
	
	LOG_PROTECT_CODE_END
}
