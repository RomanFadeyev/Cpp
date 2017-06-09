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
	//�������������� ���� ��� ������������ ������ ������ � DebugInfo
	enum {srclnNoNumber=-1, srclnSameAsPrevious=-2};

	//��������� ��� ������� ������ � ������� �������
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

	// ������� void	(cstr szModuleName,size_t nLine, size_t nStackSize)
	typedef functors::functor<void,ARGS_1(const step_info &)> step_functor;
	
	//���������, ����������� �������� 
	enum get_oi_result{goirOK, goirNotFound, goirUnknownError};	
	enum get_oi_filter{goifLocalVars, goifAutoVars, goifStack};	
	class object_info
	{
	public:
		virtual ~object_info(){};
		enum kind{oikSimple, oikStruct, oikArray,oikFunction};
		virtual cstr	GetName()=0;	//��� ������� - ��� �������� �������	
		virtual cstr	GetContext()=0;	//��� ������� - ��� �������� ������
		virtual kind	GetKind()=0;	
		virtual const variant &GetValue()=0;//���������� ����� ������ �� ������!!!
										//��� ��������� ���� ����� ���������� ������ ��������
										//��� �������� - ������ �������
										//��� ������� - ��� �����, ��� ��� ������������ � CallStack
		virtual size_t	ItemCount()=0;	//��� �������� � ��� ��������
										//��� ������� ������ �� ������
		virtual object_info& GetItem(size_t & nIndex)=0;	
										//��� �������� GetItem(0)==GetValue
										//��� ������� �� ����������
	};

	interface IGrmExec: public IUnknown
	{	
		//�������� ������������	�����
		virtual	ERunnerExitCodes STDMETHODCALLTYPE Load(cstr szFileName) PURE;
		//������ ����������. ������	����� Load!
		virtual	ERunnerExitCodes STDMETHODCALLTYPE Run(cstr szParams,int & iExitCode) PURE;
		//���������� ������� ����������� �������� �������. �������� ������������ � ������ DLL
		virtual	bool STDMETHODCALLTYPE SetParseCallBck(const grmkrnl_exports::parse_info_functor & Func) PURE;
		//���������� ������� ����������� �������� ��������� ������. �������� ������������ � ������ DLL
		virtual	bool STDMETHODCALLTYPE SetTextGenCallBck(const grmkrnl_exports::gen_info_functor  & Func) PURE;
		//���������	������ ������
		virtual	void STDMETHODCALLTYPE SetExternalMessager(const out_streams::stream_functor & Func) PURE;
		//������ ����������	c ���������� �����������. ������ ����� Load!
		virtual	ERunnerExitCodes STDMETHODCALLTYPE Debug(cstr szParams,	int	& iExitCode, const step_functor & CallbackFunc) PURE;
		//�������� ��������� ����� ��������	�� ����������� ��� �����������
		virtual	bool STDMETHODCALLTYPE CheckBreakPointOnUsed(cstr	szModulePath, size_t dwLine) PURE;
		//��������� ���������� � ������ ���� ��������� �������� ���������� ����. ����� ������, ������ ������� � ������� ���������� '\n'
		virtual	string_local_alloc STDMETHODCALLTYPE GetObjectNames(get_oi_filter filter) PURE;
		//��������� ���������� � ��������� ������� � ������ ��������� ������� (���� ������ ��� ����������)
		virtual	object_info* STDMETHODCALLTYPE GetObjectInfo(cstr	szObjectName, get_oi_result & result) PURE;
		//������������ ����������� ������� � �����������
		virtual	void STDMETHODCALLTYPE ReleaseObjectInfo(object_info* pInfo) PURE;
	};

	//���������	���������� �� �������
	typedef void (WINAPI *pf_OnItemParseCallBack) (size_t nID,cstr szText,void * pTranslator);	
	typedef ERunnerExitCodes (WINAPI *pf_Execute) (cstr szFileName, cstr szParams, pf_OnItemParseCallBack pCallBack, int &iExitCode);
	typedef	IGrmExec* (*pf_GetGrmExecInterface) ();

	//�������� �������
	const_str szf_Execute					= "Execute";
	const_str szf_GetGrmExecInterface		= "GetGrmExecInterface";

	//���������	�������
	extern "C"
	{
		#pragma warning (disable: 4190)
		//������� ��� ��������. ���������� ������ ������� ���� ������, ���������� stdcall ����������
		//��� ������� �������� � �������� ����������� ���������-�����������
		GRM_EXEC_DLL_EXPORT	ERunnerExitCodes WINAPI Execute(cstr szFileName, cstr szParams, pf_OnItemParseCallBack pCallBack, int &iExitCode);
		GRM_EXEC_DLL_EXPORT	IGrmExec * WINAPI GetGrmExecInterface();
	}
}		
