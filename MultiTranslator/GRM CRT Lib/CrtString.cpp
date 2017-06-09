/********************************************************************
	created:	2003/05/31
	created:	31:5:2003   0:05
	filename: 	$(Projects)\sources\grm crt lib\crtstring.cpp
	file path:	$(Projects)\sources\grm crt lib
	file base:	crtstring
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#include "StdAfx.h"
#include "grmcrt.h"
#include "tchar.h"

#include <BaseUtils.h>
#include <boost/regex.hpp>

using namespace base_utils;

const cstr  szCategory = _T("Strings");

namespace crt_strings
{
	//���������� n ����� �������� ������. ���� n > ����� ������, ���������� 
	//��� ������
	EXPORT size_t WINAPI StrLen(cstr szSrc){
		return strlen(szSrc);
	}
	registrator reg_len(StrLen,szCategory,"StrLen","Returns the length of a specified string");

	//���������	 ��������-�����������
	EXPORT bool WINAPI StrCmpI(cstr szSrc1, cstr szSrc2){
		return lstrcmpi(szSrc1,szSrc2)==0;
	}
	registrator reg_cmpi(StrCmpI,szCategory,"StrCmpI","Compares two strings to determine if they are the same. The comparison is not case-sensitive.");

	//���������	 ��������-���������
	EXPORT bool WINAPI StrCmp(cstr szSrc1, cstr szSrc2){
		return lstrcmp(szSrc1,szSrc2)==0;
	}
	registrator reg_cmp(StrCmp,szCategory,"StrCmp", "Compares two strings to determine if they are the same. The comparison is case-sensitive.");

	//���������� ��������� ������ ������. ���� ������ =NULL, ������	���������� 0
	//�������� bSkipSpaces ���������, ���������� ��	���	�����������	�������	� ����������
	//������ ������������ ������(���� ��� ������ ��	��������, ������������ 0), ��� ���������� 
	//����������� ���������	������.	
	EXPORT char WINAPI StrGetLast(cstr szSrc, bool bSkipSpaces=false){
		return string_smart(szSrc).get_last(bSkipSpaces);
	}
	registrator reg_get_last(StrGetLast,szCategory,"StrGetLast", "Returns last character of the string. Parameter bSkipSpaces specifies whether space characters have to be skipped or not.");

	//�������� ���������� ����������� ��������.	���� ��������� �� ������ ����
	//��� ������ ��� ������������� ������� � ����������
	EXPORT cstr WINAPI StrTrimLeft(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.trimleft().c_str();
	}
	registrator reg_trim_left(StrTrimLeft,szCategory,"StrTrimLeft","Trims leading spaces and control characters from a string");

	//�������� ��������	�����������	��������. ���� ��������� ��	������ ����
	//��� ������ ��� ������������� ������� � ����������
	EXPORT cstr WINAPI StrTrimRight(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.trimright().c_str();
	}
	registrator reg_trim_right(StrTrimRight,szCategory,"StrTrimRight","Trims trailing spaces and control characters from a string");

	//�������� ���������� �	�������� ����������� ��������. ����	��������� �� ������	����
	//��� ������ ��� ������������� ������� � ����������
	EXPORT cstr WINAPI StrTrim(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.trim().c_str();
	}
	registrator reg_trim_trim(StrTrim,szCategory,"StrTrim","Trims leading and trailing spaces and control characters from a string");

	//���������	������ n ��������� ch. ���������� ���������� ������������
	//���� ��������� ��	������ ����, ��� ������	���	������������� ������� �	����������
	EXPORT cstr WINAPI StrFillChar(char ch, size_t	nCount){
		STRING_IMPL("");
		return s.fill_char(ch,nCount).c_str();
	}
	registrator reg_fill_char(StrFillChar,szCategory,"StrFillChar","Returns a string with a specified number of repeating characters");

	//���������, ��������� �� ��������� ������� � ������ � � ����� ������
	EXPORT bool WINAPI StrIsFramed(cstr szSrc, char chLeftFrame, char chRightFrame){
		return string_smart(szSrc).frames_is_framed(chLeftFrame,chRightFrame);
	} 
	registrator reg_frames_is_framed(StrIsFramed,szCategory,"StrIsFramed","Checks if specified string is framed by specified characters");

	//������� ���������	�������	� �����	� �	������,	���� ��� ������������ 
	//������������.	 � ��������	�������	����� �������� ������ ����������� ������� �	����� �����
	EXPORT cstr WINAPI StrDeleteFrames(cstr szSrc, char chLeftFrame, char chRightFrame){
		STRING_IMPL(szSrc);
		return s.frames_delete(chLeftFrame,chRightFrame).c_str();
	}
	registrator reg_frames_delete(StrDeleteFrames,szCategory,"StrDeleteFrames","Deletes specified frames from the beginning and the end of the string");

	//����������� ������ � ��������� �������, ���� ��� ��� �� ������������
	//� ����� ������ � ������
	EXPORT cstr  WINAPI StrWrap(cstr szSrc, char chLeftFrame, char chRightFrame){
		STRING_IMPL(szSrc);
		return s.frames_wrap_on(chLeftFrame,chRightFrame).c_str();
	}
	registrator reg_wraps_on(StrWrap,szCategory,"StrWrap","Returns a copy of input string wrapped at the beginning and the end with specified characters");

	//���������	������ � ������	� ���������	�������
	//���� ������� ������ ���������, ����������� � ����� ������
	EXPORT cstr WINAPI StrInsert(cstr szSubStr, cstr szSrc, size_t nPos){
		STRING_IMPL(szSrc);
		return s.insert(szSubStr,nPos).c_str();
	}
	registrator reg_insert(StrInsert,szCategory,"StrInsert","Inserts a substring into a string beginning at a specified point");

	//������� ��������� ������(�) �� ������
	EXPORT cstr WINAPI StrDel(cstr szSrc, size_t nPos, size_t nLen=1){
		STRING_IMPL(szSrc);
		return s.del(nPos,nLen).c_str();
	}
	registrator reg_del(StrDel,szCategory,"StrDel","Removes a substring of specified length from a string starting from specified position");

	//���������� ��������� ���� � ����� ������� nNewPos. 
	//���� nPos+nLen ������ ����������� �����, ����������� ��� 
	//������ ����� 
	EXPORT cstr WINAPI StrMove(cstr szSrc, size_t nPos, size_t nLen, size_t nNewPos){
		STRING_IMPL(szSrc);
		return s.move(nPos,nLen,nNewPos).c_str();
	}
	registrator reg_move(StrMove,szCategory,"StrMove","Moves substring of specified length in specified string from one position to another");

	//���������	� �����	������ ������ nLen ��������	��������� ������
	//���� nLen	�������	������,	���	�������������� ����� ������, ��� ����������� ��	�����������
	//��������
	EXPORT cstr WINAPI StrCat(cstr szSrc, cstr szAppendStr, size_t	nLen){
		STRING_IMPL(szSrc);
		return s.cat(szAppendStr,nLen).c_str();
	}
	registrator reg_cat(StrCat,szCategory,"StrCat", "Appends one string to another");

	//���������� n ����� �������� ������. ���� n > ����� ������, ���������� 
	//��� ������
	EXPORT cstr WINAPI StrLeft(cstr szSrc, size_t nCount){
		STRING_IMPL(szSrc);
		s=s.get_left(nCount);
		return s.c_str();
	}
	registrator reg_left(StrLeft,szCategory,"StrLeft","Returns the substring of a specified length that appears at the start of a string");

	//���������� n ������ �������� ������. ���� n > ����� ������, ���������� 
	//��� ������
	EXPORT cstr WINAPI StrRight(cstr szSrc, size_t nCount){
		STRING_IMPL(szSrc);
		s=s.get_right(nCount);
		return s.c_str();
	}
	registrator reg_right(StrRight,szCategory,"StrRight","Extracts the substring which consists from last n characters of a string");

	//���������� n �������� ������, ������� � ���������� . ���� n > ���������� ����� ������, 
	//����������  ��������� ������ ����� �� ���������� ������� 
	EXPORT cstr WINAPI StrMid(cstr szSrc, size_t nFirst,size_t nCount){
		STRING_IMPL(szSrc);
		s=s.get_mid(nFirst,nCount);
		return s.c_str();
	}
	registrator reg_mid(StrMid,szCategory,"StrMid","Extracts a substring of n characters from a string, starting at specified position (zero-based)");

	//���������� ������� ������� ��������� ���������. ���� ��������� �� �������,
	//���������� -1. ����� ������� �����������������
	EXPORT long WINAPI StrPos(cstr szSrc, cstr szSubStr, size_t nStart=0){
		return string_smart(szSrc).pos(szSubStr,nStart);
	}
	registrator reg_pos(StrPos,szCategory,"StrPos","Returns the index value of the first character in a specified substring that occurs in a given string. Case-sensitive");

	//���������� ������� ������� ��������� ���������. ���� ��������� �� �������,
	//���������� -1. ����� �������������������
	EXPORT long WINAPI StrPosI(cstr szSrc, cstr szSubStr,size_t nStart=0){
		return string_smart(szSrc).posi(szSubStr,nStart);
	}
	registrator reg_posi(StrPosI,szCategory,"StrPosI", "Returns the index value of the first character in a specified substring that occurs in a given string. Case-insensitive");

	//���������� ������� ������� ��������� ���������, ������������ ���������� ����������. ���� ��������� �� �������,
	//���������� -1. 
	EXPORT bool WINAPI StrFindRegExp(cstr szSrc, cstr szRegExp, size_t nStart, int & nFoundStart, int & nFoundLen){
		
		std::string strSrc(szSrc+nStart);
		std::string::const_iterator start, end;
		start = strSrc.begin();
		end = strSrc.end();   
		boost::match_results<std::string::const_iterator> what;
		boost::match_flag_type flags = boost::match_default; 

		//flags |= boost::match_prev_avail;
		//flags |= boost::match_not_bob;

		while(boost::regex_search(start, end, what, boost::regex(szRegExp), flags))   
		{
			nFoundStart = int(what[0].first - start);
			nFoundLen = int((what[0].second - start)-nFoundStart);
			return true;
		}
		return false;
	}
	registrator reg_pos_regexp(StrFindRegExp,szCategory,"StrFindRegExp", "Searches the substring, which matches specified regular expression. Returns true, the start position and the length of found substring if succeeds");

	//������������ ���������� ��������� ���������� ������� � ������ (��������-���������)
	EXPORT size_t WINAPI StrQuant(cstr szSrc, char ch){
		return string_smart(szSrc).quant(ch);
	}
	registrator reg_quant(StrQuant,szCategory, "StrQuant","Calculates the number of repetition of specified character in the specified string. Case-sensitive");

	//������������ ���������� ��������� ���������� ������� � ������ (��������-�����������)
	EXPORT size_t WINAPI StrQuantI(cstr szSrc, char ch){
		return string_smart(szSrc).quanti(ch);
	}
	registrator reg_quanti(StrQuantI,szCategory,"StrQuantI", "Calculates the number of repetition of specified character in the specified string. Case-insensitive");

	//�������� � ������ ���� ��������� ������. ���������� ���-�� ����������� �����
	EXPORT cstr WINAPI StrReplace(cstr szSrc, cstr szOld, cstr szNew, bool bReplaceAll=true){
        	STRING_IMPL(szSrc);
        	s.replace(szOld,szNew,bReplaceAll);
		return s.c_str();

	}
	registrator reg_replace(StrReplace,szCategory,"StrReplace", "Returns a string with occurrences of one substring replaced by another substring");

	//��������� ��� ������� � ������� �������
	EXPORT cstr WINAPI StrToUpper(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.toupper().c_str();
	}
	registrator reg_toupper(StrToUpper,szCategory,"StrToUpper","Converts string to upper case");

	//��������� ��� ������� � ������ �������
	EXPORT cstr WINAPI StrToLower(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.tolower().c_str();
	}
	registrator reg_tolower(StrToLower,szCategory, "StrToLower","Converts string to lower case");

	//���������� true, ���� ��� ����� � ������� ��������
	EXPORT bool WINAPI StrIsUpper(cstr szSrc){
		STRING_IMPL(szSrc);
		for (size_t i=0; i<s.length();++i)
			if (IsCharAlpha(s[i]))
				if (!IsCharUpper(s[i]))
					return false;
		return true;
	}
	registrator reg_isupper(StrIsUpper,szCategory,"StrIsUpper","Checks if all characters in the specified string are written in upper case");

	//���������� true, ���� ��� ����� � ������� ��������
	EXPORT bool WINAPI StrIsLower(cstr szSrc){
		STRING_IMPL(szSrc);
		for (size_t i=0; i<s.length();++i)
			if (IsCharAlpha(s[i]))
				if (!IsCharLower(s[i]))
					return false;
		return true;
	}
	registrator reg_islower(StrIsLower,szCategory,"StrIsLower","Checks if all characters in the specified string are written in lower case");

	//������������ �������� (�.�. �����). � ����� ������ �������� ������ 
	//�� ��������, ������� �������������� � ����� �������
	EXPORT cstr WINAPI StrIntersect(cstr szSrc, cstr szValue){
		STRING_IMPL(szSrc);
		return s.intersect(szValue).c_str();
	}
	registrator reg_intersect(StrIntersect,szCategory,"StrIntersect","Returns string, containing only characters present in both input strings");

	//����������� �������� (�.�. �����). 
	EXPORT cstr WINAPI StrUnion(cstr szSrc, cstr szValue){
		STRING_IMPL(szSrc);
		return s.association(szValue).c_str();
	}
	registrator reg_union(StrUnion,szCategory,"StrUnion","Returns the string which includes characters from both input strings");

	//��������� ��������	
	EXPORT cstr WINAPI StrSubstract(cstr szSrc, cstr szValue){
		STRING_IMPL(szSrc);
		return s.substract(szValue).c_str();
	}
	registrator reg_substract(StrSubstract,szCategory,"StrSubstract","Deletes from first string all characters that present in second string");

	//��������������  AnsiToOem
	EXPORT cstr WINAPI StrAnsiToOem(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.ansi2oem().c_str();
	}
	registrator reg_ansi2oem(StrAnsiToOem,szCategory,"StrAnsiToOem","Translates a specified string into the OEM-defined character set");

	//��������������  OemToAnsi
	EXPORT cstr WINAPI StrOemToAnsi(cstr szSrc){
		STRING_IMPL(szSrc);
		return s.oem2ansi().c_str();
	}
	registrator reg_oem2ansi(StrOemToAnsi,szCategory,"StrOemToAnsi",  "Translates a string from the OEM-defined character set into an ANSI or a string");

	//��������������  String->int 
	EXPORT bool WINAPI StrToInt(cstr szSrc, int & result){
		if (!str_is_of_alphabet(szSrc,"-+0123456789"))
			return false;
		result = atoi(szSrc);
		return true;
	}
	registrator reg_to_int(StrToInt,szCategory,"StrToInt",  "Converts a string to integer value, if succeeds returns true and the number");

	//��������������  String->int 
	EXPORT int WINAPI StrToIntDef(cstr szSrc, int nDef){
		int nRes; 
		if (!StrToInt(szSrc,nRes))
			nRes = nDef;
		return nRes;
	}
	registrator reg_to_int_def(StrToIntDef,szCategory,"StrToIntDef",  "Converts a string to integer value, if fails returns default value");

	//��������������  String(Hex)->int 
	EXPORT bool WINAPI StrHexToInt(cstr szSrc, int & nRes){
		if (szSrc==NULL)
			return 0;
		if (*szSrc=='$')
			szSrc++;
		else if ((*szSrc=='0') && (oneof(szSrc[1],'x','X')))
			szSrc+=2;

		if (!str_is_of_alphabet(szSrc,"0123456789abcdefABCDEF"))
			return false;

		nRes = ahtoi(szSrc);
		return true;
	}
	registrator reg_hex_to_int(StrHexToInt,szCategory,"StrHexToInt",  "Converts a string that is in hexadecimal notation, to integer value. String can start either from digits (0-9,a-f,A-F) or from prefix 0x or $. If fails, returns false");

	//��������������  String(Hex)->int 
	EXPORT int WINAPI StrHexToIntDef(cstr szSrc, int nDef){
		int nRes;
		if (!StrHexToInt(szSrc,nRes))
			nRes=nDef;
		return nRes;
	}
	registrator reg_hex_to_int_def(StrHexToIntDef,szCategory,"StrHexToIntDef",  "Converts a string that is in hexadecimal notation, to integer value. String can start either from digits (0-9,a-f,A-F) or from prefix 0x or $. If fails, returns default value");

	//��������������  String(Bin)->int 
	EXPORT bool WINAPI StrBinToInt(cstr szSrc, int & nRes){
		if (szSrc==NULL)
			return false;

		string_smart s(szSrc);
		if (oneof(s.get_last(),'b','B'))
			s.del_last();

		if (!str_is_of_alphabet(s.c_str(),"01"))
			return false;

		nRes = abtoi(s);
		return true;
	}
	registrator reg_Bin_to_int(StrBinToInt,szCategory,"StrBinToInt",  "Converts a string that is in binary notation, to integer value. String can consist of digits (0,1) and can end with symbol b. If function fails, it returns false");

	//��������������  String(Bin)->int 
	EXPORT int WINAPI StrBinToIntDef(cstr szSrc, int nDef){
		int nRes;
		if (!StrBinToInt(szSrc,nRes))
			nRes=nDef;
		return nRes;
	}
	registrator reg_Bin_to_int_def(StrBinToIntDef,szCategory,"StrBinToIntDef",  "Converts a string that is in binary notation, to integer value. String can consist of digits (0,1) and can end with symbol b. If function fails, it returns default value");

	//��������������  String->double
	EXPORT bool WINAPI StrToDouble(cstr szSrc, double_proxy & nRes){
		//��������� ����������� ������
		if (!str_is_of_alphabet(szSrc,"+-0123456789eE."))
			return false;
		
		//���������� � �� ������������� ���������� ������ �� ����������� ������
		//����� ���� ������������ ��������
		nRes = double_proxy::from_double((atof(szSrc)));
		return true;
	}
	registrator reg_to_double(StrToDouble,szCategory,"StrToDouble",  "Converts a string to double value");

	//��������������  String->double
	EXPORT double_proxy WINAPI StrToDoubleDef(cstr szSrc, double_proxy nDef){
		double_proxy nRes;
		if (!StrToDouble(szSrc,nRes))
			nRes = nDef;
		return nRes;
	}
	registrator reg_to_double_def(StrToDoubleDef,szCategory,"StrToDoubleDef",  "Converts a string to double value");
};