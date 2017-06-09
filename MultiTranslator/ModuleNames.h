/********************************************************************
	created:	2002/10/05
	created:	5:10:2002   10:23
	filename: 	$(Projects)\sources\modulenames.h
	file path:	$(Projects)\sources
	file base:	modulenames
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	����� ��������� �������� ��������� ������� �������
*********************************************************************/
#include <BaseUtils.h>
#include <ProductEdition.h>
#pragma  once

using namespace base_utils;
namespace module_names
{
	//��� �����������
	#ifdef _DEBUG
	const_str szCompilerName       = _T("grmcd.exe");
	#else
	const_str szCompilerName       = _T("grmc.exe");
	#endif

	//��� �������������� ������
	#ifdef _DEBUG
	const_str szExecuterName       = _T("grmexecd.exe");
	#else
	const_str szExecuterName       = _T("grmexec.exe");
	#endif

	//��� �������������� ������ � ���� ������������ ���������� (��� �������)
	#ifdef  _DEBUG
	const_str szDebuggerName       = _T("grmexecd.dll");
	#else
	const_str szDebuggerName       = _T("grmexec.dll");
	#endif

	//��� ������������ ����������, ���������� ���� �������
	#ifdef  _DEBUG
	const_str szGrmKrnlName        = _T("grmkrnld.dll");
	#else                          
	const_str szGrmKrnlName        = _T("grmkrnl.dll");
	#endif

	const_str szSDLibName          = _T("mtsd.dll");

#ifndef USE_INTERNAL_HIGHLIGHTER
	#ifdef  _DEBUG
	const_str szSourceLexer        = _T("scilexerd.dll");
	#else                          
	const_str szSourceLexer        = _T("scilexer.dll");
	#endif
#endif

	//��� �������� ���������� ������� ��� ���� ��������
	//���������  CRT � C
	#ifdef  _DEBUG
	const_str szCrtLibName        = _T("grmcrtd.mtl");
	#else                          
	const_str szCrtLibName        = _T("grmcrt.mtl");
	#endif

	//��� ���������� c ������������ ��� mtdev (��������� mso.dll � Office)
	#ifdef  _DEBUG
	const_str szMTDevLibName     = _T("mtcg.dll"); //
	#else                         
	const_str szMTDevLibName     = _T("mtcg.dll"); //�������� ����������
	#endif


	//��� ��������� ��� ������ ������
	const_str szFindInFilesName    = _T("findstr.exe");
	const_str szReplaceInFilesName = _T("mung.exe");
	
	//��������� ����������, ������� ��������� 
	//�������, � ������� ����� ������ ��� ��, ��� � � ������� ��������
	const_str szEnvPath				= _T("ProjDLL");

	//������� � ������� ��������� ���������� 
	const_str szFolderHighLight		= _T("highlight");
	//������� � ��������� ���������
	const_str szFolderSkins			= _T("skins");
	//�������, ����������� �� ��������� ��� �������� ������� ������ ������
	const_str szFolderDocumentsDefault	= _T("samples");
	//������� � ��������� ��������� �������� ������� ��� ��������� ������
	const_str szFolderLangGen			= _T("generators");
	//������� � ����� ������
	const_str szFolderKnowledgeBase		= _T("Knowledge Base");
}
