/********************************************************************
	created:	2002/10/04
	created:	4:10:2002   20:52
	filename: 	$(Projects)\sources\grm executor\stdlib\engineutils.cpp
	file path:	$(Projects)\sources\grm executor\stdlib
	file base:	engineutils
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Набор различных утилитных функций для интерпретатора команд
*********************************************************************/

#include "stdafx.h"
#include "EngineUtils.h"

namespace engine_utils
{
	string_smart gen_parse_error_msg(cstr szErrMsg, DWORD dwLine, DWORD dwPos, bool bPossible, cstr szErrorHubName)
	{
		string_smart res;
		if (!bPossible){
			if ((!szErrMsg) || (!*szErrMsg)){
				if ((!szErrorHubName) || (!*szErrorHubName))
					res=Format("Syntax error occured at position (%d, %d)", dwLine, dwPos);
				else
					res=Format("Syntax error occured at position (%d, %d), rule: %s", dwLine, dwPos,szErrorHubName);
			}
			else{
				if ((!szErrorHubName) || (!*szErrorHubName))
					res=Format("Syntax error occured at position (%d, %d). The message is:\n\n%s.\n", dwLine, dwPos,szErrMsg);
				else
					res=Format("Syntax error occured at position (%d, %d). The message is:\n\n%s.\n\nRule(s), where the error was found, is:\n\n%s", dwLine, dwPos,szErrMsg,szErrorHubName);
			}
		}
		else{
			if ((!szErrMsg) || (!*szErrMsg)){
				if ((!szErrorHubName) || (!*szErrorHubName))
					res=Format("Possible syntax error at position (%d, %d)", dwLine, dwPos);
				else
					res=Format("Possible syntax error at position (%d, %d), rule(s): %s", dwLine, dwPos,szErrorHubName);
			}
			else{
				if ((!szErrorHubName) || (!*szErrorHubName))
					res=Format("Possible syntax error '%s' at position (%d, %d)", szErrMsg,dwLine, dwPos);
				else
					res=Format("Possible syntax error '%s' at position (%d, %d), rule(s): %s", szErrMsg,dwLine, dwPos,szErrorHubName);
			}
		}	
		return res;
	}

	#define ASSIGN_OP(val,cast,opertr,operand,operand2) \
		switch ((static_cast<variant&>(val)).get_type())\
		{\
			case variant::vtBOOL:	val=(bool)   ((cast (val.as_bool())) ## opertr ## (cast (operand))); break;\
			case variant::vtCHAR:	val=(char_t) ((cast (val.as_char())) ## opertr ## (cast (operand))); break;\
			case variant::vtSMALL:	val=(byte)	 ((cast (val.as_small())) ## opertr ## (cast (operand))); break;\
			case variant::vtSHORT:	val=(short)	 ((cast (val.as_short())) ## opertr ## (cast (operand))); break;\
			case variant::vtINT:	val=(int)	 ((cast (val.as_int())) ## opertr ## (cast (operand))); break;\
			case variant::vtDOUBLE:	val=(double) ((cast (val.as_double())) ## opertr ## (cast (operand2))); break;\
			default:\
				_ASSERT(false);\
		}
	#define NOP
	
	void inc_variant(variant & val)
	{
		if (val.get_type()==variant::vtSTRING){
			val.cast(variant::vtINT);
			val = (int) val.as_int()+1;
			val.cast(variant::vtSTRING);
		}
		else
		  ASSIGN_OP(val,NOP,+,1,1.0);
	}
	//
	void dec_variant(variant & val)
	{
		if (val.get_type()==variant::vtSTRING){
			val.cast(variant::vtINT);
			val = (int) val.as_int()-1;
			val.cast(variant::vtSTRING);
		}
		else
		  ASSIGN_OP(val,NOP,-,1,1.0);
	}	
	//
	void add_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
			val = string_smart(val.as_cstr(),arg.as_cstr());
		else
		  ASSIGN_OP(val,NOP, +,arg.as_int(),arg.as_double());
	}
	//
	void mul_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
			;
		else
		  ASSIGN_OP(val,NOP, *,arg.as_int(),arg.as_double());
	}
	//
	void sub_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
			//Вычитание множеств	
			val = val.as_string().substract(arg.as_cstr());
		else
			ASSIGN_OP(val,NOP, -,arg.as_int(),arg.as_double());
	}
	//
	#pragma warning(disable:4804)
	void div_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
			;
		else
			ASSIGN_OP(val,NOP, /,arg.as_int(),arg.as_double());
	}
	//
	void rmndr_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
			;
		else
			ASSIGN_OP(val,int,%,arg.as_int(),arg.as_double());
	}
	//
	void shl_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
		//Удаляем самый левый символ
			val = val.as_string().del(0);
		else
			ASSIGN_OP(val,int,<<,arg.as_int(),arg.as_double());
	}
	//
	void shr_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
		//Вставляем слева пробел
			val = val.as_string().insert(32,0);
		else
			ASSIGN_OP(val,int,>>,arg.as_int(),arg.as_double());
	}
	//
	void and_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
		//пересеченине множеств
			val = val.as_string().intersect(arg.as_cstr());
		else
			ASSIGN_OP(val,int,&,arg.as_int(),arg.as_double());
	}
	void xor_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
			;
		else
			ASSIGN_OP(val,int,^,arg.as_int(),arg.as_double());
	}
	//
	void or_assign_variant(variant & val, variant & arg){
		if (val.get_type()==variant::vtSTRING)
		//объединение множеств
			val = val.as_string().association(arg.as_cstr());
		else
			ASSIGN_OP(val,int,|,arg.as_int(),arg.as_double());
	}
	
	#define OP(val1,val2,cast,opertr) \
		switch (variant::cast_of(val1.get_type(),val2.get_type()))\
		{\
			case variant::vtBOOL:	return (bool)((cast (val1.as_bool())) ## opertr ## (cast (val2.as_bool())));\
			case variant::vtCHAR:	return (char)((cast (val1.as_char())) ## opertr ## (cast (val2.as_char())));\
			case variant::vtSMALL:	return (byte)((cast (val1.as_small())) ## opertr ## (cast (val2.as_small())));\
			case variant::vtSHORT:	return (short)((cast (val1.as_short())) ## opertr ## (cast (val2.as_short())));\
			case variant::vtINT:	return (int)((cast (val1.as_int())) ## opertr ## (cast (val2.as_int())));\
			case variant::vtDOUBLE:	return (double)((cast (val1.as_double())) ## opertr ## (cast (val2.as_double())));\
			default:\
				_ASSERT(false);return (long)0;\
		}
	
	variant and_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,int,&);		
	}
	variant or_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,int,|);		
	}
	variant xor_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,int,^);		
	}
	variant shl_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,int,<<);		
	}
	variant shr_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,int,>>);		
	}
	variant add_variant(variant & val1, variant & val2){
		if ((val1.isstring()) || (val2.isstring()))
			return string_smart(val1.as_cstr(),val2.as_cstr());
		else
			OP(val1,val2,NOP,+);		
	}
	variant sub_variant(variant & val1, variant & val2){		
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,NOP,-);		
	}
	variant mul_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else {
			double x1 = val1.as_double();
			double x2 = val2.as_double();
			x1 = x1*x2;
			OP(val1,val2,NOP,*);		
		}
			
	}
	variant div_variant(variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,NOP,/);		
	}
	variant rmndr_variant (variant & val1, variant & val2){
		if (val1.get_type()==variant::vtSTRING)
			return val1;
		else
			OP(val1,val2,int,%);		
	}

}
