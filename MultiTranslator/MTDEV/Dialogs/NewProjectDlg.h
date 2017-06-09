/********************************************************************
	created:	2002/03/03
	created:	3:3:2002   12:36
	filename: 	$(Projects)\Sources\MTDEv\Dialogs\NewProject.h
	file path:	$(Projects)\Sources\MTDEv\Dialogs

	file base:	NewProject
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Display a dialog to allow the user to select a file path.
*********************************************************************/
#pragma once

#include "CObjectEx.h" //���� �� ���������
#include "BaseStructs.h"


/////////////////////////////////////////////////////////////////////////////
// CdlgNewProject dialog
class CdlgNewProject : public CCObjectEx, public INewProjectLink
{ 
public:
	//����� - ������ �������� �������. �������
	class CProjectWizard: public INewProjectWizardLink
	{
	protected:
		HICON		m_hIcon;
		string_std	m_strTitle;
	public:
		//���������� ������ ��� ����������� � ������
		virtual HICON GetImage() {return m_hIcon;};
		//���������� ���� ��������
		virtual cstr  GetTitle(){return m_strTitle.c_str();};
		//����������� ����������
		virtual ~CProjectWizard(){};
	};
	
	//����� - ������ �������� ������� �� ���������
	class CProjectWizardDefault: public CProjectWizard
	{
	public:
		CProjectWizardDefault();
		override ~CProjectWizardDefault();
		override bool Run(IProjectDocLink* pIface){return true;}
	};
	
	//����� - ������ �������� ������������ ������� 
	class CProjectWizardSimple: public CProjectWizard
	{
	public:
		CProjectWizardSimple();
		override ~CProjectWizardSimple();
		override bool Run(IProjectDocLink* pIface);
	};


// Construction
public:
	CdlgNewProject();   // standard constructor

private:
	map<OLE_HANDLE,CProjectWizard*> m_mapRegisteredWizards;

protected:
	override bool Run(
		string_adapter szProjectNameDef, //��� ������� �� ���������
		string_adapter szProjectDirDef,	//������������ ���������� �� ���������
		OUT string_smart & out_strProjectName,	//��� �������
		OUT string_smart & out_strProjectDir,	//������������ ����������, ��� ����� �������� ������
		INewProjectWizardLink* & pWizardLink 
		);

	override void OnRunApp();
	override void OnStopApp();
};
