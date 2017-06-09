// StatisticDlg.cpp : implementation file
//

#include "stdafx.h"
#include <GUIService.h>
#include <AppMessages.h>
#include "StatisticDlg.h"
using namespace base_utils;

//Создание переходника для объекта
static i_objects::object_thunk< CdlgStatistic > dlgStatistic(__MSG_PREFIX__);

static UINT Images[][6]= {
	// Unselected.Collapsed UnSelected.Expanded Selected.Collapsed Selected.Expanded
	{0,0,0,0},	//Rules Folder
	{1,1,1,1},	//Rule Item
	{2,2,2,2},	//Variant Item
	{3,3,3,3},	//Failed 
	{4,4,4,4},	//OK
	{5,5,5,5}	//Unknown
	};	

#define UC 0
#define UE 1
#define SC 2
#define SE 3

#define ITEM_IMAGE_ROOT 				0
#define ITEM_IMAGE_RULE					1
#define ITEM_IMAGE_VARIANT				2
#define ITEM_IMAGE_FAILED				3
#define ITEM_IMAGE_OK					4
#define ITEM_IMAGE_UNKWN				5

/////////////////////////////////////////////////////////////////////////////
// CdlgStatistic dialog


CdlgStatistic::CdlgStatistic(CWnd* pParent /*=NULL*/)
	: CdlgDialog_B(IDD)
{
}
//------------------------------------------------------------------------
void CdlgStatistic::DoDataExchange(CDataExchange* pDX)
{
	CdlgDialog_B::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgStatistic)
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_Messages);
	DDX_Control(pDX, IDC_OPTIMIZE_PROGRESS, m_OptimizeProgress);
	DDX_Control(pDX, IDC_TREE_RULES_OPT, m_RulesTreeOptmz);
	DDX_Control(pDX, IDC_TREE_RULES, m_RulesTree);
	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CdlgStatistic, CdlgDialog_B)
	ON_BN_CLICKED(IDC_BUTTON_OPTIMIZE, OnButtonOptimize)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_RULES, OnItemExpandedTreeRules)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_RULES_OPT, OnItemExpandedTreeRulesOpt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgStatistic message handlers

BOOL CdlgStatistic::OnInitDialog() 
{
	if (!__super::OnInitDialog()) return false;
	
	CFile	StatisticFile;
	CFileException * pError = new CFileException;

	if (!StatisticFile.Open(change_fileext(m_pProjectLink->GetPath().c_str(),".stc").c_str(),
							CFile::modeRead, pError))
	{
		guiMsgAttention(0,string_res(IDS_NO_STATISTIC_FILE,HRESOURCES).c_str());
		return true;	
	}

	m_ImgLst.Create(IDB_STATISTIC,16,10,GetSysColor(COLOR_WINDOW));
	m_RulesTree.SetImageList(&m_ImgLst,TVSIL_NORMAL);
	m_RulesTreeOptmz.SetImageList(&m_ImgLst,TVSIL_NORMAL);

	HTREEITEM					m_RuleRootFolder;
	HTREEITEM					m_RuleRootFolderOptmz;
	m_RuleRootFolder=m_RulesTree.InsertItem(string_res(IDS_CLW_RULES_TITLE,HRESOURCES).c_str(),Images[ITEM_IMAGE_ROOT][UC], Images[ITEM_IMAGE_ROOT][SC]);
	m_RuleRootFolderOptmz=m_RulesTreeOptmz.InsertItem(string_res(IDS_CLW_RULES_TITLE,HRESOURCES).c_str(),Images[ITEM_IMAGE_ROOT][UC], Images[ITEM_IMAGE_ROOT][SC]);
	m_RulesTree.SetItemData(m_RuleRootFolder,DWORD(m_RuleRootFolderOptmz));
	m_RulesTreeOptmz.SetItemData(m_RuleRootFolderOptmz,DWORD(m_RuleRootFolder));

	CArchive Archive(&StatisticFile,CArchive::load);
	try	
	{
		m_Rules.clear();
		size_t Count;
		Archive>>(size_t &) Count;
		for (size_t i=0;i<Count;i++)
		{
			size_t		 nLen;
			string_smart strName;
			size_t		 nVariants;
			CArray  <size_t, size_t> aCalls;
			CArray  <size_t, size_t> aSortedCalls;
			CArray  <size_t, size_t>  aOptimizeMap;

			Archive>>(DWORD &)	nLen;	//Длина имени правила
			//Имя правила
			strName.reserve(nLen+1); Archive.Read(strName.buffer(), nLen);	strName.buffer()[nLen]=0;
			
			HTREEITEM hRItem	=m_RulesTree.InsertItem(strName.c_str(),Images[ITEM_IMAGE_RULE][UC], Images[ITEM_IMAGE_RULE][SC],m_RuleRootFolder,TVI_LAST);
			HTREEITEM hRItemOptmz=m_RulesTreeOptmz.InsertItem(strName.c_str(),Images[ITEM_IMAGE_RULE][UC], Images[ITEM_IMAGE_RULE][SC],m_RuleRootFolderOptmz,TVI_LAST);
			m_RulesTree.SetItemData(hRItem,DWORD(hRItemOptmz));
			m_RulesTreeOptmz.SetItemData(hRItemOptmz,DWORD(hRItem));

			//Добавляем правило
			CRuleInfo Rule(	hRItem,hRItemOptmz,string_smart("\"",strName.c_str(),"\"").c_str(), false);
	
			//Вычитываем варианты
			Archive>>nVariants;		//Кол-во вариантов в правиле
			
			aOptimizeMap.RemoveAll();
			for (size_t j=0; j<nVariants; j++)
			{
				DWORD   nCalls;
	
				Archive>>nCalls;	//Кол-во вызовов
				aCalls.Add(nCalls);
				int k; for (k=aSortedCalls.GetSize()-1;k>=0;k--)
					if (aSortedCalls[k]>=nCalls) break;

				aSortedCalls.InsertAt(k+1,nCalls);
				aOptimizeMap.InsertAt(k+1,j); // Соответствие: k+1 - желаемая позиция,
											  // j - номер варианта
			}
			for (size_t j=0; j<nVariants; j++)
			{
				HTREEITEM hVItem = m_RulesTree.InsertItem( Format("Variant %d - calls:  %d",j+1,aCalls[j]).c_str(),
												Images[ITEM_IMAGE_VARIANT][UC],
												Images[ITEM_IMAGE_VARIANT][SC],hRItem,TVI_LAST);
				//Определяем желаемое место варианта в правиле
				size_t k; for (k=0;(k<nVariants) && (aOptimizeMap[k]!=j);k++);
				Rule.m_bNeedOptimize|=k!=j; //Если текущая позиция варианта не оптимальна по порядку

				if (Rule.m_bNeedOptimize)
					m_RulesTreeOptmz.SetItemImage(hRItemOptmz,Images[ITEM_IMAGE_UNKWN][UC],Images[ITEM_IMAGE_UNKWN][SC]);
			
				//Добавляем собранную информацию в список
				CVariantInfo Variant(Format("variant № %d",j+1).c_str(),j,k);
				Rule.vars_list.push_back(Variant);
			}
			
			for (size_t i=0; i<nVariants; i++)
				for (size_t j=0; j<nVariants; j++)
					if (Rule.vars_list[j].m_nNewPos==i) {
						m_RulesTreeOptmz.InsertItem(Format("Variant %d - calls:  %d",j+1,aCalls[j]).c_str(),
														Images[ITEM_IMAGE_VARIANT][UC],
														Images[ITEM_IMAGE_VARIANT][SC],hRItemOptmz,TVI_LAST);
					}
				
			
			m_Rules.push_back(Rule);	//Вот здесь правило добавляется в список
		}
	}
	catch (...)
	{
		guiMsgAttention(0,string_res(IDS_CANT_READ_FILE_s,HRESOURCES).c_str(),(LPCTSTR)StatisticFile.GetFileName());
		Archive.Close();
		return true;	
	}
	
	Archive.Close();
	m_RulesTree.Expand(m_RuleRootFolder,TVE_EXPAND);
	m_RulesTreeOptmz.Expand(m_RuleRootFolderOptmz,TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//------------------------------------------------------------------------
void CdlgStatistic::OnDestroy()
{
	m_ImgLst.DeleteImageList();
	__super::OnDestroy();
}
//------------------------------------------------------------------------
bool CdlgStatistic::Run(IProjectDocLink * pProjectLink)
{
	if (!is_file_exist(change_fileext(pProjectLink->GetPath().c_str(),".stc").c_str())){
		guiMsgAttention(0,string_res(IDS_NO_STATISTIC_FILE,HRESOURCES).c_str());
		return false;
	}
	
	m_pProjectLink=pProjectLink;	
	
	return DoModal()==IDOK;
}
//------------------------------------------------------------------------
void CdlgStatistic::OnButtonOptimize() 
{
	//Для начала, парсим текущий текст проекта
	IParserLink * pParserLink = i_objects::GetInterface<IParserLink>();
	ASSERT_RETURN_VOID(pParserLink);
	
	LOG_PROTECT_CODE_BEGIN
	IParserLink::syntax_info SInfo;
	pParserLink->ParseWholeProject(*m_pProjectLink,SInfo);
	
	m_OptimizeProgress.SetStep(1);
	m_OptimizeProgress.SetRange(0,m_Rules.size());
	m_OptimizeProgress.SetPos(0);
		
	for (rules_list::iterator itrule = m_Rules.begin(); itrule!=m_Rules.end();++itrule)
	{
		//Отмечаем прохождение очередного правила
		m_OptimizeProgress.StepIt();	
		
		CRuleInfo &	Rule = *itrule;
		if (!Rule.m_bNeedOptimize) continue;

		//Начинаем искать правило в текущих исходниках
		IParserLink::si_iterator j;
		IParserLink::syntax_item * pSItem=NULL;
		for (j=SInfo.m_Rules.GetFirstUnitPos();!SInfo.m_Rules.IsEndUnitPos(j);) {
			pSItem=&SInfo.m_Rules.GetNextUnit(j);
			if (pSItem->strName==Rule.m_strName)
				break;
			else
				pSItem=NULL;
			
		}
		
		if (!pSItem)	//Так и не нашли
		{
			Message(Format("Warning: rule %s not found in project",Rule.m_strName.c_str()).c_str());
			SetRuleState(Rule,CRuleInfo::rsFailed);
		}
		else
		{
			CDocument *pDoc=AfxGetApp()->OpenDocumentFile(pSItem->strFilename.c_str());
			ISourceDocLink * pDocLink;
			if (!pDoc)	{//файл открыть не удалось
				Message(Format("Error: Cannot open document %s. Operation aborted",pSItem->strFilename.c_str()).c_str());	
				break; /*remove*/
			}
			//Мы можем работать только с классом ISourceDocLink
			else if ((pDocLink = dynamic_cast <ISourceDocLink *> (pDoc))==NULL){
				Message(Format("Warning: rule %s not found in project",Rule.m_strName.c_str()).c_str());
				SetRuleState(Rule,CRuleInfo::rsFailed);
			}
			else if (pSItem->GetUnitsCount()!=Rule.vars_list.size()){
				Message(Format("Warning: rule %s has different quantity of variants",Rule.m_strName.c_str()).c_str());
				SetRuleState(Rule,CRuleInfo::rsFailed);
			}
			else{
				string_smart strBuffer;
				pDocLink->GetViewLink()->GetText(strBuffer);
				for (size_t i=0; i<Rule.vars_list.size();i++)	//Этот цикл, чтобы выполнить перебор по возрастающей, своего рода сортировка
					for (size_t j=0; j<Rule.vars_list.size();j++)
						if ((Rule.vars_list[j].m_nNewPos==i) && 
						    (Rule.vars_list[j].m_nNewPos!=Rule.vars_list[j].m_nPos)) //При x == y вариант должен отстаться на том же месте
						{
							
							CVariantInfo & vi = Rule.vars_list[j];
							IParserLink::syntax_item & svi= pSItem->GetUnitAt(j);

							//Элементы форматирования
							for (long & k=pSItem->GetUnitAt(i).nPosStart;
								(k>0)&& (isspace(strBuffer[k-1])) && (strBuffer[k-1]!='\n');k--);
							for (long & k=pSItem->GetUnitAt(i).nPosEnd;
								(strBuffer[k]) && (isspace(strBuffer[k])) && (strBuffer[k-1]!='\n');k++);
							
							for (long & k=pSItem->GetUnitAt(j).nPosStart;
								(k>0)&& (isspace(strBuffer[k-1])) && (strBuffer[k-1]!='\n');k--);
							for (long & k=pSItem->GetUnitAt(j).nPosEnd;
								(strBuffer[k]) && (isspace(strBuffer[k])) && (strBuffer[k-1]!='\n');k++);
							
							// Перестановка варианта
							int Delta=svi.nPosEnd-svi.nPosStart;							
							ASSERT_RETURN_VOID(Delta>=0); //На всякий случай
								
							strBuffer.move(	svi.nPosStart,Delta,pSItem->GetUnitAt(i).nPosStart);
							//У всех вариантов после только что переставленного
							//нужно подправить позицию
							for (size_t k=i; k<j; k++){
								pSItem->GetUnitAt(k).nPosStart+=Delta;
								pSItem->GetUnitAt(k).nPosEnd+=Delta;
								Rule.vars_list[k].m_nPos++;
							}
							SetRuleState(Rule,CRuleInfo::rsPassed);
							Rule.m_bNeedOptimize=false;													
						}
				pDocLink->GetViewLink()->SetText(strBuffer.c_str());
			}
		}
	} // of for 
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CdlgStatistic::Message(cstr szMsg)
{
	m_Messages.AddString(szMsg);
} 
//------------------------------------------------------------------------
void CdlgStatistic::SetRuleState(CRuleInfo & Rule, CRuleInfo::ERuleState nState)
{
	ASSERT(Rule.m_hItem2!=NULL);	//Это hItem к m_RulesTree
	
	switch  (nState)
	{
		case CRuleInfo::rsReset:
			m_RulesTreeOptmz.SetItemImage(Rule.m_hItem2,Images[ITEM_IMAGE_RULE][UC],Images[ITEM_IMAGE_RULE][SC]);
			break;
		case CRuleInfo::rsPassed:
			m_RulesTreeOptmz.SetItemImage(Rule.m_hItem2,Images[ITEM_IMAGE_OK][UC],Images[ITEM_IMAGE_OK][SC]);		
			break;
		case CRuleInfo::rsFailed:
			m_RulesTreeOptmz.SetItemImage(Rule.m_hItem2,Images[ITEM_IMAGE_FAILED][UC],Images[ITEM_IMAGE_FAILED][SC]);		
			break;
		default: 
			ASSERT(FALSE);
	}
}
//------------------------------------------------------------------------
void CdlgStatistic::OnItemExpandedTreeRules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hPairItem = reinterpret_cast<HTREEITEM> (pNMTreeView->itemNew.lParam); 		
	*pResult = 0;

	if (hPairItem && (pNMTreeView->itemNew.state & TVIS_EXPANDED) && !m_RulesTreeOptmz.IsItemExpanded(hPairItem)) 
		m_RulesTreeOptmz.Expand(hPairItem,TVE_EXPAND);
}
//------------------------------------------------------------------------
void CdlgStatistic::OnItemExpandedTreeRulesOpt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hPairItem = reinterpret_cast<HTREEITEM> (pNMTreeView->itemNew.lParam); 	
	*pResult = 0;

	if (hPairItem && (pNMTreeView->itemNew.state & TVIS_EXPANDED) && !m_RulesTree.IsItemExpanded(hPairItem)) 
		m_RulesTree.Expand(hPairItem,TVE_EXPAND);
}
