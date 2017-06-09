/********************************************************************
	created:	2002/04/26
	created:	10:3:2002	1:13
	filename:	$(Projects)\Sources\MTDEv\Builder.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	Builder
	file ext:	h
	author:		Fadeyev	R.V.
	
	purpose:	Класс, занимающийся	компиляцией	проекта
*********************************************************************/
#if !defined(AFX_BUILDER_H__D28218CA_B0CF_41E6_9C0C_51B06ED3516D__INCLUDED_)
#define AFX_BUILDER_H__D28218CA_B0CF_41E6_9C0C_51B06ED3516D__INCLUDED_

#pragma	once
#include <BaseUtils.h>
#include "CObjectEx.h" //Один из родителей

#include "BaseStructs.h"
#include "Names.h"

class CBuilder :public CCObjectEx,public IBuilderLink
{
public:
	class CBuildContext: public CWnd{
	//окно нужно для синхронизации сообщений. Окно создается в основном потоке
	//а вот некоторые методы работают в другом потоке
	private:
		CBuilder*	m_pBuilderLink;	
		
		afx_msg	LRESULT msgOnSelfDestroy(WPARAM wParam, LPARAM lParam);
		afx_msg	LRESULT msgOnSelfFree(WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()
	public:
		CBuildContext(CBuilder* pBuilderLink);
		~CBuildContext();

		void SelfFree(DWORD exit_code);		//уничтожение самого себя
	};
protected:
	CBuildContext  *	m_pBuildContext; //создается динамически при старте компиляции
	IProjectDocLink *	m_pProjectLink;
	functor_build		m_pfCallBk;	//Обратная функция при завершении компиляции

	HANDLE m_hCurrentBuildThread; //Запущенный поток, который запускает процесс. По нему можно судить о занятости
	HANDLE	m_hCurrentRunProcess;

	//Процедура	потока компиляции
	static DWORD WINAPI CompilerThread(LPVOID);
	//Методы IBuilderLink
	override void IBuilderLink::Build(IProjectDocLink	* pProjectLink,
									 functor_build pfCallBk=functor_build());//Начало компиляции
	override void IBuilderLink::Stop();			//Принудительная остановка текущего выполнения
	override bool IBuilderLink::IsBusy();		//Выполняется ли в данный момент компиляция
	
	//Если выполняется компиляция, то какого проекта
	override IProjectDocLink * IBuilderLink::GetBuildingProject() {return m_pProjectLink;};

	//Закрытие главного окна
	override void OnCloseQueryAppMainWnd(bool & CanClose);
public:
	CBuilder();
	virtual	~CBuilder();
};
#endif // !defined(AFX_BUILDER_H__D28218CA_B0CF_41E6_9C0C_51B06ED3516D__INCLUDED_)