/********************************************************************
	created:	2002/01/07
	created:	7:1:2002   15:42
	filename: 	$(Projects)\Sources\MTDEv\CObjectEx.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	CObjectEx
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	������� ����� ����. ��������, ��������������� �� ������
				� ������ �������
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
					public IApplicationLink::IEventSink, //��������� ������� IApplicationLink				
					public IMainFrameLink::IEventSink,
					public IStorageLink::IEventSink
{
private:
	//IApplicationLink::IEventSink
	override void evOnCreateApp()		//�������� ����������
	{OnCreateApp();LoadSettings();}
	
	override void evOnCreateAppMainWnd()//�������� ������� �����
	{OnCreateAppMainWnd();}

	override void evOnRunApp()		//����������� ������� ����� � ������ ����� ���������
	{OnRunApp();}

	override void evOnStopApp()		//��������� ����������, ������ ������ OnRun
	{OnStopApp(); }
	
	override void evOnDestroyApp()		//���������� ����������, ������ ������ OnCreate
	{SaveSettings();OnDestroyApp(); if (IsConnected()) Disconnect();}

	//IMainFrameLink::IEventSink
	override void evOnCloseQueryAppMainWnd(evdCloseQueryMainWnd & Info)	//�������� ������� �����
	{	
		bool bCanClose=true;
		OnCloseQueryAppMainWnd(bCanClose);
		if (!bCanClose)
			Info.SupressClose();
	}
	
	//IStorageLink::IEventSink
	override void evOnChangeData(IStorageLink::EParam ParameterChanged) 
	{OnStorageDataChanged(ParameterChanged);}

	override void evOnCloseAppMainWnd()	//����� ����������� ������ �����
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

	//������ � ���������/����������� ���������� � �������. ���������� ���
	//�������� � �������� ����������
	virtual void LoadSettings() {}
	virtual void SaveSettings()	{}
public:
	CCObjectEx();
	virtual ~CCObjectEx();

};

#endif // !defined(AFX_COBJECTEX_H__F0C0595B_4091_4AFD_9123_A9C474DF9947__INCLUDED_)
