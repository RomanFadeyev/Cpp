/********************************************************************
	created:	2002/10/09
	created:	9:10:2002	21:16
	filename:	$(Projects)\sources\grmexecexports.h
	file path:	$(Projects)\sources
	file base:	grmexecexports
	file ext:	h
	author:		Fadeyev	R.V.
	
	purpose:	
*********************************************************************/
#include <BaseUtils.h>
#include <OutStreams.h>
#include <ExitCodes.h>
#include <GrmKrnlExports.h>

#pragma	once
using namespace	base_utils;
using namespace	out_streams;
using namespace	exit_codes;

#ifdef _GRM_EXEC_
  #ifdef _USRDLL 
	#pragma	message("\n --- Exporting GrmExec functions... ---\n")  
	#define	GRM_EXEC_DLL_EXPORT	 _declspec(dllexport)
  #else
	#define	GRM_EXEC_DLL_EXPORT
  #endif
#else
  #define GRM_EXEC_DLL_EXPORT  _declspec(dllimport)
#endif

namespace grmexec_exports
{
	//Дополнительный коды для формирования номера строки в DebugInfo
	enum {srclnNoNumber=-1, srclnSameAsPrevious=-2};

	//Структура для посылки данных о текущей команде
	struct step_info
	{
		cstr   m_szSource;
		size_t m_nLine;
		size_t m_nScopeLevel;
		bool   m_bForcedStop;

		step_info():m_nLine(0), m_nScopeLevel(0),m_bForcedStop(false),m_szSource(NULL)
		{}
		step_info(cstr szSource, size_t nLine, size_t nScopeLevel, bool bForcedStop=false):
			m_szSource(szSource), m_nLine(nLine), m_nScopeLevel(nScopeLevel), m_bForcedStop(bForcedStop)
		{}
	};

	// Функтор void	(cstr szModuleName,size_t nLine, size_t nStackSize)
	typedef functors::functor<void,ARGS_1(const step_info &)> step_functor;
	
	//Структура, описывающая свойства 
	enum get_oi_result{goirOK, goirNotFound, goirUnknownError};	
	enum get_oi_filter{goifLocalVars, goifAutoVars, goifStack};	
	class object_info
	{
	public:
		virtual ~object_info(){};
		enum kind{oikSimple, oikStruct, oikArray,oikFunction};
		virtual cstr	GetName()=0;	//Для функции - это название функции	
		virtual cstr	GetContext()=0;	//Для функции - это название модуля
		virtual kind	GetKind()=0;	
		virtual const variant &GetValue()=0;//Передавать можно только по ссылке!!!
										//Для структуры этот метод возвращает пустую величину
										//для массивов - первый элемент
										//для функции - это адрес, где она остановилась в CallStack
		virtual size_t	ItemCount()=0;	//Для структур и для массивов
										//Для функции ничего не значит
		virtual object_info& GetItem(size_t & nIndex)=0;	
										//для массивов GetItem(0)==GetValue
										//для функции не определено
	};

	interface IGrmExec: public IUnknown
	{	
		//Загрузка исполняемого	файла
		virtual	ERunnerExitCodes STDMETHODCALLTYPE Load(cstr szFileName) PURE;
		//Запуск выполнения. Только	после Load!
		virtual	ERunnerExitCodes STDMETHODCALLTYPE Run(cstr szParams,int & iExitCode) PURE;
		//Назначение функции отображения процесса разбора. Является передатчиком в другую DLL
		virtual	bool STDMETHODCALLTYPE SetParseCallBck(const grmkrnl_exports::parse_info_functor & Func) PURE;
		//Назначение функции отображения процесса генерации текста. Является передатчиком в другую DLL
		virtual	bool STDMETHODCALLTYPE SetTextGenCallBck(const grmkrnl_exports::gen_info_functor  & Func) PURE;
		//Изменения	потока вывода
		virtual	void STDMETHODCALLTYPE SetExternalMessager(const out_streams::stream_functor & Func) PURE;
		//Запуск выполнения	c отладочной информацией. Только после Load!
		virtual	ERunnerExitCodes STDMETHODCALLTYPE Debug(cstr szParams,	int	& iExitCode, const step_functor & CallbackFunc) PURE;
		//Проверка указанной точки останова	на возможность там остановится
		virtual	bool STDMETHODCALLTYPE CheckBreakPointOnUsed(cstr	szModulePath, size_t dwLine) PURE;
		//Получение информации о именах всех доступных объектов указанного типа. Возвр строка, каждый элемент в которой отделяется '\n'
		virtual	string_local_alloc STDMETHODCALLTYPE GetObjectNames(get_oi_filter filter) PURE;
		//Получение информации о указанном объекте в данном контексте отладки (пока только для переменных)
		virtual	object_info* STDMETHODCALLTYPE GetObjectInfo(cstr	szObjectName, get_oi_result & result) PURE;
		//Освобождение полученного объекта с информацией
		virtual	void STDMETHODCALLTYPE ReleaseObjectInfo(object_info* pInfo) PURE;
	};

	//Прототипы	указателей на функции
	typedef void (WINAPI *pf_OnItemParseCallBack) (size_t nID,cstr szText,void * pTranslator);	
	typedef ERunnerExitCodes (WINAPI *pf_Execute) (cstr szFileName, cstr szParams, pf_OnItemParseCallBack pCallBack, int &iExitCode);
	typedef	IGrmExec* (*pf_GetGrmExecInterface) ();

	//Названия функций
	const_str szf_Execute					= "Execute";
	const_str szf_GetGrmExecInterface		= "GetGrmExecInterface";

	//Прототипы	функций
	extern "C"
	{
		#pragma warning (disable: 4190)
		//Функция для клиентов. Использует только простые типы данных, использует stdcall соглашение
		//Эту функцию вызывают в основном самодельные программы-трансляторы
		GRM_EXEC_DLL_EXPORT	ERunnerExitCodes WINAPI Execute(cstr szFileName, cstr szParams, pf_OnItemParseCallBack pCallBack, int &iExitCode);
		GRM_EXEC_DLL_EXPORT	IGrmExec * WINAPI GetGrmExecInterface();
	}
}		
