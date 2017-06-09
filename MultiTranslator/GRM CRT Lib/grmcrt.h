#include <map>
#include <BaseUtils.h>
#include <MultiThreadUtils.h>
#include <GrmCrtLibExports.h>

using namespace base_utils;
using namespace std;
using namespace grm_structs;
using namespace grmcrtlib_exports;

/*
Нижеприведенные функции работают со строками. Один нюанс - 
так как функции могут возвращать только char* (это ограничение спецификации CRT-библиотек
для мультитранслятора - можно возвращать только простые типы), то использование string
в качестве возвращаемогно значения невозможно. Поэтому, чтобы решить проблему с выделением/освобождением памяти,
было решено использовать статические string_smart в каждой из функций
для избежания проблем с многопоточностью ипользуйте макрос STRING_IMPL, который
синхронизирует очередность вызовов с помощью критической секции
*/


#define STRING_IMPL(x) static string_smart s;\
	static thread_utils::critical_section cs;\
	str_assign(s,x,cs);

void str_assign(string_smart & str, cstr sz, thread_utils::critical_section & cs);

//В связи с особенностями передачи данных через стек
//нам необходимо "отцеплять" текстовый буфер от variant, т.к. он автоматически 
//уничтожается на стороне вызывающего
void finalize_variant(variant & var);

//for type double. Wee need to present it like 8 bytes of struct. Do not use double as a return value!!!!
struct double_proxy{
	DWORD hi;
	DWORD low;
	//Не использовать операторы и конструкторы. Что-то подглючивает
	static double to_double(double_proxy f){
		return *((double*)(&f));
	}
	static double_proxy from_double(double f){
		return *((double_proxy*)(&f));
	}
};

//------------------------------------------------------------------------
// class registrator
//------------------------------------------------------------------------
class registrator
{
public:
	typedef vector<arg_data> args;
	
	struct arg_data:public grm_structs::arg_data{

		arg_data(int VarType, int PassType)
		{
			m_VarType = var_type(VarType);
			m_PassType = pass_type(PassType);
		}
	};

	struct func_data{
		arg_data	m_RetValue;
		string_std	m_strFuncName;
		args		m_lstArgs;	
		string_std	m_strCategory;
		string_std	m_strDescription;
		
		func_data (cstr szCategory, cstr szFuncName, cstr szDesc, arg_data RetVal, const args & Args)
			: m_strCategory(szCategory),m_strFuncName(szFuncName),m_strDescription(szDesc), m_RetValue(RetVal),m_lstArgs(Args){
		}
	};

	typedef vector<func_data> lst_funcs;

protected:
	static lst_funcs * m_pRegisteredFuncs;
	static lst_funcs & list(){
		if (m_pRegisteredFuncs==NULL)
			m_pRegisteredFuncs = new lst_funcs;
		return * m_pRegisteredFuncs;
	}
	template<class T>
	struct arg_traits;
	
	template<> struct arg_traits<bool>		{enum { type = at_bool};	enum { pass = pt_byval};};
	template<> struct arg_traits<char>		{enum { type = at_char};	enum { pass = pt_byval};};
	template<> struct arg_traits<int>		{enum { type = at_int};		enum { pass = pt_byval};};
	template<> struct arg_traits<int&>		{enum { type = at_int};		enum { pass = pt_byref};};
	template<> struct arg_traits<long>		{enum { type = at_int};		enum { pass = pt_byval};};
	template<> struct arg_traits<size_t>	{enum { type = at_int};		enum { pass = pt_byval};};
	template<> struct arg_traits<double_proxy> {enum { type = at_double};	enum { pass = pt_byval};};
	template<> struct arg_traits<double_proxy&> {enum { type = at_double};	enum { pass = pt_byref};};
	template<> struct arg_traits<double>	{enum { type = at_double};	enum { pass = pt_byval};};
	template<> struct arg_traits<cstr>		{enum { type = at_string};	enum { pass = pt_byval};};
	template<> struct arg_traits<str>		{enum { type = at_string};	enum { pass = pt_byval};};
	template<> struct arg_traits<variant>	{enum { type = at_variant};	enum { pass = pt_byval};};
	template<> struct arg_traits<variant&>	{enum { type = at_variant};	enum { pass = pt_byref};};

	registrator(){}
public:
	//Коструктор для метода класса
	template <class TRet> 
	registrator(TRet (WINAPI *pClF)(),cstr szCategory, cstr szFuncName, cstr szDesc){
		list().push_back(func_data(szCategory,szFuncName,szDesc,arg_data(arg_traits<TRet>::type,arg_traits<TRet>::pass), args()));
	}
	
	template <class TRet, class TArg1> 
	registrator(TRet (WINAPI *pClF)(TArg1),cstr szCategory, cstr szFuncName, cstr szDesc){
		args a; 
		a.push_back(arg_data(arg_traits<TArg1>::type,arg_traits<TArg1>::pass));
		list().push_back(func_data(szCategory,szFuncName,szDesc, arg_data(arg_traits<TRet>::type,arg_traits<TRet>::pass), a));
	}

	template <class TRet, class TArg1, class TArg2> 
	registrator(TRet (WINAPI *pClF)(TArg1,TArg2),cstr szCategory, cstr szFuncName, cstr szDesc){
		args a; 
		a.push_back(arg_data(arg_traits<TArg1>::type,arg_traits<TArg1>::pass));
		a.push_back(arg_data(arg_traits<TArg2>::type,arg_traits<TArg2>::pass));
		list().push_back(func_data(szCategory,szFuncName,szDesc, arg_data(arg_traits<TRet>::type,arg_traits<TRet>::pass), a));
	}
	
	template <class TRet, class TArg1, class TArg2, class TArg3> 
	registrator(TRet (WINAPI *pClF)(TArg1,TArg2,TArg3),cstr szCategory, cstr szFuncName, cstr szDesc){
		args a; 
		a.push_back(arg_data(arg_traits<TArg1>::type,arg_traits<TArg1>::pass));
		a.push_back(arg_data(arg_traits<TArg2>::type,arg_traits<TArg2>::pass));
		a.push_back(arg_data(arg_traits<TArg3>::type,arg_traits<TArg3>::pass));
		list().push_back(func_data(szCategory,szFuncName,szDesc, arg_data(arg_traits<TRet>::type,arg_traits<TRet>::pass), a));
	}

	template <class TRet, class TArg1, class TArg2, class TArg3, class TArg4> 
	registrator(TRet (WINAPI *pClF)(TArg1,TArg2,TArg3, TArg4),cstr szCategory, cstr szFuncName, cstr szDesc){
		args a; 
		a.push_back(arg_data(arg_traits<TArg1>::type,arg_traits<TArg1>::pass));
		a.push_back(arg_data(arg_traits<TArg2>::type,arg_traits<TArg2>::pass));
		a.push_back(arg_data(arg_traits<TArg3>::type,arg_traits<TArg3>::pass));
		a.push_back(arg_data(arg_traits<TArg4>::type,arg_traits<TArg4>::pass));
		list().push_back(func_data(szCategory,szFuncName,szDesc, arg_data(arg_traits<TRet>::type,arg_traits<TRet>::pass), a));
	}

	template <class TRet, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5> 
	registrator(TRet (WINAPI *pClF)(TArg1,TArg2,TArg3, TArg4, TArg5),cstr szCategory, cstr szFuncName, cstr szDesc){
		args a; 
		a.push_back(arg_data(arg_traits<TArg1>::type,arg_traits<TArg1>::pass));
		a.push_back(arg_data(arg_traits<TArg2>::type,arg_traits<TArg2>::pass));
		a.push_back(arg_data(arg_traits<TArg3>::type,arg_traits<TArg3>::pass));
		a.push_back(arg_data(arg_traits<TArg4>::type,arg_traits<TArg4>::pass));
		a.push_back(arg_data(arg_traits<TArg5>::type,arg_traits<TArg5>::pass));
		list().push_back(func_data(szCategory,szFuncName,szDesc, arg_data(arg_traits<TRet>::type,arg_traits<TRet>::pass), a));
	}

	//Перебор
	static size_t count(){
		return list().size();
	}
	
	static const func_data & get_at(size_t nIndex){
		return list()[nIndex];
	}

};
