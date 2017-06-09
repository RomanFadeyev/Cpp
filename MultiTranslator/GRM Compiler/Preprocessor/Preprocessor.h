/********************************************************************
	created:	2004/10/23
	created:	23:10:2004   21:53
	filename: 	x:\multitranslator\sources\grm compiler\preprocessor\preprocessor.h
	file path:	x:\multitranslator\sources\grm compiler\preprocessor
	file base:	preprocessor
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once
#include "BaseStructs.h"
class CPreprocessor
{
private:
	string_smart m_strFilename;
	string_smart m_strInputTranslationDir;
	string_smart m_strOutputTranslationDir;
	
	const IBuildContext & m_BuildContext;

	bool ProcessComments(string_smart OUT & strData);
	bool ProcessDirectives(string_smart OUT & strData);
	bool ProcessInOutDirections(string_smart OUT & strData);
public:
	bool Analyze(string_adapter szFileName, string_smart OUT & strData);
	void SetInputTranslationDir(string_adapter strInDir) {m_strInputTranslationDir=strInDir;}
	void SetOutputTranslationDir(string_adapter strOutDir){m_strOutputTranslationDir=strOutDir;}

	CPreprocessor(const IBuildContext & BuildContext);
	~CPreprocessor(void);
};
