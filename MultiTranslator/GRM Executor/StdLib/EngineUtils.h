/********************************************************************
	created:	2002/10/04
	created:	4:10:2002   20:50
	filename: 	$(Projects)\sources\grm executor\stdlib\engineutils.h
	file path:	$(Projects)\sources\grm executor\stdlib
	file base:	engineutils
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Набор различных утилитных функций для интерпретатора команд
*********************************************************************/

#include <BaseUtils.h>

#pragma once

using namespace base_utils;

namespace engine_utils
{
	//по указанным признакам ошибки генерирует текст
	//szErrMsg - текст ошибки, может быть NULL
	string_smart gen_parse_error_msg(cstr szErrMsg, DWORD dwLine, DWORD dwPos, bool bPossible, cstr szErrorHubName);

	//Выполняет операцию инкремента для типа variant
	void inc_variant(variant & val);
	//Выполняет операцию декремента для типа variant
	void dec_variant(variant & val);

	void add_assign_variant(variant & val, variant & arg);
	void mul_assign_variant(variant & val, variant & arg);
	void sub_assign_variant(variant & val, variant & arg);
	void div_assign_variant(variant & val, variant & arg);
	void rmndr_assign_variant(variant & val, variant & arg);
	void shl_assign_variant(variant & val, variant & arg);
	void shr_assign_variant(variant & val, variant & arg);
	void and_assign_variant(variant & val, variant & arg);
	void xor_assign_variant(variant & val, variant & arg);
	void or_assign_variant (variant & val, variant & arg);

	variant and_variant(variant & val1, variant & val2);
	variant or_variant(variant & val1, variant & val2);
	variant xor_variant(variant & val1, variant & val2);
	variant shl_variant(variant & val1, variant & val2);
	variant shr_variant(variant & val1, variant & val2);
	variant add_variant(variant & val1, variant & val2);
	variant sub_variant(variant & val1, variant & val2);		
	variant mul_variant(variant & val1, variant & val2);
	variant div_variant(variant & val1, variant & val2);
	variant rmndr_variant(variant & val1, variant & val2);
}