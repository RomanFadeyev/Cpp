/********************************************************************
	created:	2003/05/31
	created:	31:5:2003   19:13
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lexsource.cpp
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lexsource
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <OutStreams.h>
#include <ParseUtils.h>

#include "Define.h"
#include "LexSource.h"
#include "EWQueue.h"

using namespace out_streams;

//------------------------------------------------------------------------
// CLexSource
//------------------------------------------------------------------------
CLexSource::CLexSource():m_nCurPos(0)
{
}
//------------------------------------------------------------------------
CLexSource::~CLexSource()
{
}
//------------------------------------------------------------------------	
bool CLexSource::LoadFromString(string_adapter szFileName, string_smart & strText)
{
	//����� ������� ����� ����� ����������� ������������
	_STATIC_ASSERT((numeric_limits<char_t>::is_signed==0),char_datatype_must_be_unsigned);

	m_strFileName = szFileName;
	raw_buffer<attr> arrAttribs;

	arrAttribs.reserve(strText.length()+1);
	arrAttribs.zero();
	
	//��������������� ������: �����������, � ����� ����� ��������� ������ ������
	bool bString=false;
	bool bChar  =false;
	size_t nComments=0;

	cstr szText = strText.c_str();
	for (size_t i=0; *szText; ++szText,++i){
		char_t ch		= *szText;
		char_t ch_next	= *(szText+1);
		char_t ch_prev	= (i==0) ? 0 : *(szText-1);
		attr & at = arrAttribs[i];

		_ASSERT((int)ch>=0);

		//������/����� ������
		if ((ch=='\"') && (!bChar)){
			if ((bString) && (ch_prev!='\\'))
				bString=false;
			else
				bString=true;
		}
		//������/����� �������
		else if ((ch=='\'') && (!bString)){
			if ((bChar) && (ch_prev!='\\'))
				bChar=false;
			else
				bChar=true;
		}
		else if ((ch=='\n') && (ch_prev!='\\')){
			bChar=bString=false;//� ����� ������ ��� ����� � ������� ����������
		}
		
		//���� �� ����������� � ������ ������� ��������� �������, ���������� ��. ��������� ������ ����
		 if ((!bString) && (!bChar) && (isspace(ch)) && (isspace(ch_next)))
			at=litera::TAG_COMMENT;
		//�� ASCII
		else if ((UCHAR)ch>127)	
			at=litera::TAG_ALT;
		//�����
		else if (isalpha(ch)) 
			at=litera::TAG_LETTER;
		//�����
		else if (isdigit(ch)) 
			at=litera::TAG_DIGIT;
		//������ � ����
		else if (isspace(ch)) 
			at=litera::TAG_SPACE;
		//���� ����������
		else if (ispunct(ch)) 
			at=litera::TAG_PUNCT;
		else
			_ASSERT(false);

		if (at==litera::TAG_COMMENT)	//������� ��������, �������
			nComments++;			//��������� � �����������
	}
	
	//������������ ���������� ����� �� ���������� ���������
	m_lstText.clear();
	m_lstText.reserve(strText.length()-nComments+1);	//�������� ������ ����� ������ �������

	size_t nLine=0, nPos=0, nRealLine=1; //��������� ����� � 1
	const errors::ew_pos_corrector::list_mapped_pos & MappedPositions(errors::main_pos_corrector.GetMappedPositionsForFile(m_strFileName));

	for (size_t	i=0; i<strText.length();i++){
		if (arrAttribs[i]!=litera::TAG_COMMENT) {
			_ASSERT(arrAttribs[i]!=litera::TAG_NONE);
			m_lstText.push_back(litera_info(strText.c_str()[i],arrAttribs[i],nRealLine-1,nPos));
		}
		if (strText.c_str()[i]=='\n'){
			nLine++; nPos=0; 
			nRealLine = nLine+1;
			
			//��� ����� ��������, ���� �������� � ����� ����������� ������� ������
			//����� ���� ��������� � �������� ����� ��� ������ ������, �� �������� �������
			const errors::ew_pos_corrector::mapped_position * pPos = MappedPositions.GetMappedPosition(nLine+1);
			if (pPos!=NULL)
				nRealLine=pPos->map_from(nRealLine);
		}
		else
			nPos++;
	}
	//��������� ������ ��������� ������
	m_lstText.push_back(litera_info(0,litera::TAG_END,nLine,nPos));

	m_nCurPos=m_lstText.size();
	m_nCurPos=0;
	return true;
}
