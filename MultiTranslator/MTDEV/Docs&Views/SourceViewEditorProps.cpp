/********************************************************************
	created:	2002/09/27
	created:	27:9:2002   9:07
	filename: 	$(Projects)\sources\mtdev\docs&views\sourcevieweditorprops.cpp
	file path:	$(Projects)\sources\mtdev\docs&views
	file base:	sourcevieweditorprops
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	����� ���������� CSourceView::CEditor, ����������
				������/���������� �������� ��������� � ��� ������
*********************************************************************/

#include "stdafx.h"
#include "SourceView.h"
#include "grmcExports.h"

//{{
static struct{
	long dwMarker;
	long dwType;	
	long dwFore;
	long dwBack;
}FoldMarkers[][7]={ //���� �������� ������������. ����� 4 ���� ��������
	{{SC_MARKNUM_FOLDEROPEN,	SC_MARK_ARROWDOWN,clBlack,clBlack},
	{SC_MARKNUM_FOLDER, 		SC_MARK_ARROW,	clBlack,clBlack},
	{SC_MARKNUM_FOLDERSUB,		SC_MARK_EMPTY,	clBlack,clBlack},
	{SC_MARKNUM_FOLDERTAIL, 	SC_MARK_EMPTY,	clBlack,clBlack},
	{SC_MARKNUM_FOLDEREND,		SC_MARK_EMPTY,	clWhite,clBlack},
	{SC_MARKNUM_FOLDEROPENMID,	SC_MARK_EMPTY,	clWhite,clBlack},
	{SC_MARKNUM_FOLDERMIDTAIL,	SC_MARK_EMPTY,	clWhite,clBlack}},

	{{SC_MARKNUM_FOLDEROPEN,	SC_MARK_MINUS,	clWhite, clBlack},
	{SC_MARKNUM_FOLDER, 		SC_MARK_PLUS,	clWhite, clBlack},
	{SC_MARKNUM_FOLDERSUB,		SC_MARK_EMPTY,	clWhite, clBlack},
	{SC_MARKNUM_FOLDERTAIL, 	SC_MARK_EMPTY,	clWhite, clBlack},
	{SC_MARKNUM_FOLDEREND,		SC_MARK_EMPTY,	clWhite, clBlack},
	{SC_MARKNUM_FOLDEROPENMID,	SC_MARK_EMPTY,	clWhite, clBlack},
	{SC_MARKNUM_FOLDERMIDTAIL,	SC_MARK_EMPTY,	clWhite, clBlack}},

	{{SC_MARKNUM_FOLDEROPEN,	SC_MARK_CIRCLEMINUS,clWhite, clDkGray},
	{SC_MARKNUM_FOLDER, 		SC_MARK_CIRCLEPLUS, clWhite, clDkGray},
	{SC_MARKNUM_FOLDERSUB,		SC_MARK_VLINE,		clWhite, clDkGray},
	{SC_MARKNUM_FOLDERTAIL, 	SC_MARK_LCORNERCURVE,		clWhite, clDkGray},
	{SC_MARKNUM_FOLDEREND,		SC_MARK_CIRCLEPLUSCONNECTED,clWhite, clDkGray},
	{SC_MARKNUM_FOLDEROPENMID,	SC_MARK_CIRCLEMINUSCONNECTED, clWhite, clDkGray},
	{SC_MARKNUM_FOLDERMIDTAIL,	SC_MARK_TCORNERCURVE,clWhite, clDkGray}},

	{{SC_MARKNUM_FOLDEROPEN,	SC_MARK_BOXMINUS,	clWhite, clGray},
	{SC_MARKNUM_FOLDER, 		SC_MARK_BOXPLUS,	clWhite, clGray},
	{SC_MARKNUM_FOLDERSUB,		SC_MARK_VLINE,		clWhite, clGray},
	{SC_MARKNUM_FOLDERTAIL, 	SC_MARK_LCORNER,	clWhite, clGray},
	{SC_MARKNUM_FOLDEREND,		SC_MARK_BOXPLUSCONNECTED, clWhite, clGray},
	{SC_MARKNUM_FOLDEROPENMID,	SC_MARK_BOXMINUSCONNECTED, clWhite, clGray},
	{SC_MARKNUM_FOLDERMIDTAIL,	SC_MARK_TCORNER,	clWhite, clGray}}
};
//}}


/////////////////////////////////////////////////////////////////////////////
// style_definition
/////////////////////////////////////////////////////////////////////////////

//��������� ���� ��������
#define SetMarkerView(Id,Type,Fore,Back) {\
	m_rMessenger.Perform(SCI_MARKERDEFINE, Id,(LPARAM)Type);\
	m_rMessenger.Perform(SCI_MARKERSETFORE,Id,(LPARAM)Fore);\
	m_rMessenger.Perform(SCI_MARKERSETBACK,Id,(LPARAM)Back);}

/////////////////////////////////////////////////////////////////////////////
// CSourceView::CEditor
/////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
void CSourceView::CEditor::SetStyles()
{
	LOG_PROTECT_CODE_BEGIN

	_ASSERT(m_pDocStyleDescriptor);
	mtdev_lib::IDocumentColorChema * pColorChema = m_pDocStyleDescriptor->ColorChema;

	ISourceDocStylerLink::style_definition aStyle; 

	//Initially set the default style
	m_rMessenger.Perform(SCI_STYLERESETDEFAULT);
	m_pDocStyleDescriptor->GetStyleDescByID(STYLE_DEFAULT,aStyle);
	m_rMessenger.SetStyle(aStyle);
	//Then copies global style to all others
	m_rMessenger.Perform(SCI_STYLECLEARALL);

	//Then set remained styles
	for (int i=0;i<pColorChema->ItemCount;++i)
	{
		m_pDocStyleDescriptor->GetStyleDescByIndex(i,aStyle);
		
		//������ �����, ����������� ���� - ��� Fold Margin
		if (aStyle.m_nStyleID==255){
			m_rMessenger.SetFoldMarginBackColor(true,color_to_rgb(aStyle.m_clBack));
			m_rMessenger.SetFoldMarginForeColor(true,color_to_rgb(aStyle.m_clFore));
		}
		else if (aStyle.m_nStyleID == STYLE_DEFAULT){
		}
		else{
			m_rMessenger.SetStyle(aStyle);
		}
	}

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::SetMargins()
{
	LOG_PROTECT_CODE_BEGIN

	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);

	// ���� ��� �������
	m_rMessenger.SetMarginType(mgnLines,SC_MARGIN_NUMBER);
	m_rMessenger.SetMarginWidth(mgnLines, pStorage->Editor_ShowNumberMargin ? 40 : (int)0);

	// ���� ��� �������
	m_rMessenger.SetMarginType(mgnBase,SC_MARGIN_SYMBOL);
	m_rMessenger.SetMarginWidth(mgnBase, pStorage->Editor_ShowSelectionMargin ? 20 :(int)0);

	// ���� ��� ������
	m_rMessenger.SetMarginType(mgnFold , (int)SC_MARGIN_SYMBOL);
	m_rMessenger.Perform(SCI_SETMARGINMASKN, mgnFold, SC_MASK_FOLDERS);
	m_rMessenger.SetMarginMouseSensitive(mgnFold , 1);
	m_rMessenger.SetMarginWidth(mgnFold, pStorage->Editor_ShowFoldMargin ? 14 :(int)0);
	//������� ����� ������������
	//16 - ����� ����� �����������, �����
	m_rMessenger.Perform(SCI_SETFOLDFLAGS, true ? SC_FOLDFLAG_LINEAFTER_CONTRACTED : 0);//�� ��������!!!

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::SetViewSettins()
{
	LOG_PROTECT_CODE_BEGIN

	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);

	//���������� ENDL
	m_rMessenger.Perform(SCI_SETEOLMODE, SC_EOL_CRLF);	//�� ������, ����� ��� AutoIndention
	//���������/����������� ENDL
	m_rMessenger.Perform(SCI_SETVIEWEOL, (BOOL)false);						
	//��� ���������� ������ �������� <32
	m_rMessenger.SetControlCharSymbol(149/*'�'*/);

	//���������/����������� WhiteSpace
	m_rMessenger.SetViewWhiteSpace(pStorage->Editor_ShowWhiteSpaces!=FALSE);

	//���������/����������� ������������ ����� 
	m_rMessenger.SetIndentationGuides(pStorage->Editor_ShowIndentGuides!=FALSE);

	//���������/����������� ��������
	m_rMessenger.SetHScrollBar(pStorage->Editor_ShowHorzSB!=FALSE);
	m_rMessenger.SetVScrollBar(pStorage->Editor_ShowVertSB!=FALSE);

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::SetDrawSettings()
{
	LOG_PROTECT_CODE_BEGIN

	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);

	if (pStorage->Editor_ShowRightMargin){
		m_rMessenger.Perform(SCI_SETEDGEMODE,EDGE_LINE);
		m_rMessenger.Perform(SCI_SETEDGECOLUMN,pStorage->Editor_ShowRightMarginAt);
	}
	else{
		m_rMessenger.Perform(SCI_SETEDGEMODE,EDGE_NONE);
		m_rMessenger.Perform(SCI_SETEDGECOLUMN);
	}

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::SetReadonlySettings()
{
	LOG_PROTECT_CODE_BEGIN
	
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ASSERT_RETURN_VOID(pStorage);
	
	bool bReadOnly = ((m_rOwner.GetDocumentLink()->IsReadOnly()) &&  //ReadOnly
					  (pStorage->General_AllowEditReadOnlyFiles==FALSE)); //� �� ��������� �������������

	m_rMessenger.SetReadOnly(bReadOnly);

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceView::CEditor::Tune()
{
	IStorageLink * pStorage = singleton_cache::GetStorageLink();
	ISourceDocStylerLink * pDocStyler = GetDocStyler();
	_ASSERT(m_pDocStyleDescriptor);

	//��������� �������
	SetMargins();

	//��������� ������ ������� ����� ����������� white space � ����
	SetViewSettins();

	//��������� ��������� ����������
	SetDrawSettings();

	//�������������� ReadOnlye
	SetReadonlySettings();

	if (m_pDocStyleDescriptor)
	{
		//???
		m_rMessenger.Perform(SCI_SETTABINDENTS, false);
		m_rMessenger.Perform(SCI_SETBACKSPACEUNINDENTS, false);

		//������������� TAB
		m_rMessenger.SetUseTabs(m_pDocStyleDescriptor->GetUseTabs()!=FALSE);

		//������ ���������
		m_rMessenger.SetTabWidth(m_pDocStyleDescriptor->GetTabSize());

		//Wrap mode
		m_rMessenger.SetWordWrap(m_pDocStyleDescriptor->GetWordWrap()!=FALSE);
		if (m_pDocStyleDescriptor->GetWordWrap())
			m_rMessenger.Perform(SCI_SETLAYOUTCACHE,SC_CACHE_PAGE);

		//������ indent
		//���������� ����� ���� ��� �������� � m_nIndentSize - ��� ����� ��� �������� �������
		m_rMessenger.SetIndent(m_nIndentSize = m_pDocStyleDescriptor->GetIndentSize());

		m_bIndentOpening = m_pDocStyleDescriptor->GetIndentOpening();
		m_bIndentClosing = m_pDocStyleDescriptor->GetIndentClosing();

		//���������/���������� ������ ������������
		m_rMessenger.SetProperty("fold",m_pDocStyleDescriptor->FoldCode!=FALSE);
		m_rMessenger.SetProperty("fold.comment",m_pDocStyleDescriptor->FoldComment!=FALSE);
	
		//  -- �������� ���� --
		m_rMessenger.SetLexerLanguage(m_pDocStyleDescriptor->Lexer);
		int	LexLanguage	= m_rMessenger.GetLexer();

		//���������	����������������� �����. ��	����� ���� ��������� �����
		for	(int i=0;i<KEYWORDSET_MAX;++i)
			if (i!=3)
				m_rMessenger.SetKeyWords(i, string_smart(m_pDocStyleDescriptor->GetKeywords(i)).c_str());

		//������������� ������������� ����� ������� �� ����������� ���������
		string_smart lst_words;
		for (known_it i=m_lstKnownWords.begin(); i!=m_lstKnownWords.end(); i++)
			lst_words.cat((*i).c_str()).cat(" ");
		lst_words.cat(m_pDocStyleDescriptor->GetKeywords(3));
		m_rMessenger.SetKeyWords(3, lst_words.trim().c_str());
		
		// -- ������������� �����	 --
		// ��������� ����� �� ���������. These attributes are used wherever	no explicit	choices	are	made.
		m_rMessenger.SetFoldMarginBackColor(false,0);
		m_rMessenger.SetFoldMarginForeColor(false,0);
		SetStyles();
	  
		//-- ��������� indentation --
		m_pDocStyleDescriptor->GetStatementIndent(m_StatementIndent);
		m_pDocStyleDescriptor->GetStatementEnd(m_StatementEnd);
		m_pDocStyleDescriptor->GetBlockStart(m_BlockStart);
		m_pDocStyleDescriptor->GetBlockEnd(m_BlockEnd);
	}
		
	//��� �������� ������������
	for (int i=0;i<7;i++) {
		int nMarkerType = 3; // ����� ���� 0,1,2,3
		SetMarkerView(FoldMarkers[nMarkerType][i].dwMarker,
					  FoldMarkers[nMarkerType][i].dwType,
					  FoldMarkers[nMarkerType][i].dwFore,
					  FoldMarkers[nMarkerType][i].dwBack);
	}

	//���� ����������� ������
	m_rMessenger.SetSelFore(true,RGB(255,255,255));							//�� ��������!!!
	m_rMessenger.SetSelBack(true,RGB(0,0,0));								//�� ��������!!!

	//��� �������� ��������. ����� �� �������������, � ������� �� �������� ������������
	//��� �������� ����� �������� . ����� �� �������������, � ������� �� �������� ������������
	SetMarkerView(MARKER_BOOKMARK,SC_MARK_ROUNDRECT,RGB(0, 0, 0x7f),RGB(0x80, 0xff, 0xff));
	SetMarkerView(MARKER_BREAKPOINT,SC_MARK_SHORTARROW,RGB(0, 0, 0x7f),RGB(0xff, 0, 0));	
	SetMarkerView(MARKER_DISABLE_BREAKPOINT,SC_MARK_SHORTARROW,RGB(0xAF,0,0),RGB(0x7f, 0x7f, 0x7f));	
	SetMarkerView(MARKER_COMMAND_POINTER,SC_MARK_ARROW,RGB(0,0,0x7f),RGB(0, 0xff, 0));	

	//��������� autocompletion
	m_rMessenger.Perform(SCI_AUTOCSETIGNORECASE, grmc_exports::bCaseInsensivity);
	m_rMessenger.Perform(SCI_AUTOCSETCHOOSESINGLE, false);
	m_rMessenger.Perform(SCI_AUTOCSETCANCELATSTART, false);
}
//------------------------------------------------------------------------
void CSourceView::CEditor::OnStorageDataChanged(IStorageLink::EParam nParameterChanged)
{
	//���������/����������� WhiteSpace
	//���������/����������� ������������ ����� 
	if (oneof(nParameterChanged,IStorageLink::pmEditor_ShowWhiteSpaces,IStorageLink::pmEditor_ShowIndentGuides,
		IStorageLink::pmEditor_ShowHorzSB,IStorageLink::pmEditor_ShowVertSB))
	{
		SetViewSettins();
	}
	//������
	else if (oneof(nParameterChanged, IStorageLink::pmEditor_ShowSelectionMargin, IStorageLink::pmEditor_ShowFoldMargin,	
					IStorageLink::pmEditor_ShowNumberMargin))
	{
		SetMargins();
	}
	//������� ������
	else if (oneof(nParameterChanged,IStorageLink::pmEditor_ShowRightMargin,IStorageLink::pmEditor_ShowRightMarginAt))
	{
		SetDrawSettings();
		m_rOwner.Invalidate();
	}
	//����������� �������������� ReadOnly ������
	else if (nParameterChanged == IStorageLink::pmGeneral_AllowEditReadOnlyFiles)
	{
		SetReadonlySettings();
	}

};