/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   18:09
	filename: 	$(Projects)\sources\mtdev\dialogs\syntaxdiagramsdlg.h
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	syntaxdiagramsdlg
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#pragma once
#include "Resource.h" 

#include "CObjectEx.h" //Один из родителей
#include "BaseStructs.h"
#include "Names.h"

class CdlgSyntaxDiagrams : public CCObjectEx, public ISyntaxDiagramsLink
{	
protected:
	mtdev_sd::ILibraryInfoPtr	m_spSDLibraryInfo; //Инифиализационный итерфейс
	mtdev_sd::ISyntaxDiagramPtr m_spSD; //Непосредственно синтаксические диаграммы
	show_type   m_nShowType;
	IProjectDocLink * m_pProjectLink;

	bool LoadLibrary();
	bool Init();
	cstr GetPalcementDocExt()			const {return (m_nShowType == stTree) ? EXT_STP : EXT_SDP;}
	string_cstr GetPlacementDocPath()	const {return change_fileext(m_pProjectLink->GetPath().c_str(),GetPalcementDocExt());}
	
	// From ISyntaxDiagrams
	override bool Run(IProjectDocLink * pProjectLink, show_type nShowType);
public:
	CdlgSyntaxDiagrams();   // standard constructor
	virtual ~CdlgSyntaxDiagrams();
};
