/********************************************************************
	created:	2002/02/25
	created:	25:2:2002   13:14
	filename: 	C:\Sources\MultiTranslator.V2.5\Sources\MTDEv\Docs&Views\SourceDocTemplate.cpp
	file path:	C:\Sources\MultiTranslator.V2.5\Sources\MTDEv\Docs&Views

	file base:	SourceDocTemplate
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	���������� ������ ������� ��������� ��������� ������. ����������� �� 
				������������ CMultiDocTemplate � ����� �������� ��� ���������
*********************************************************************/

#include "stdafx.h"
#include <BaseUtils.h>

#include "Names.h"
#include "Resource.h"

#include "SourceDocTemplate.h"
#include "SourceDoc.h"
#include "SourceView.h"
#include "ChildFrm.h"
//////////////////////////////////////////////////////////////////////
// CSourceDocTemplate::CRegistrator class
//////////////////////////////////////////////////////////////////////

static CSourceDocTemplate::CRegistrator DocTemplateRegistrator;

void CSourceDocTemplate::CRegistrator::OnCreateApp(){
	AfxGetApp()->AddDocTemplate(new CSourceDocTemplate(	//Source Documents
		IDR_DOC_SOURCE,
		RUNTIME_CLASS(CSourceDoc),
		RUNTIME_CLASS(CChildFrame),			// custom MDI child	frame
		RUNTIME_CLASS(CSourceView)
	));
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CSourceDocTemplate class
//////////////////////////////////////////////////////////////////////

CSourceDocTemplate::CSourceDocTemplate ( UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass ):
    CDocumentTemplateEx(nIDResource, pDocClass, pFrameClass, pViewClass)

{
}
//------------------------------------------------------------------------
CSourceDocTemplate::~CSourceDocTemplate()
{

}
//------------------------------------------------------------------------
CSourceDocTemplate::lst_doc_info CSourceDocTemplate::GetAdditionalSupportedDocs() const
{
	lst_doc_info result;	

	LOG_PROTECT_CODE_BEGIN
	ISourceDocStylerLink * pLink = singleton_cache::GetSourceDocStylerLink();
	_ASSERTE(pLink!=NULL);
	
	lst_doc_info lstDocs=pLink->GetDocTypes();
	for (lst_doc_info::iterator i = lstDocs.begin(); i!=lstDocs.end();++i)
	{
		DOCUMENT_INFO & d = (*i);
		CDocument * pDoc;
		
		string_list strFilters = str_split_text(d.m_strDocFilter.c_str(),";");
		
		//���������� ��� ����������, ���������� ��� ���������
		for (long i = strFilters.items().count()-1;i>=0;i--)
		{	
			string_smart strFilter (strFilters.items()[i]);
			strFilter.trim();
			if (!strFilter.isempty()) //�������� �����������
			{
				if (strFilter.pos('.')==-1)	//����� ����������, ��� �������� txt
					strFilter.insert("*.",0);
				else if (strFilter[0]=='.') //���������� � ���� .txt
					strFilter.insert("*",0);

				//���� ��� *.* ��� ����� ���������� ��� ���� � ������, �� ���������� ���
				if  ((strFilter.cmpi("*.*")) ||
					(const_cast<CSourceDocTemplate*>(this)->MatchDocType(strFilter.c_str(),pDoc)==yesAttemptNative))
					strFilter.erase();
			}

			//���������� �� ������
			if (strFilter.isempty())
				strFilters.items().remove(i);
			//������������ ��� ����
			else
				strFilters.items().set_at(i,strFilter.c_str());
		}

		//���� �� �������� � ������ ����-�� ���� �������, ������, ���� �������� ���������� ��� ���������  
		if (strFilters.items().count()!=0)
		{
			string_smart strWholeFilter = str_merge_text(strFilters,";");
			string_smart strWholeDocName (d.m_strDocTitle," (",strWholeFilter,")");
			result.push_back(DOCUMENT_INFO (strWholeDocName,strWholeFilter));
		}
	}
	LOG_PROTECT_CODE_END

	//����� �� ���������, ������� ��� ����������, � ��������, ���� � 
	//�������� �������������� �������
	return result;
}
