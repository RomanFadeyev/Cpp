/********************************************************************
	created:	2002/01/07
	created:	7:1:2002   15:42
	filename: 	$(Projects)\Sources\MTDEv\CObjectEx.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	CObjectEx
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Базовый класс конц. объектов, ориентированный на работу
				в данном проекте
*********************************************************************/

// CObjectEx.h: interface for the CCObjectEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COBJECTEX_H__F0C0595B_4091_4AFD_9123_A9C474DF9947__INCLUDED_)
#define AFX_COBJECTEX_H__F0C0595B_4091_4AFD_9123_A9C474DF9947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IObject.h>	
#include "BaseStructs.h"
using namespace i_objects;
using namespace std;

class CCObjectEx :	public i_objects::object, 
					public IApplicationLink::IEventSink, //Обработка событий IApplicationLink				
					public IMainFrameLink::IEventSink,
					public IStorageLink::IEventSink
{
private:
	//IApplicationLink::IEventSink
	override void evOnCreateApp()		//Создание приложения
	{OnCreateApp();LoadSettings();}
	
	override void evOnCreateAppMainWnd()//Создание главной формы
	{OnCreateAppMainWnd();}

	override void evOnRunApp()		//Отображение главной формы и запуск цикла сообщений
	{OnRunApp();}

	override void evOnStopApp()		//Остановка приложения, всегда парный OnRun
	{OnStopApp(); }
	
	override void evOnDestroyApp()		//Разрушение приложения, всегда парный OnCreate
	{SaveSettings();OnDestroyApp(); if (IsConnected()) Disconnect();}

	//IMainFrameLink::IEventSink
	override void evOnCloseQueryAppMainWnd(evdCloseQueryMainWnd & Info)	//Закрытие главной формы
	{	
		bool bCanClose=true;
		OnCloseQueryAppMainWnd(bCanClose);
		if (!bCanClose)
			Info.SupressClose();
	}
	
	//IStorageLink::IEventSink
	override void evOnChangeData(IStorageLink::EParam ParameterChanged) 
	{OnStorageDataChanged(ParameterChanged);}

	override void evOnCloseAppMainWnd()	//Перед разрушением гланой формы
	{OnCloseAppMainWnd();}

protected:
	virtual void OnCreateApp()			{}
	virtual void OnCreateAppMainWnd()	{}
	virtual void OnCloseQueryAppMainWnd(bool & CanClose){}
	virtual void OnCloseAppMainWnd()	{}
	virtual void OnRunApp()			{}
	virtual void OnStopApp()		{}
	virtual void OnDestroyApp()		{}
	virtual void OnStorageDataChanged(IStorageLink::EParam nParameterChanged) {}

	//Работа с загрузкой/сохранением параметров в реестре. Вызываются при
	//загрузке и выгрузке приложения
	virtual void LoadSettings() {}
	virtual void SaveSettings()	{}
public:
	CCObjectEx();
	virtual ~CCObjectEx();

};

#endif // !defined(AFX_COBJECTEX_H__F0C0595B_4091_4AFD_9123_A9C474DF9947__INCLUDED_)
