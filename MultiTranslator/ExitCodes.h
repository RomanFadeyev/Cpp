/********************************************************************
	created:	2002/08/19
	created:	19:8:2002   9:32
	filename: 	$(Projects)\sources\exitcodes.h
	file path:	$(Projects)\sources
	file base:	exitcodes
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	����� ��������� ���� �������� ��� ��������� �������� �������
*********************************************************************/

#pragma once

namespace exit_codes
{
	//���� �������� �������������� ������ (Executer.exe ��� grmexec[d].exe)
	//��� ������ ������ <0 � �������� �����������������
	//��� ��������� ���� �������� ������������ ���������������� �������� main
	//��� ������ ���, �� ������ � ������������� (� DLL ������ ��� ������� ��
	//���, ��� ��� ������ ������� Run ���������� ����������� �������� �� ������ ��� ��������� ����)
	//���� ������� main ���, ������������ 0
	enum ERunnerExitCodes	
	{
		recOK					= 0,	//��� ���������
		recNEED_PARAMS			=-1,	//��� ������� ����������
		recCANT_LOAD_LIB		=-2,	//�� ������� ��������� ��� ��������� ��� ������
		recCANT_LOAD_CODE		=-3,	//�� ������� ��������� ����������� ����
		recWRONG_FORMAT			=-4,	//�������� ������ bin-�����
		recWRONG_SFX_FORMAT		=-5,	//�������� ������ SFX-�����
		recNO_LOADED_CODE		=-6,	//��� ������������ ���� (DLL-������)
		recABNORMAL_TERMINATION =-7,	//����������� ���������� (���� ������ ��� �������� main)
		recFORCED_TERMINATION	=-255	//�������������� ����������
	};
	
	inline const char * ERunnerExitCodes_Str(ERunnerExitCodes nCode)
	{
		switch(nCode) {
		case recOK					: return "Completed. No errors.";		
		case recNEED_PARAMS			: return "Input parameters are absent.";
		case recCANT_LOAD_LIB		: return "Can't load library(s).";		
		case recCANT_LOAD_CODE		: return "Can't load executable file.";	
		case recWRONG_FORMAT		: return "Wrong format of executable file.";
		case recWRONG_SFX_FORMAT	: return "Wrong format of executable file.";
		case recNO_LOADED_CODE		: return "No loaded code.";				
		case recABNORMAL_TERMINATION: return "Abnormal termination.";			
		case recFORCED_TERMINATION	: return "Forced termination";				
		default:
			return "Unknown error";
		}
	}
}
