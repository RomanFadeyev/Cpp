/********************************************************************
	created:	2002/07/11
	created:	11:7:2002   13:14
	filename: 	$(Projects)\sources\mtdev\dialogs\addruledlg.cpp
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	addruledlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Диалог для добавления правила в грамматику
*********************************************************************/

#include "stdafx.h"
#include <BaseUtils.h>
#include <AppMessages.h>
#include <GUIService.h>
#include "Resource.h"
#include "AddRuleDlg.h"


#include "Names.h"
using namespace base_utils;

//Создание переходника для объекта
static i_objects::object_thunk< CdlgAddRule > CdlgAddRule(__MSG_PREFIX__);


IMPLEMENT_DYNAMIC(CdlgAddRule, CdlgDialog_B)

CdlgAddRule::CdlgAddRule(CWnd* pParent /*=NULL*/)
	: CdlgDialog_B(CdlgAddRule::IDD, pParent,ID_ACCEL)
{
	m_EditorHwnd=0;
	//{{AFX_DATA_INIT(CdlgAddRule)
	m_strModuleName	= _T("");
	//}}AFX_DATA_INIT
}

CdlgAddRule::~CdlgAddRule()
{
}

void CdlgAddRule::DoDataExchange(CDataExchange* pDX)
{
	CdlgDialog_B::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgAddRule)
	DDX_Control(pDX, IDC_RULE_NAME,	m_edRuleName);
	DDX_Control(pDX, IDC_STATIC_ERROR, m_lblError);
	DDX_Control(pDX, IDC_COMBO_PLACE_RULES, m_cmbPlaceRules);
	DDX_Control(pDX, IDC_COMBO_MODULES,	m_cmbModules);
	DDX_Control(pDX, IDC_EDIT_ITEM, m_btnEditTerm);
	DDX_Control(pDX, IDC_NEW_TERM, m_btnNewTerm);
	DDX_Control(pDX, IDC_MOVE_TERM_UP, m_btnMoveTermUp);
	DDX_Control(pDX, IDC_MOVE_TERM_DOWN, m_btnMoveTermDown);
	DDX_Control(pDX, IDC_DELETE_TERMS, m_btnDeleteTerm);
	DDX_Control(pDX, IDC_ADD_VARIANT, m_btnAddVariant);
	DDX_Control(pDX, IDC_ADD_TO_VARIANT, m_btnAddToVariant);
	DDX_Control(pDX, IDC_TREE1,	m_ResultTree);
	DDX_Control(pDX, IDC_EDIT3,	m_tmpEdt);
	DDX_Control(pDX, IDC_LIST1,	m_TermsList);
	DDX_Control(pDX, IDC_LIST2,	m_AvailableTermsList);
	DDX_Control(pDX, IDC_VAR_TAB, m_VarTable);
	DDX_Control(pDX, IDC_CHECK_OPTIONAL, m_chkIsOptional);
	DDX_Control(pDX, IDC_IS_TERM, m_rdoIsTerm);
	DDX_Control(pDX, IDC_IS_UNTERM, m_rdoIsUnTerm);
	DDX_Text(pDX, IDC_RULE_NAME, m_strRuleName);
	DDX_CBString(pDX, IDC_COMBO_MODULES, m_strModuleName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgAddRule, CdlgDialog_B)
	//{{AFX_MSG_MAP(CdlgAddRule)
	ON_BN_CLICKED(IDC_ADD_VARIANT, OnAddVariant)
	ON_NOTIFY(TCN_SELCHANGE, IDC_VAR_TAB, OnSelchangeVarTab)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkTermsList)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnKillfocusTmpEdt)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkAvailableTermsList)
	ON_NOTIFY(TCN_SELCHANGING, IDC_VAR_TAB,	OnSelchangingVarTab)
	ON_LBN_SELCHANGE(IDC_LIST1,	OnSelchangeTermsList)
	ON_BN_CLICKED(IDC_IS_TERM, OnRadioTerm)
	ON_BN_CLICKED(IDC_IS_UNTERM,	OnRadioUnterm)
	ON_BN_CLICKED(IDC_EDIT_ITEM, OnEditButton)
	ON_BN_CLICKED(IDC_CHECK_OPTIONAL, OnCheckOptional)

	ON_COMMAND(IDC_MOVE_TERM_UP,	cmdOnMoveTermUp)
	ON_COMMAND(IDC_MOVE_TERM_DOWN,  cmdOnMoveTermDown)
	ON_COMMAND(IDC_DELETE_TERMS,	cmdOnDeleteTerms)	
	ON_COMMAND(IDC_ADD_TO_VARIANT,	cmdOnAddToVariant)	
	ON_COMMAND(IDC_NEW_TERM,		cmdOnNewTerm)
	ON_CBN_SELCHANGE(IDC_COMBO_MODULES, OnCbnSelchangeComboModules)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
bool CdlgAddRule::AddRule()
{
	ISourceDocLink * pDoc = dynamic_cast<ISourceDocLink *> (AfxGetApp()->OpenDocumentFile(m_strModuleName));
	IParserLink * pParserIFace = singleton_cache::GetParserLink();

	if (!(pDoc && pParserIFace)) {
		guiMsgError(m_hWnd,MSG_ERR_UnexpectedError());
		return false;
	}
	
	long nInsPos =0;	
	string_smart strInsertText;

	//Здесь мы выполняем основную подготовку к вставке, а затем - саму вставку
	try
	{
		IParserLink::syntax_info SInfo;

		//Добавляем правило
		IParserLink::syntax_item & SItemRule = SInfo.m_Rules.AddUnit();
		SItemRule.strName=LPCTSTR(m_strRuleName);

		//Заполняем правило
		for	(size_t i=0; i<m_Variants.size();++i)
		{
			//Добавляем вариант
			IParserLink::syntax_item & SVariant = SItemRule.AddUnit();
			variant_contents & CurVariant=m_Variants[i];

			for	(variant_contents::iterator i=CurVariant.begin(); i!=CurVariant.end(); ++i)
			{
				//Добавляем терм
				IParserLink::syntax_item & STerm = SVariant.AddUnit();
				STerm.strName=(*i).c_str();
			}
		}
		//Отправляем данные парсеру - он знает формат оформления текста
		pParserIFace->AssembleToText(SInfo,strInsertText);

		//Находим позицию, куда будет вставлен наш текст
		DWORD_PTR dwData;
		if (m_cmbPlaceRules.GetCurItemData(dwData)){
			nInsPos = reinterpret_cast<rule_data*>(dwData)->m_nEndPos+1;
		}
		else {
			string_smart strText;pDoc->GetViewLink()->GetText(strText);
			//Если есть текст, то его надо пропустить через парсер
			if (!strText.isempty()){
				IParserLink::syntax_info SyntaxData; //Для получения данных после разбора
				nInsPos = strText.length()-1;
				pParserIFace->ParseSourceText(strText, __FUNCSIG__, SyntaxData);

				//Добираемся до последнего элемента и берем его конечную позицию
				if (SyntaxData.m_Rules.GetUnitsCount()>0)
					nInsPos = SyntaxData.m_Rules.GetUnitAt(SyntaxData.m_Rules.GetUnitsCount()-1).nPosEnd;
			}
		}

		//Вставка текста. 
		pDoc->GetViewLink()->InsertText(strInsertText.c_str(), nInsPos);
	}
	catch (...){
		guiMsgError(m_hWnd,MSG_ERR_UnexpectedError());
		return false;
	}
	
	m_EditorHwnd =0;

	//Выполнение данных действий не является особо важным, главное - 
	//текст уже вставлен. Поэтому возвращаем в любом случае true
	LOG_PROTECT_CODE_BEGIN
	//Пропускаем начальные переводы строк
	long nOffs =0;
	for (cstr szPos=strInsertText.c_str();szPos && oneof(*szPos,'\n','\r'); szPos++)
		nOffs++;
	pDoc->GetViewLink()->GotoPos(nInsPos+nOffs);
	pDoc->GetViewLink()->EnsureRangeVisible(nInsPos+nOffs,nInsPos+strInsertText.length());
	pDoc->GetViewLink()->SelectRange(nInsPos+nOffs,nInsPos+strInsertText.length());

	m_EditorHwnd=pDoc->GetViewLink()->GetObject()->m_hWnd;
	LOG_PROTECT_CODE_END

	return true;
}
//------------------------------------------------------------------------
bool CdlgAddRule::Run()
{
	bool Res = DoModal()==IDOK;
	if (Res){
		//Переносим фокус на редактор, куда было добавлено правило
		AfxGetMainWnd()->SetFocus ();
		::SetFocus(m_EditorHwnd);
	}
	return Res;
}
//------------------------------------------------------------------------
void CdlgAddRule::BuildTree()
{
	int	CurSel=m_VarTable.GetCurSel();
	if ((CurSel!=-1) &&	(m_TermsList.GetCount()!=0))
	{
		variant_contents & CurVariant=m_Variants[CurSel];

		CurVariant.clear();
		for	(int i=0;i<m_TermsList.GetCount();i++) 
			CurVariant.push_back(LPCTSTR(m_TermsList.GetText(i)));
	}
	
	m_ResultTree.BeginUpdate();
	m_ResultTree.DeleteAllItems();

	for	(size_t i=0; i<m_Variants.size(); ++i)
	{
		CString	VariantName;
		VariantName.Format("Variant%d",i+1);
		HTREEITEM Item=m_ResultTree.InsertItem(VariantName);
	
		variant_contents & CurVariant=m_Variants[i];
		for	(variant_contents::iterator i=CurVariant.begin(); i!=CurVariant.end(); ++i)
			m_ResultTree.InsertItem((*i).c_str(),Item);

		m_ResultTree.Expand(Item,TVE_EXPAND);
	}
	m_ResultTree.EndUpdate();
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// CdlgAddRule message handlers
//------------------------------------------------------------------------
void CdlgAddRule::OnIdle()
{	
	bool bVarTermSelected = m_TermsList.GetCurItemIndex()!=-1;
	bool bAvailTermSelected = m_AvailableTermsList.GetCurItemIndex()!=-1;
	m_btnDeleteTerm.EnableWindow(bVarTermSelected);
	m_btnMoveTermDown.EnableWindow(bVarTermSelected);
	m_btnMoveTermUp.EnableWindow(bVarTermSelected);
	m_btnEditTerm.EnableWindow(m_TermsList.GetSelCount()==1);
	m_chkIsOptional.EnableWindow(bVarTermSelected);
	m_rdoIsUnTerm.EnableWindow(bVarTermSelected);
	m_rdoIsTerm.EnableWindow(bVarTermSelected);

	m_btnAddToVariant.EnableWindow(bAvailTermSelected);
}
//------------------------------------------------------------------------
BOOL CdlgAddRule::OnInitDialog()
{
	__super::OnInitDialog();
	
	m_tmpEdt.SetParent(&m_TermsList);
	#define TUNE_BTN(Btn,Icon) {Btn.SetIcon(Icon); Btn.SetFlat(true); CString S; Btn.GetWindowText(S); Btn.SetTextAndToolTip(S); Btn.SetTextAlign(UINT(-1));}
	TUNE_BTN(m_btnAddToVariant,IDI_ITEM_MOVE_ICON);
	TUNE_BTN(m_btnNewTerm,IDI_ITEM_NEW_ICON);
	TUNE_BTN(m_btnDeleteTerm,IDI_ITEM_DELETE_ICON);		
	TUNE_BTN(m_btnMoveTermDown,IDI_ITEM_MOVEDOWN_ICON);	
	TUNE_BTN(m_btnMoveTermUp,IDI_ITEM_MOVEUP_ICON);
	TUNE_BTN(m_btnEditTerm,IDI_ITEM_EDIT_ICON);
	#undef  TUNE_BTN

	m_lstRulesLocation.clear();
	m_lblError.SetBkColor(RGB(255,0,0));//GetSysColor(COLOR_3DFACE),,CLabel::Gradient);
	
	IDocMngrLink* pDocMngrLink=i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN_FALSE(pDocMngrLink);
	IProjectDocLink * pActiveProject= pDocMngrLink->GetActiveProject();
	ASSERT_RETURN_FALSE(pActiveProject);
	IParserLink * pParserLink= i_objects::GetInterface<IParserLink>();
	ASSERT_RETURN_FALSE(pParserLink);

	//Список модулей в проекте
	for (cstr szFileName = pActiveProject->GetFileFirst();szFileName; szFileName = pActiveProject->GetFileNext())
		if (pActiveProject->GetFileType(szFileName)==IProjectDocLink::fileSource)
			m_cmbModules.AddString(szFileName);
	m_cmbModules.SetCurSel(0);

	//Получаем список всех правил, доступных в проекте
	IParserLink::syntax_info SyntaxInfo;
	pParserLink->ParseWholeProject(*pActiveProject,SyntaxInfo);

	IParserLink::si_iterator i;
	//Правила
	for (i=SyntaxInfo.m_Rules.GetFirstUnitPos();!SyntaxInfo.m_Rules.IsEndUnitPos(i);) {
		IParserLink::syntax_item & SItem = SyntaxInfo.m_Rules.GetNextUnit(i);
		m_AvailableTermsList.AddString(string_smart("<",SItem.strName.c_str(),">").c_str());
		m_lstRulesLocation.push_back(rule_data(SItem.strName.c_str(),SItem.strFilename.c_str(),SItem.nPosStart,SItem.nPosEnd));
	}

	//Подборка автоимени
	string_smart strDefRuleName;
	for (int x=1;x<1000;x++){
		strDefRuleName = Format("Unnamed Rule %d",x);
		string_smart strDefRuleName2("\"",strDefRuleName.c_str(),"\"");
		
		bool bCollision = false;
		for (vector_rule_data::iterator i = m_lstRulesLocation.begin(); (!bCollision) && (i!=m_lstRulesLocation.end()); i++)
			bCollision = ((*i).m_strName==strDefRuleName) || ((*i).m_strName==strDefRuleName2);
		
		if (!bCollision)
			break;
	}

	m_edRuleName.SetWindowText(strDefRuleName.c_str());

	m_AvailableTermsList.SetCurItemIndex(0);
	OnCbnSelchangeComboModules();

	m_Variants.clear();
	OnAddVariant();
	m_edRuleName.SetFocus();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
//------------------------------------------------------------------------
void CdlgAddRule::OnAddVariant()	
{
	LRESULT	RES;
	OnSelchangingVarTab(NULL, &RES);

	m_Variants.push_back(variant_contents());
	m_VarTable.InsertItem(m_VarTable.GetItemCount(),Format("Variant %d",m_VarTable.GetItemCount()+1).c_str());
	m_VarTable.SetCurSel(m_VarTable.GetItemCount()-1);
	OnSelchangeVarTab(NULL,&RES);
}
//------------------------------------------------------------------------
void CdlgAddRule::OnSelchangeVarTab(NMHDR* pNMHDR, LRESULT* pResult)	
{
	int	CurSel=m_VarTable.GetCurSel();
	if (CurSel==-1) {*pResult=true;return;}

	variant_contents & CurVariant=m_Variants[CurSel];
	for	(variant_contents::iterator i=CurVariant.begin(); i!=CurVariant.end(); ++i)
		m_TermsList.AddString((*i).c_str());

	BuildTree();
	*pResult = 0;
}
//------------------------------------------------------------------------
void CdlgAddRule::OnSelchangingVarTab(NMHDR*	pNMHDR,	LRESULT* pResult) 
{
	int	CurSel=m_VarTable.GetCurSel();
	if (CurSel==-1) {*pResult=true;return;}

	variant_contents & CurVariant=m_Variants[CurSel];

	CurVariant.clear();
	for	(int i=0;i<m_TermsList.GetCount();i++) 
		CurVariant.push_back(LPCTSTR(m_TermsList.GetText(i)));

	m_TermsList.ResetContent();

	*pResult = false;
}
//------------------------------------------------------------------------
void CdlgAddRule::cmdOnAddToVariant() 
{
	if (m_btnAddToVariant.GetStyle() & WS_DISABLED) //Вот такой вот специфический Update для Accelerator
		return;

	for	(int i=0;i<m_AvailableTermsList.GetCount();i++)
		if (m_AvailableTermsList.GetSel(i)>0)
		{
			CString	Str;
			m_AvailableTermsList.GetText(i,Str);
			m_AvailableTermsList.SetSel(i,false);
			m_TermsList.SetCurItemIndex(m_TermsList.AddString(Str));
		}

	BuildTree();
}
//------------------------------------------------------------------------
void CdlgAddRule::cmdOnNewTerm() 
{
	if (m_btnNewTerm.GetStyle() & WS_DISABLED) //Вот такой вот специфический Update для Accelerator
		return;

	m_TermsList.SetCurItemIndex(m_TermsList.AddString("\"New (Un)Terminal\""));
	OnSelchangeTermsList();
	m_TermsList.SetFocus();
}
//------------------------------------------------------------------------
void CdlgAddRule::cmdOnDeleteTerms() 
{
	if (m_btnDeleteTerm.GetStyle() & WS_DISABLED) //Вот такой вот специфический Update для Accelerator
		return;

	int CurSel=m_TermsList.GetCurItemIndex();
	for	(int i=m_TermsList.GetCount()-1;i>=0;i--)
		if (m_TermsList.GetSel(i)>0)
			m_TermsList.DeleteString(i);

	m_TermsList.SetFocus();
	m_TermsList.SetCurItemIndex(min(CurSel,m_TermsList.GetCount()-1));
	BuildTree();	
}
//------------------------------------------------------------------------
void CdlgAddRule::cmdOnMoveTermUp()
{
	if (m_btnMoveTermUp.GetStyle() & WS_DISABLED) //Вот такой вот специфический Update для Accelerator
		return;

	for	(int i=1;i<m_TermsList.GetCount();i++)
		if (m_TermsList.GetSel(i)>0)
		{
			CString	Str;
			m_TermsList.GetText(i,Str);
			m_TermsList.DeleteString(i);
			m_TermsList.InsertString(i-1,Str);
			m_TermsList.SetSel(i-1,true);
		}
	BuildTree();	
}
//------------------------------------------------------------------------
void CdlgAddRule::cmdOnMoveTermDown() 
{
	if (m_btnMoveTermDown.GetStyle() & WS_DISABLED) //Вот такой вот специфический Update для Accelerator
		return;

	for	(int i=m_TermsList.GetCount()-2;i>=0;i--)
	{
		if (m_TermsList.GetSel(i)>0)
		{
			CString	Str;
			m_TermsList.GetText(i,Str);
			m_TermsList.DeleteString(i);
			m_TermsList.InsertString(i+1,Str);
			m_TermsList.SetSel(i+1,true);
		}
	}
	BuildTree();
}
//------------------------------------------------------------------------
void CdlgAddRule::OnDblclkTermsList() 
{
	CString	Str;
	CRect r;
	int	CurSel=m_TermsList.GetCurSel();
	if (CurSel==LB_ERR )
		return;

	m_TermsList.GetText(CurSel,Str);
	m_TermsList.GetItemRect(CurSel,&r);
	r.bottom+=5;
	m_tmpEdt.MoveWindow(r);
	m_tmpEdt.SetWindowText(Str);
	m_tmpEdt.ShowWindow(SW_SHOW);
	m_tmpEdt.SetFocus();
	m_tmpEdt.SetSel(0xFFFF0000);
	m_nCurrentlyEditedItem=CurSel;
}
//------------------------------------------------------------------------
void CdlgAddRule::OnKillfocusTmpEdt() 
{
	CString	Str;
	m_tmpEdt.GetWindowText(Str);
	ASSERT(m_nCurrentlyEditedItem>=0);
	m_tmpEdt.ShowWindow(SW_HIDE);
	m_TermsList.DeleteString(m_nCurrentlyEditedItem);
	m_TermsList.InsertString(m_nCurrentlyEditedItem,Str);
	m_TermsList.SetSel(m_nCurrentlyEditedItem);
	m_nCurrentlyEditedItem=-1;

	OnSelchangeTermsList();
	BuildTree();
}
//------------------------------------------------------------------------
void CdlgAddRule::OnDblclkAvailableTermsList() 
{
	cmdOnAddToVariant();
	BuildTree();
}
//------------------------------------------------------------------------
BOOL CdlgAddRule::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ((wParam==IDOK) &&  (m_tmpEdt.GetStyle()& WS_VISIBLE))
		return m_TermsList.SetFocus(),true;

	return __super::OnCommand(wParam, lParam);
}
//------------------------------------------------------------------------
void CdlgAddRule::OnOK()	
{
	UpdateData();
	if (m_strRuleName.IsEmpty())
	{
		m_lblError.SetWindowText(string_res(IDS_NO_RULE_NAME,HRESOURCES).c_str());
		m_edRuleName.SetFocus();
		m_lblError.ShowWindow(SW_SHOW);
		return;
	}

	if (m_strModuleName.IsEmpty())
	{
		m_lblError.SetWindowText(string_res(IDS_NO_MODULE_NAME,HRESOURCES).c_str());
		m_cmbModules.SetFocus();
		m_lblError.ShowWindow(SW_SHOW);
		return;
	}
	BuildTree(); 
	
	//Здесь мы непосредственно добавляем правило в редактор
	//если все удалось, можно закрывать диалог
	if (AddRule())
		__super::OnOK();
}
//------------------------------------------------------------------------
void CdlgAddRule::OnSelchangeTermsList() 
{
	//Меняем только показания только в случае одного выделенного элемента!
	if (m_TermsList.GetSelCount()>1)
		return;

	int	CurSel=m_TermsList.GetCurItemIndex();

	if (CurSel!=-1) 
	{
		CString	Str;
		m_TermsList.GetText(CurSel,Str);
		Str.TrimLeft("\t ");

		if ((Str.GetLength()>0) && (Str[0]=='[')){
			m_chkIsOptional.SetCheck(true);
			Str.Delete(0,1);
			Str.TrimLeft("\t ");
		}
		else
			m_chkIsOptional.SetCheck(false);

		m_rdoIsTerm.SetCheck((Str.GetLength()>0) && (Str[0]!='<'));
		m_rdoIsUnTerm.SetCheck((Str.GetLength()>0) && (Str[0]=='<'));
	}
}
//------------------------------------------------------------------------
void CdlgAddRule::OnRadioTerm() 
{
	for	(int i=0;i<m_TermsList.GetCount();i++)
		if (m_TermsList.GetSel(i)>0)
		{
			CString	Str;
			m_TermsList.GetText(i,Str);
			
			Str.Trim("\t ");
			if (Str.GetLength()==0)
				continue;

			if ((Str.GetLength()>=2) && (Str[0]=='[') && (Str[1]=='<'))
				Str.Delete(1);
			else if (Str[0]=='<')
				Str.Delete(0);

			if ((Str.GetLength()>=2) && (Str[Str.GetLength()-1]==']') && (Str[Str.GetLength()-2]=='>'))
				Str.Delete(Str.GetLength()-2);
			else if (Str[Str.GetLength()-1]=='>')
				Str.Delete(Str.GetLength()-1);
			
			m_TermsList.DeleteString(i);
			m_TermsList.InsertString(i,Str);
			m_TermsList.SetSel(i);
		}
	BuildTree();	
}
//------------------------------------------------------------------------
void CdlgAddRule::OnRadioUnterm() 
{
	for	(int i=0;i<m_TermsList.GetCount();i++)
		if (m_TermsList.GetSel(i)>0)
		{
			CString	Str;
			m_TermsList.GetText(i,Str);
			
			Str.Trim("\t ");
			if (Str.GetLength()==0)
				Str="<>";
			else {
				if ((Str.GetLength()>=2) && (Str[0]=='[') && (Str[1]!='<'))
					Str.Insert(1,'<');
				else if (!oneof(Str[0],'<','['))
					Str.Insert(0,'<');
				
				if ((Str.GetLength()>=2) && (Str[Str.GetLength()-1]==']') && (Str[Str.GetLength()-2]!='>'))
					Str.Insert(Str.GetLength()-1,'>');
				else if (!oneof(Str[Str.GetLength()-1],'>',']'))
					Str+='>';
			}

			m_TermsList.DeleteString(i);
			m_TermsList.InsertString(i,Str);
			m_TermsList.SetSel(i);
		}
	BuildTree();	
}
//------------------------------------------------------------------------
void CdlgAddRule::OnEditButton() 
{
	if (m_TermsList.GetCount()==0) return;
	int	CurSel=m_TermsList.GetCurSel();
	if (CurSel!=-1)
	{
		OnDblclkTermsList();
	}
}
//------------------------------------------------------------------------
void CdlgAddRule::OnCheckOptional() 
{
	for	(int i=0;i<m_TermsList.GetCount();i++)
		if (m_TermsList.GetSel(i)>0)
		{
			CString	Str;
			m_TermsList.GetText(i,Str);
			Str.Trim("\t ");
			
			if (m_chkIsOptional.GetCheck()) {
				if (Str.GetLength()==0)
					Str="[]";
				else if ((Str[0]!='[') && (Str[Str.GetLength()-1]!=']'))
				{ Str.Insert(0,'[');Str+=']';}
			}
			else
				if ((Str.GetLength()>0) && (Str[0]=='[') && (Str[Str.GetLength()-1]==']'))
					{Str.Delete(0);	Str.Delete(Str.GetLength()-1);}

			m_TermsList.DeleteString(i);
			m_TermsList.InsertString(i,Str);
			m_TermsList.SetSel(i);
		}
	BuildTree();	
}
//------------------------------------------------------------------------
void CdlgAddRule::OnCbnSelchangeComboModules()
{
	m_cmbPlaceRules.ResetContent();
	
	CString strFileName; m_cmbModules.GetWindowText(strFileName);
	for (vector_rule_data::iterator i=m_lstRulesLocation.begin();i!=m_lstRulesLocation.end();i++)
		if (strFileName.CompareNoCase((*i).m_strFileName.c_str())==0)
			m_cmbPlaceRules.AddString((*i).m_strName.c_str(),(DWORD_PTR)&(*i));

	m_cmbPlaceRules.SetCurSel(m_cmbPlaceRules.GetCount()-1);
}
