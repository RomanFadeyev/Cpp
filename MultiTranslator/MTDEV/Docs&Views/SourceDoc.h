/********************************************************************
	created:	2002/02/23
	created:	23:2:2002	13:00
	filename:	$(Projects)\Sources\MTDEv\Docs&Views\SourceDoc.h
	file path:	$(Projects)\Sources\MTDEv\Docs&Views

	file base:	SourceDoc
	file ext:	h
	author:		Fadeyev	R.V.
	
	purpose:	Реализация документа исходного файла (SourceFile)
*********************************************************************/
#pragma	once

#include <MiscUtils.h>

#include "BaseStructs.h"
#include "Names.h"
#include "CObjectEx.h"

#include "COMSourceDocument.h"	//COM

class CSourceDoc : public CDocumentEx,public CCObjectEx, public ISourceDocLink
{
protected: // create from serialization	only
	COM::SourceDocument* m_pCoClass;	//Это - СоClass для представления документа в COM'e
	friend class COM::AbstractDocument;

	//Признак выполнения очередной операции
	enum op_state  {opNone,	opOpening, opSaving};

	//CEditorViews	m_Editors;	   
	op_state		m_nOpState;
	clock_t			m_tLastModify;
	CRect			m_PageMargins;
	misc_utils::file_watch m_WatchDog;	
	
	void StartWatchDog();
	void StopWatchDog();

	CSourceDoc();
	DECLARE_DYNCREATE(CSourceDoc)

	override void Serialize(CArchive& ar);
	override BOOL OnNewDocument();
	override BOOL OnOpenDocument(LPCTSTR lpszPathName);
	override void OnCloseDocument();
	override BOOL OnSaveDocument(LPCTSTR lpszPathName);

	override BOOL SaveModified();
	override void SetModifiedFlag(BOOL bModified = TRUE);

	//Multiple Instance Link
	override size_t GetObjectID()const	{return CDocumentEx::GetObjectID();}

	//From CCObjectEx
	override void OnStorageDataChanged(IStorageLink::EParam nParameterChanged);

	void OnChangeDocumentOutside(misc_utils::file_watch &,misc_utils::file_watch::change_type);
	CView *	GetView();

	DECLARE_MESSAGE_MAP()
	afx_msg	void cmdOnCompile();
	afx_msg	void cmdupOnCompile(CCmdUI *pCmdUI);
	afx_msg	void cmdupOnFileSave(CCmdUI *pCmdUI);
public:
	CRect GetPageMargins()	{return m_PageMargins;}
	//--------------------------------------------------------------------
	//Интерфейс	ISourceDocLink нашего документа	проекта	с другими объектами
	override CDocumentEx* GetObject()				{return	this;}	 //Указатель на	документ
	override IDocumentLink*	GetDefaultInterface()	{return	this;}	 //Указатель на	интерфейс
	override ref_cnt_ptr<COM::IDocument> GetComInterface(){return GetComInterface2().get();}
	override ref_cnt_ptr<COM::ISourceDocument> GetComInterface2();

	override string_smart GetPath() const;
	override string_smart GetExt() const;
	override string_smart GetName() const;
	override bool IsReadOnly() const;
	
	override bool Close();
	override bool Save();
	override bool SaveAs(cstr szNewFileName);	//Сохраняет документ

	override clock_t GetTimeOfLastChange();
	override ISourceViewLink* GetViewLink();
	override IDebugManageLink* GetDebugManageLink()	{return	GetViewLink()->GetDebugManageLink();}
	//--------------------------------------------------------------------
	override ~CSourceDoc();

#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext&	dc)	const;
#endif

};

