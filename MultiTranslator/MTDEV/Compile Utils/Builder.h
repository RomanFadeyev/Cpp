/********************************************************************
	created:	2002/04/26
	created:	10:3:2002	1:13
	filename:	$(Projects)\Sources\MTDEv\Builder.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	Builder
	file ext:	h
	author:		Fadeyev	R.V.
	
	purpose:	�����, ������������	�����������	�������
*********************************************************************/
#if !defined(AFX_BUILDER_H__D28218CA_B0CF_41E6_9C0C_51B06ED3516D__INCLUDED_)
#define AFX_BUILDER_H__D28218CA_B0CF_41E6_9C0C_51B06ED3516D__INCLUDED_

#pragma	once
#include <BaseUtils.h>
#include "CObjectEx.h" //���� �� ���������

#include "BaseStructs.h"
#include "Names.h"

class CBuilder :public CCObjectEx,public IBuilderLink
{
public:
	class CBuildContext: public CWnd{
	//���� ����� ��� ������������� ���������. ���� ��������� � �������� ������
	//� ��� ��������� ������ �������� � ������ ������
	private:
		CBuilder*	m_pBuilderLink;	
		
		afx_msg	LRESULT msgOnSelfDestroy(WPARAM wParam, LPARAM lParam);
		afx_msg	LRESULT msgOnSelfFree(WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()
	public:
		CBuildContext(CBuilder* pBuilderLink);
		~CBuildContext();

		void SelfFree(DWORD exit_code);		//����������� ������ ����
	};
protected:
	CBuildContext  *	m_pBuildContext; //��������� ����������� ��� ������ ����������
	IProjectDocLink *	m_pProjectLink;
	functor_build		m_pfCallBk;	//�������� ������� ��� ���������� ����������

	HANDLE m_hCurrentBuildThread; //���������� �����, ������� ��������� �������. �� ���� ����� ������ � ���������
	HANDLE	m_hCurrentRunProcess;

	//���������	������ ����������
	static DWORD WINAPI CompilerThread(LPVOID);
	//������ IBuilderLink
	override void IBuilderLink::Build(IProjectDocLink	* pProjectLink,
									 functor_build pfCallBk=functor_build());//������ ����������
	override void IBuilderLink::Stop();			//�������������� ��������� �������� ����������
	override bool IBuilderLink::IsBusy();		//����������� �� � ������ ������ ����������
	
	//���� ����������� ����������, �� ������ �������
	override IProjectDocLink * IBuilderLink::GetBuildingProject() {return m_pProjectLink;};

	//�������� �������� ����
	override void OnCloseQueryAppMainWnd(bool & CanClose);
public:
	CBuilder();
	virtual	~CBuilder();
};
#endif // !defined(AFX_BUILDER_H__D28218CA_B0CF_41E6_9C0C_51B06ED3516D__INCLUDED_)