/********************************************************************
    created:    2002/04/26
    created:    10:3:2002   1:13
    filename:   $(Projects)\Sources\MTDEv\Bars\OutputBar.h
    file path:  $(Projects)\Sources\MTDEv\Bars

    file base:  OutputDlg
    file ext:   h
    author:     Fadeyev R.V.
    
    purpose:    Панель, содержащая окно сообщений компилятора, окно вывода результатов
*********************************************************************/

#if !defined(AFX_OUTPUTBAR_H__708257AC_7B83_11D3_A74B_0090274409AC__INCLUDED_)
#define AFX_OUTPUTBAR_H__708257AC_7B83_11D3_A74B_0090274409AC__INCLUDED_

#pragma once
#include "CObjectEx.h" //Один из родителей
#include "ExtControls.h" //Один из родителей
#include "Bar_B.h" //Один из родителей


/////////////////////////////////////////////////////////////////////////////
// CbarOutput window
class CbarOutput :	public Cbar_B,
					public IDocumentLink::IEventSink
{
public:
	//-- class -----------------------------------------------------------
	//Базовый класс вывода сообщений на основе ListBox
	class COutputList_B: public CListBox, public CCObjectEx, public IOutputLink
	{
	private:
		static CFont	m_Font;
		Cbar_B &		m_Owner;
		TCHAR			m_cLastSymbol;
	protected:
		//Поиск 1. Вызывается из  ExtractPosition
		bool ExtractPositionMthd1(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos);
		//Поиск 2. Вызывается из  ExtractPosition
		bool ExtractPositionMthd2(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos);	
		//Поиск 3. Вызывается из  ExtractPosition
		bool ExtractPositionMthd3(IN cstr szMessage, OUT string_smart & strFileName, OUT long & nLine, long &nPos);	
		
		//Из указанной строки сообщения компилятора пытается выделить имя файла и номер строки
		bool ExtractPosition(IN cstr szMessage , OUT string_smart & strFileName, OUT long & nLine, long &nPos);
		//Открывается указанный файл и в нем устанавливается курсор на указанную позицию
		bool LocateToPos(cstr strFileName, long nLine, long nPos);
	
		//Создание главного окна
		override void OnCreateAppMainWnd()	{
			i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}

		//Получение заголовка
		virtual cstr GetTitle()=0;
		
	public:
		COutputList_B(Cbar_B & Owner):m_Owner(Owner){};
		//---------------------------------------------------------------------
		//Интерфейс к нашему объекту
		override void StartSession(bool bClear=true);
		override void StopSession(bool bClear=false);
		override void Print(cstr szText);
		override string_smart GetText();			
		override void Clear()				{ResetContent();}

		DECLARE_MESSAGE_MAP()
		afx_msg void msgOnActivate();		//Обработчик события, активизации		
		afx_msg LRESULT msgOnRealign(WPARAM wParam, LPARAM lParam);		//Обработчик события изменения размеров
		afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void cmdupOnEditCopy(CCmdUI *pCmdUI);
		afx_msg void cmdOnEditCopy();
		afx_msg void cmdOnEditClear();
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	};
	
	//-- class -----------------------------------------------------------
	//Вывод сообщений для компилятора
	class COutputBuild: public COutputList_B{
	public:
		//Constructor/Destructor
		COutputBuild(Cbar_B & Owner):COutputList_B(Owner){};
		override ~COutputBuild(){};
		override cstr GetTitle()	{return _T("Build");}

		//Multiple Instance Link
		override size_t GetObjectID()const	{return IOutputLink::idOutputBuild;}
	protected:	
		DECLARE_MESSAGE_MAP()
		afx_msg void cmdOnNextError();
		afx_msg void cmdOnPrevError();
		afx_msg void cmdupOnNextError(CCmdUI *pCmdUI);
		afx_msg void cmdupOnPrevError(CCmdUI *pCmdUI);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	};

	//-- class -----------------------------------------------------------
	//Вывод сообщений FindInFiles
	class COutputFindInFiles: public COutputList_B{
	public:
		//Constructor/Destructor
		COutputFindInFiles(Cbar_B & Owner):COutputList_B(Owner){};
		override ~COutputFindInFiles(){};
		override cstr GetTitle()	{return _T("Find in Files");}	
		//Multiple Instance Link
		override size_t GetObjectID()const	{return IOutputLink::idOutputFindInFiles;}
	protected:	
		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	};
	
	//Вывод сообщений парсинга
	class COutputParse: public COutputList_B{
	public:
		COutputParse(Cbar_B & Owner):COutputList_B(Owner){};
		override ~COutputParse(){};
		override cstr GetTitle()	{return _T("Parse Debug");}	

		//Multiple Instance Link
		override size_t GetObjectID()const	{return IOutputLink::idOutputParse;}
	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	};
protected:
	CBCGTabWnd	m_wndTabs;
	COutputBuild		m_wndOutputBuild;
	COutputParse		m_wndOutputParse;	
	COutputFindInFiles	m_wndOutputFindInFiles;

	//--------------------------------------------------------------------
	//Перекрытие методов абстрактного базового класса
	override cstr	GetBarName(){return barnOutput;};
	override DWORD	GetBarID()	{return ID_VIEW_OUTPUT;};
	override DWORD	GetBarDock(){return CBRS_ALIGN_BOTTOM;};
	override CBCGTabWnd* GetTabWnd(){return &m_wndTabs;}
	//--------------------------------------------------------------------
	//Создание главного окна
	override void OnCreateAppMainWnd()		{i_objects::GetInterface<IMainFrameLink>()->IncludeToCmdMsgMap(this);}
	//from IDocumentLink::IEventSink
	override void IDocumentLink::IEventSink::evOnCloseDocument(evdCloseDocumentInfo &);
	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &) {};
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &) {};

	//Multiple Instance Link
	override size_t GetObjectID()const	{return IBarLink::idBarOutput;}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT msgOnRealign(WPARAM wParam, LPARAM lParam);		//Обработчик события изменения размеров
	afx_msg void cmdOnViewOutput();
	afx_msg void cmdupOnViewOutput(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	CbarOutput();
	virtual ~CbarOutput();
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTBAR_H__708257AC_7B83_11D3_A74B_0090274409AC__INCLUDED_)
