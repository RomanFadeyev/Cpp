/********************************************************************
	created:	2002/10/02
	created:	2:10:2002   18:22
	filename: 	$(Projects)\sources\grm executor\interpreter.cpp
	file path:	$(Projects)\sources\grm executor
	file base:	interpreter
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Интерпретатор кода
*********************************************************************/

#include "stdafx.h"
#include <MiscUtils.h>

#include "Interpreter.h"

#define DBG_STATE_RUN		0
#define DBG_STATE_PAUSE		1
#define DBG_STATE_STOP		2
#define DBG_STATE_1STEP		3

CInterpreter*	CInterpreter::pCurCode=NULL;

// Индексы можно проверить в Lexicon::Compile (добавление переменных)
#define PARAM_COUNT_IDX			0
#define PARAM_STR				1
#define SOURCE_FILE_NAME_IDX	2
#define DEST_FILE_NAME_IDX		3
#define DELIMITERS_IDX			4		

//Исключение, генерируемое интерпретатором
class runtime_exception : public exception{
public:
	runtime_exception(cstr szText): exception(szText){}
};

void msg_box(const string_smart & strText){
	::MessageBox(0,strText.c_str(),"Runtime error",0);
}

#define THROW_RT_ERROR(szMsg) throw(runtime_exception(szMsg))
#define GEN_RT_ERROR1(szMsg) msg_box(string_smart(szMsg))
#define GEN_RT_ERROR2(szMsg1,szMsg2) msg_box(string_smart(szMsg1,szMsg2))
#define GEN_RT_ERROR3(szMsg1,szMsg2,szMsg3) msg_box(string_smart(szMsg1,szMsg2,szMsg3))
#define GEN_RT_ERROR4(szMsg1,szMsg2,szMsg3,szMsg4) msg_box(string_smart(szMsg1,szMsg2,szMsg3,szMsg4))
#define GEN_RT_ERROR5(szMsg1,szMsg2,szMsg3,szMsg4,szMsg5) msg_box(string_smart(szMsg1,szMsg2,szMsg3,szMsg4,szMsg5))
#define GEN_RT_ERROR6(szMsg1,szMsg2,szMsg3,szMsg4,szMsg5,szMsg6) msg_box(string_smart(szMsg1,szMsg2,szMsg3,szMsg4,szMsg5,szMsg6))


CInterpreter::CInterpreter():m_fStepCallBack()
{
	pCurCode=this;
	m_bParsing=false;
}
//------------------------------------------------------------------------
CInterpreter::~CInterpreter()
{

}
//------------------------------------------------------------------------
void CInterpreter::SubRoutineHandler(DWORD SubRoutineAddr)
{
	if (SubRoutineAddr==0)	//Это прием оптимизации в компиляторе
		return;				//он выкидывает заведомо пустые функции и 
							//заменяет их подобным вызовом

	DWORD CommandPointer;
	pCurCode->m_LocalStack.PushFrame();
	pCurCode->m_DeclaredVariables.PushFrame();

	pCurCode->RunFromTo(SubRoutineAddr,RET_S,CommandPointer);

	pCurCode->m_DeclaredVariables.PopFrame();	// destroy superfluous levels
	pCurCode->m_LocalStack.PopFrame();		//	
}
//------------------------------------------------------------------------
void CInterpreter::ClearInternalStructures()
{
	//стек адресов
	m_AddrStack.Clear();	
	//общий стек
	m_DataStack.Clear();		
	//все обявленные переменные на разных уровнях видимости
	m_DeclaredVariables.Clear();	
	//локальный стек в пределах области видимости
	m_LocalStack.Clear();	
	//Список библиотек
	m_LibraryManager.clear();
	//Контейнер
	m_Container.clear();
}
//------------------------------------------------------------------------
void CInterpreter::Clear()
{
	m_Code.Clear();
}
//------------------------------------------------------------------------
void CInterpreter::SetStepCallback(grmexec_exports::step_functor fStepCallback){
	m_fStepCallBack=fStepCallback;
}
//---------------------------------------------------------------------
ERunnerExitCodes CInterpreter::Load(cstr szFileName)
{
	//Очищаем импортируемые функции
	m_ImportFuncs.clear();

	ERunnerExitCodes ret = m_Code.Load(szFileName);
	
	//Загружаем все требуемые импортируемые функции
	if (ret==recOK) {
		string_list strAuxDir = extract_filedir(szFileName);
		m_ImportFuncs.reserve(m_Code.GetImportFuncCount());

		for (size_t i=0; i<m_Code.GetImportFuncCount();i++){
			const grm_structs::func_data & fdata = m_Code.GetImportFunc(i);
			imp_func_data f(fdata,strAuxDir);
			if (!f.m_Func){ 
				//26/03/2005 Попробуем еще поискать по известным источникам
				grm_structs::func_data fdata2 = fdata;
				strcpy(fdata2.m_szLibName,extract_filenameext(fdata2.m_szLibName).c_str());
				imp_func_data f(fdata2,strAuxDir);

				if (!f.m_Func){
					output("Can't load imported function ",extract_filenameext(fdata.m_szLibName).c_str(),"::",fdata.m_szFuncName,".\n");
					return recCANT_LOAD_LIB;
				}
				m_ImportFuncs.push_back(f);
			}
			else {
				m_ImportFuncs.push_back(f);
			}
		}
	}
	return ret;
}
//------------------------------------------------------------------------
ERunnerExitCodes CInterpreter::Run(size_t argc, cstr argv[], int & iExitCode)
{
	iExitCode=recABNORMAL_TERMINATION; //Считаем, что именно это вернет функция main

	ASSERT_RETURN(m_Code.IsLoaded(),recNO_LOADED_CODE);
	_ASSERTE(argc>=1);
	//Параметры командной строки
	m_argv.reserve(argc);
	for (size_t i=0;i<m_argv.size();i++)
		m_argv[i]=argv[i];

	
	DWORD CommandPointer;
	ClearInternalStructures();	//на всякий случай очищаем все внутренние структуры

	m_DeclaredVariables.PushLevel(); //По умолчанию всегда имеется глобальный уровень видимости
	m_LocalStack.PushLevel();		 //По умолчанию всегда имеется глобальный уровень видимости

	///////--------------- Intializing Data ------------
	bool bSuccess = RunFromTo(0,END_ISEGM,CommandPointer);
	if (!bSuccess)
		return recWRONG_FORMAT;

	///////--------------- Run Code ------------
	if (!m_Code.IsStartPoint())	
		m_DataStack.Push(SVar(0L));				//эмулируем	код возврата
	else	//есь дальнейшая точка входа
	{
		// Для main
		m_AddrStack.Push(CommandPointer,"main");	// адрес возврата 
		m_DeclaredVariables.PushFrame();	//
		m_LocalStack.PushFrame();			//


		//if (pExternalDebugHandler)  // Мы в отладке
		//	FunctionsInfoStack.Push(DWORD ("main"));

		bSuccess = RunFromTo(m_Code.GetStartPoint(),HALT,CommandPointer);
		
		//Сохраняем результаты
		string_smart s = m_DeclaredVariables.Get(0,DEST_FILE_NAME_IDX).as_string();
		if (!s.isempty())
			if (!m_Container.store(s.c_str()))
				output("Can't open file '",s.c_str(),"' for write.\n");
	}

	//Допольнительная проверка правильности функционирования
	if (bSuccess){
		//Забираем из стека результат выполнения функции (пока никак не используется)
		_ASSERT(!m_DataStack.IsEmpty());
		iExitCode = m_DataStack.Pop().as_int();

		_ASSERT(m_AddrStack.IsEmpty());
		_ASSERT(m_DataStack.IsEmpty());
		_ASSERT(m_LocalStack.Size()==1); //см начало процедуры
	}
	ClearInternalStructures();
	return recOK;
}
//------------------------------------------------------------------------
bool CInterpreter::RunFromTo(DWORD StartAddr, ECodeOperation EndOperation, DWORD &CommandPointer)
{
	m_Code.SetIP(StartAddr); //Устанавливаем текущуб команду
	size_t nNextIP;
	while (m_Code.CurCmd().Operation!=EndOperation)
	{
		nNextIP=m_Code.GetIP();

		if (m_fStepCallBack)
			m_fStepCallBack(grmexec_exports::step_info(
								m_Code.CurCmd().m_strModuleName.c_str(),
								m_Code.CurCmd().dwSrcLine,
								m_AddrStack.Size()+m_bParsing,false));


		try{
			if (!ProcessCommand(m_Code.CurCmd(),nNextIP))
				return false;
		}
		catch (exception & e) 
		{
			cstr szModule; 
			size_t nSrcLine;
			
			//В будущем сюда еще добавиться название модуля
			if (m_Code.GetSrcData(m_Code.GetIP(),szModule,nSrcLine))
				MessageBox(0,Format("Runtime error: %s.\n\nModule = \"%s\"\nLine = %d",e.what(),szModule,nSrcLine).c_str(),NULL,MB_OK);
			else
				MessageBox(0,Format("Runtime error: %s.",e.what()).c_str(),NULL,MB_OK);
			
			if (m_fStepCallBack)
				m_fStepCallBack(grmexec_exports::step_info(
								szModule,
								nSrcLine,
								m_AddrStack.Size()+m_bParsing,true));
			return false;	
		}
		m_Code.SetIP(nNextIP);
	}
	CommandPointer=m_Code.GetIP();
	return true;
}
//------------------------------------------------------------------------
bool CInterpreter::ProcessCommand(SCommand & Cmd, size_t & nNextIP)
{
	#define CCOp1		Cmd.Op1
	#define CCOp2		Cmd.Op2
	#define CCOp3		Cmd.Op3
	#define CCOp4		Cmd.Op4

	#define LvlIdx		CCOp1.Index // первым параметром обычно указывается уровень
	#define VarIdx		CCOp2.Index // вторым - номер переменной, константы в текущем уровне
	#define TypeIdx		CCOp3.Index	// третьим - номер типа

	SVar Var1,Var2,Var3; 
	long  i;
	DWORD dwHelp;
	
	//---------- DECODER -------------
	switch (Cmd.Operation)
	{
		case NOP:
			break;
		case PUSH_EMPTY:		
			_ASSERTE(CCOp1.Class==TYPE_CLASS_ID); // Op1- тип аргумента
			m_DataStack.Push(SVar(m_Code.GetType(CCOp1.Index)));
			break;
		case PUSH:
			_ASSERTE(CCOp1.Class==NO_CLASS); // недостроено
			_ASSERTE(CCOp2.Class==NO_CLASS);	// недостроено
			// при осутствии параметров берем из стека переменных
			m_DataStack.Push(m_LocalStack.Pop()); 
			break;
		case POP_ALIAS:
			_ASSERTE(CCOp1.Class!=NO_CLASS);
			m_DataStack.Pop(Var1);
//			_ASSERT(_CrtIsValidHeapPointer(Var1.m_pSrc)); //Здесь должен быть указатель на настоящую переменную
			m_DeclaredVariables.Get(LvlIdx,VarIdx).SetAlias(Var1.GetLValueSrc());
			m_DeclaredVariables.Get(LvlIdx,VarIdx).Assign(*Var1.GetLValueSrc());
			break;
     	case POP:
			if (CCOp1.Class==NO_CLASS){	// при осутствии параметров кладем в стек переменных
				m_LocalStack.Push(m_DataStack.Pop());	
			}
			else
			{
				_ASSERTE(CCOp1.Class!=NO_CLASS);
				m_DeclaredVariables.Get(LvlIdx,VarIdx).Assign(m_DataStack.Pop());
			}
			break;
		case NEWA:			
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			_ASSERTE(CCOp2.Class==ID_CLASS_ID);
			_ASSERTE(CCOp3.Class==TYPE_CLASS_ID);
			dwHelp = m_DeclaredVariables.Add(LvlIdx,SVar(m_Code.GetType(TypeIdx),CCOp2.m_strSrcText.c_str())); // create var
			_ASSERTE(dwHelp==VarIdx);	// нужно удостовериться, что мы записали именно в свою ячейку
			
			if (CCOp4.Class==NO_CLASS){ // Берем размерность массива из стека
				m_LocalStack.Pop(Var3);
				m_DeclaredVariables.Get(LvlIdx,VarIdx).CreateArray(Var3.as_int());
			}
			else {
				_ASSERTE(CCOp4.Class==DIRECT_NUMBER_CLASS_ID);
				m_DeclaredVariables.Get(LvlIdx,VarIdx).CreateArray(CCOp4.Index);
			}
			break;
	  	case NEW:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
	 		_ASSERTE(CCOp2.Class==ID_CLASS_ID);
			_ASSERTE(CCOp3.Class==TYPE_CLASS_ID);
			dwHelp = m_DeclaredVariables.Add(LvlIdx,SVar(m_Code.GetType(TypeIdx),CCOp2.m_strSrcText.c_str())); // create var
			_ASSERTE(dwHelp==VarIdx);	// нужно удостовериться, что мы записали именно в свою ячейку
			break;
		case ASSIGN:
			m_LocalStack.Pop(Var2); // присваемое (правая часть)
			m_LocalStack.Pop(Var1); // присваиватель (левая часть)
			_ASSERTE(Var1.GetLValueSrc());
			_ASSERTE(Var1.GetLValueSrc()->IsLValue());	//

			if (Var1.GetLValueSrc()->IsSimple())
			{
				_ASSERTE(Var1.GetLValueSrc()->IsSimple());
				switch (CCOp1.Index)
				{
				case SIMPLE_ASSIGN :
					Var1.GetLValueSrc()->Assign(Var2);
					break;
				case ADD_ASSIGN    :
					add_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case MUL_ASSIGN    :
					mul_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case SUB_ASSIGN    :
					sub_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case DIV_ASSIGN    :
					div_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case RMNDR_ASSIGN  :
					rmndr_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case SHL_ASSIGN    :
					shl_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case SHR_ASSIGN    :
					shr_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case AND_ASSIGN    :
					and_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case XOR_ASSIGN    :
					xor_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				case OR_ASSIGN     :
					or_assign_variant(*Var1.GetLValueSrc(),Var2);
					break;
				default:
					_ASSERTE(0);
				}
			}
			else{
				Var1.GetLValueSrc()->operator = (Var2);
			}
			m_LocalStack.Push(*Var1.GetLValueSrc()); 
			break;
		case STORE:			
			if (CCOp1.Class==NO_CLASS) // Мы внутри структуры
			{
				m_LocalStack.Pop(Var1);
				_ASSERTE(Var1.IsLValue());
				_ASSERTE(VarIdx<Var1.Members.size());
				//Сместились и заталкиваем смещенное значение
				Var2=*Var1.Members[VarIdx];	//Не используйте ту же переменную!
				_ASSERTE(Var2.IsLValue());
				m_LocalStack.Push(Var2);
			}	
			else 
			{
				switch (CCOp2.Class)
				{
					case ID_CLASS_ID:
						Var1.CopyRef(m_DeclaredVariables.Get(LvlIdx,VarIdx));
						_ASSERTE(Var1.IsLValue());
						m_LocalStack.Push(Var1);
						break;
					case CONST_CLASS_ID:
						// CCOp1.Index - можно игнорировать 
						m_LocalStack.Push(m_Code.GetConst(VarIdx));
						break;
					default:
						_ASSERTE(0);
				}
			}
			break;
		case STORE_IDX:
			m_LocalStack.Pop(Var3);

			if (CCOp1.Class==NO_CLASS) // Мы внутри структуры
			{
				m_LocalStack.Pop(Var1);
				_ASSERTE(Var1.IsLValue());
				_ASSERTE(VarIdx<Var1.Members.size());
				//Сместились и заталкиваем смещенное значение
				SVar * pVar = Var1.Members[VarIdx];
				if (Var3.as_int()>=(int)pVar->GetArrayDim())
					THROW_RT_ERROR("Index out of bound");
				
				Var2=*pVar->Neighbours[Var3.as_int()]; //Не используйте ту же переменную!
				_ASSERTE(Var2.IsLValue());
				m_LocalStack.Push(Var2);
			}	
			else
			{
				_ASSERTE (CCOp2.Class==ID_CLASS_ID);
				if ((Var3.as_int()<0) || (Var3.as_int()>=(int)m_DeclaredVariables.Get(LvlIdx,VarIdx).GetArrayDim()))
					THROW_RT_ERROR("Index out of bounds");
					
				Var1.CopyRef(*m_DeclaredVariables.Get(LvlIdx,VarIdx).Neighbours[Var3.as_int()]);
				Var1.SetLValueSign(true);				
				m_LocalStack.Push(Var1);
				break;
			}
			break;
		case SIZEOF:
			if (CCOp1.Class==NO_CLASS) // Type
				m_LocalStack.Push(m_Code.GetType(VarIdx).GetMemSize());	
			else if (CCOp1.Class==DIRECT_NUMBER_CLASS_ID){
				SVar & RefVar = m_DeclaredVariables.Get(LvlIdx,VarIdx);
				if (RefVar.GetArrayDim()==0)
					m_LocalStack.Push(RefVar.m_pType->GetMemSize());	
				else
					m_LocalStack.Push(RefVar.m_pType->GetMemSize()*RefVar.GetArrayDim());	
			}
			else
				_ASSERTE(0);
			break;
		case SIZEA:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			_ASSERTE(CCOp2.Class==ID_CLASS_ID);
			{
				SVar & RefVar = m_DeclaredVariables.Get(LvlIdx,VarIdx);
				m_LocalStack.Push(RefVar.GetArrayDim());	
			}
			break;
    	case EQ:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
    		m_LocalStack.Push(Var1==Var2);
			break;
		case INEQ:			
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(Var1!=Var2);
			break;
		case LESS:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(Var1<Var2);
			break;
		case MORE:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(Var1>Var2);
			break;
	    case LESS_EQ:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(Var1<=Var2);
			break;
		case MORE_EQ:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(Var1>=Var2);
			break;
		case NOT:
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(!Var1);		
			break;
		case NEG:
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(sub_variant(variant(0),Var1));
			break;
		case L_AND:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push((!Var1.isempty()) && (!Var2.isempty()));
			break;
		case L_OR:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push((!Var1.isempty()) || (!Var2.isempty()));
			break;
		case AND:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(and_variant(Var1,Var2));
			break;
		case OR:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(or_variant(Var1,Var2));
			break;
		case XOR:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(xor_variant(Var1,Var2));
			break;
		case SHL:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(shl_variant(Var1,Var2));
			break;
		case SHR:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(shr_variant(Var1,Var2));
			break;
	    case ADD: 
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(add_variant(Var1,Var2));
			break;
		case SUB:		
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(sub_variant(Var1,Var2));
			break;
		case MUL:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
			m_LocalStack.Push(mul_variant(Var1,Var2));
			break;
    	case DIV:
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
    		m_LocalStack.Push(div_variant(Var1,Var2));
			break;
    	case RMNDR: 
			m_LocalStack.Pop(Var2);										
			m_LocalStack.Pop(Var1);										
    		m_LocalStack.Push(rmndr_variant(Var1,Var2));
			break;
		case GET_JZ:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			_ASSERTE(CCOp1.Index<m_Code.GetSize());
			m_LocalStack.Pop(Var1);
			if (!Var1){
				nNextIP=CCOp1.Index;
				return true;
			}
			break;
		case JMP:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			_ASSERTE(CCOp1.Index<m_Code.GetSize());
			nNextIP=CCOp1.Index;
			return true;
		case NEW_LEVEL:  	
			m_DeclaredVariables.PushLevel();
			m_LocalStack.PushLevel();
			break;
		case EXIT_LEVEL: 
			m_DeclaredVariables.PopLevel();
			m_LocalStack.PopLevel();
			break;
		case RET:
			m_DeclaredVariables.PopFrame();	// destroy superfluous levels
			m_LocalStack.PopFrame();		//	
				
			nNextIP=m_AddrStack.Pop();	// адрес возврата

			//if (pExternalDebugHandler)  // Мы в отладке
			//		FunctionsInfoStack.Pop();
			break;
		case RESTORE:	
			m_LocalStack.ClearTopLevel();
			break;
		case UNSTORE:	
			m_LocalStack.Delete();
			break;
		case CALL:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);		
			m_AddrStack.Push(m_Code.GetIP(),CCOp1.m_strSrcText.c_str());		// адрес возврата 
			m_LocalStack.PushFrame();
			m_DeclaredVariables.PushFrame();

			//if (pExternalDebugHandler)  // Мы в отладке
			//	FunctionsInfoStack.Push(DWORD (LPCTSTR(CCOp1.SrcText)));
			nNextIP=CCOp1.Index;
			return true;
		case PRINT:
			m_LocalStack.Pop().Print(outs);
			m_LocalStack.Push(SVar()); 	// put NULL
			break;
		case WRITE:{
				string_smart s;
				m_LocalStack.Push(m_Container.append(m_LocalStack.Pop()));
			}
			break;
		case FORMAT:
			//Op1 - кол-во аргументов в стеке
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			_ASSERTE(CCOp1.Index!=0);
			{
				raw_buffer<char> buff(CCOp1.Index*sizeof(long)*2); //*2 - c запасом
				raw_buffer<SVar> ops(CCOp1.Index); // выносим все операнды из стека
				buff.zero();
				//Обратный порядок передачи
				char * pBuf= buff.ptr()+buff.buffer_size(); //перемещаемся в конец буфера 
				
				//Переписываем аргументы из стека и размещаем их в массиве в обратном порядке
				//что дает их "нормальное представление"
				for (i=1;i<int(CCOp1.Index);i++){
					m_LocalStack.Pop(ops[i]);
					pBuf-=ceilgran(ops[i].size_of(),sizeof(DWORD)); //на 32-битных платформах минимальный размер передачи
					ops[i].write_dump(pBuf);						
				}
				
				m_LocalStack.Pop(Var1);		// format string
				Var1.cast(variant::vtSTRING);

				string_smart strRes(256); 
				while (_vsnprintf(strRes.buffer(),strRes.buffer_size()-1,Var1.as_cstr(),pBuf) <0 ) 
					strRes.reserve(strRes.buffer_size()*2);
					
				m_LocalStack.Push(strRes.c_str());
			}
			break;
		case RECALL_EXTERN:
			// Op1 - FuncIndex in import table
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			{
				_ASSERT(CCOp1.Index<m_ImportFuncs.size());	
				const imp_func_data & fdata = m_ImportFuncs[CCOp1.Index];
				
				raw_buffer<byte> arg_buff(fdata.calc_args_mem(daDWORD)); //Выравнивание на 4 байта
				raw_buffer<byte> out_buff(fdata.m_RetValue.mem_size(daDWORD)); 
				arg_buff.zero();
				out_buff.zero();

				vector<SVar> lst_ops; // выносим все операнды из стека в список
				lst_ops.reserve(fdata.m_nArgCount);

				//Переписываем аргументы из стека и размещаем их в массиве в НОРМАЛЬНОМ порядке
				//так как это идет чтение данных из стека функций, а не из локального стека
				for (size_t i=0;i<fdata.m_nArgCount;i++)
					lst_ops.push_back(m_DataStack.Pop());

				//Сбрасываем дамп параметров в общий массив памяти
				byte* pBuf= arg_buff.ptr(); 
				for (size_t i=0;i<fdata.m_nArgCount;++i){
					SVar & var = (lst_ops[i].IsLValue())			//Отслеживаем значение по ссылке
									? *lst_ops[i].GetLValueSrc()
									: lst_ops[i]; 

					raw_buffer<byte> bf  = fdata.m_Args[i].mem_dmp(var,daDWORD);
					bf.copy_to(pBuf,bf.buffer_size()); pBuf+=bf.buffer_size();
				}

				variant RetVal; //Стандартные библиотеки не могут возвращать другой тип,
								//кроме того, что заложен в variant
				//Здесь не остлеживается передача значения по ссылке
				RetVal.set_type(grm_structs::object::object2variant(fdata.m_RetValue.m_VarType));

				misc_utils::call_stdcall_func(	fdata.m_Func.ptr(),
												arg_buff.ptr(),arg_buff.buffer_size(),
												out_buff.ptr(),out_buff.buffer_size());
				RetVal.read_dump(out_buff.ptr());
				m_DataStack.Push(SVar(RetVal));
			}
			break;
		case LABEL:
			break;
		case INC_POST:
			m_LocalStack.Get(Var1); 

			_ASSERTE(Var1.GetLValueSrc());
			_ASSERTE(Var1.GetLValueSrc()->IsLValue());
			_ASSERTE(Var1.GetLValueSrc()->IsSimple());
			
			m_LocalStack.Push(Var1);  //Так как это пост-инкрементация, то сначала кладем
									  //старое значение
			inc_variant(*Var1.GetLValueSrc());
			Var1.CopyRef(*Var1.GetLValueSrc());
			break;
		case DEC_POST:
			m_LocalStack.Pop(Var1); 

			_ASSERTE(Var1.GetLValueSrc());
			_ASSERTE(Var1.GetLValueSrc()->IsLValue());
			_ASSERTE(Var1.GetLValueSrc()->IsSimple());

			m_LocalStack.Push(Var1); //Так как это пост-декрементация, то сначала кладем
									 //старое значение
			
			dec_variant(*Var1.GetLValueSrc());
			Var1.CopyRef(*Var1.GetLValueSrc());
			break;
		case PRMCOUNT:
			//Op1 - scope level 
			//Op2 - index of variable
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);	// Здесь не может располаться структура
														// это внутренняя команда, пользователем не регулируется
			_ASSERTE(LvlIdx==0);						// Глобальная переменная
			_ASSERTE(CCOp2.Class=ID_CLASS_ID);

			_ASSERTE(m_DeclaredVariables.Get(LvlIdx,VarIdx).isint());
			m_DeclaredVariables.Get(LvlIdx,VarIdx).Assign((int)m_argv.size());
			break;
		case PRMSTR:
			//Op1 - scope level 
			//Op2 - index of variable
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);	// Здесь не может располаться структура
														// это внутренняя команда, пользователем не регулируется
			_ASSERTE(LvlIdx==0);						// Глобальная переменная
			_ASSERTE(CCOp2.Class=ID_CLASS_ID);
			for (i=0;i<(long)m_argv.size();i++)
			{
				_ASSERTE(i<=(long)m_DeclaredVariables.Get(LvlIdx,VarIdx).GetArrayDim());
				_ASSERTE(m_DeclaredVariables.Get(LvlIdx,VarIdx).Neighbours[i]->isstring());
				m_DeclaredVariables.Get(LvlIdx,VarIdx).Neighbours[i]->Assign(m_argv[i]);
			}
			break;
// Команды управления грамматиками =======================================
		case S_WORDS:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			(pIGrmKrnl->SolitaryWords)((unsigned char)CCOp1.Index);
			break;
		case I_W_SPACES:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			(pIGrmKrnl->IgnoreWhiteSpaces)((unsigned char)CCOp1.Index);
			break;
		case I_CASE:
			_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);
			(pIGrmKrnl->IgnoreCase)((unsigned char)CCOp1.Index);
			break;
		case GET_TEXT:
			m_LocalStack.Push((pIGrmKrnl->GetCurrentText)()); 	
			break;
		case I_BLOCK:
			m_LocalStack.Pop(Var2);	// End
			m_LocalStack.Pop(Var1);	// Start
			_ASSERTE((Var1.IsSimple()));
			_ASSERTE((Var2.IsSimple()));
		
			m_LocalStack.Push((pIGrmKrnl->IgnoreBlock)(Var1.as_cstr(),Var2.as_cstr()));
			break;
		case REPLACE_BLOCK:
			m_LocalStack.Pop(Var2);	// End
			m_LocalStack.Pop(Var1);	// Start
			_ASSERTE((Var1.IsSimple()));
			_ASSERTE((Var2.IsSimple()));

			m_LocalStack.Push((pIGrmKrnl->ReplaceBlock)(Var1.as_cstr(),Var2.as_cstr()));
			break;
// Встроенные функции ====================================================
		case SAVE_FILES:
			{
				cstr szFileName = m_DeclaredVariables.Get(0,DEST_FILE_NAME_IDX).as_cstr();
				if (!m_Container.store(szFileName))
					GEN_RT_ERROR3("Can't open file '",szFileName,"' for write");
			}
			break;
		case GET_FILE_POS:
			m_LocalStack.Push(m_Container.get_end()); 	
			break;
		case INS_BEFORE:				
		case INS_AFTER:
			m_LocalStack.Pop(Var2);	// what
			m_LocalStack.Pop(Var1);	// where 
			_ASSERTE((Var1.IsSimple()));
			_ASSERTE((Var2.IsSimple()));

			if (Var1.isstring())
				m_LocalStack.Push(m_Container.insert(Var2,Var1.as_cstr(),Cmd.Operation==INS_BEFORE));
			else
				m_LocalStack.Push(m_Container.insert(Var2,Var1.as_uint(),Cmd.Operation==INS_BEFORE));
			break;
		case PARSE:
			if (m_bParsing){
				GEN_RT_ERROR1("Recursion parse detected");
				m_LocalStack.Push(DWORD(0)); 	// create const =0 and push to stack
			}
			else{
				_ASSERTE(m_DeclaredVariables.Get(0,SOURCE_FILE_NAME_IDX).isstring());
				cstr szFilename = m_DeclaredVariables.Get(0,SOURCE_FILE_NAME_IDX).as_cstr();
				
				if (!(pIGrmKrnl->OpenSource)(szFilename)){
					GEN_RT_ERROR3("Can't open source file '",szFilename, "'");
					m_LocalStack.Push(false);
				}
				else{
					m_bParsing=true;
					_ASSERTE(m_DeclaredVariables.Get(0,DELIMITERS_IDX).isstring());
					if (!m_DeclaredVariables.Get(0,DELIMITERS_IDX).as_string().isempty())	//Временно!!!!
						(pIGrmKrnl->Delimiters)(m_DeclaredVariables.Get(0,DELIMITERS_IDX).as_cstr());

					_ASSERTE(CCOp1.Class==DIRECT_NUMBER_CLASS_ID);

					grmkrnl_exports::analyze_data dt;

					m_LocalStack.PushFrame();
					m_DeclaredVariables.PushFrame();

					if (!(pIGrmKrnl->Analyze)(CInterpreter::SubRoutineHandler,CCOp1.Index,dt))
					{
						string_smart s =gen_parse_error_msg(
											dt.szErrorText,
											dt.dwLine,dt.dwPos,
											dt.bPossibleError,
											dt.szErrorRuleName);
						GEN_RT_ERROR1(s.c_str());
						m_LocalStack.Push(false);
					}
					else
						m_LocalStack.Push(true);

					//Сохраняем статистику 
					string_smart strStcFile = base_utils::change_fileext(m_Code.GetFileName(),".stc");
					(pIGrmKrnl->SaveStatistic)(strStcFile.c_str());

					m_LocalStack.PopFrame();
					m_DeclaredVariables.PopFrame();

					//if (pExternalEventHandler)
						//	pExternalEventHandler(EVENT_END_PARSE);
					m_bParsing=false;
				}
			}
			break;
		// Неиспользуемые Метки
		case RESERVED:
		case FUNC:
		case SUBROUTINE:
		case ENDF:
		case ENDSUBR:
		case HALT:
		case END_ISEGM:
		case RET_S:
			THROW_RT_ERROR("Error in file: illegal operation");
			_ASSERTE(0);
			return false;
		case POP_NEW:	
		case SAVE:
		case LOAD:
			GEN_RT_ERROR1("This operation under construction");
			_ASSERTE(0);
			return false;
		// Неизвестные команды
		default:
			THROW_RT_ERROR("Unknown operation");
			_ASSERTE(0);
			return false;
			
	}
	nNextIP++;
	return true;
}
//------------------------------------------------------------------------
bool CInterpreter::CheckBreakPointOnUsed(cstr szModulePath, size_t dwLine)
{
	for (DWORD i=0; i<m_Code.GetSize();i++){
		SCommand & cmd = m_Code[i];
		if (cmd.dwSrcLine==dwLine) //временно
			return true;
	}
	return false;
}
//------------------------------------------------------------------------
string_smart CInterpreter::GetObjectNames(grmexec_exports::get_oi_filter filter)
{
	string_smart strResult;
	size_t i,j;
	switch (filter){
		case grmexec_exports::goifLocalVars:
			for (i=0;(i==0)||(i<m_DeclaredVariables.TopFrameSize());i++)
				for (j=0;j<m_DeclaredVariables.LevelSize(i);j++){
					cstr szName = m_DeclaredVariables.Get(i,j).GetDebugInfo();
					if ((szName) && (*szName)){
						if (!strResult.isempty())
							strResult.cat('\n');
						strResult.cat(szName);
					}
				}
			break;
		case grmexec_exports::goifAutoVars: 
			i = m_DeclaredVariables.TopFrameSize()-1;
			for (j=0;j<m_DeclaredVariables.LevelSize(i);j++){
				cstr szName = m_DeclaredVariables.Get(i,j).GetDebugInfo();
				if ((szName) && (*szName)){
					if (!strResult.isempty())
						strResult.cat('\n');
					strResult.cat(szName);
				}
			}
			break;		
		case grmexec_exports::goifStack:
			for (i=0;i<m_AddrStack.Size();i++){
				if (!strResult.isempty())
					strResult.cat('\n');
				strResult.cat("@").cat(m_AddrStack.GetNameAt(i));
			}
			break;
	}
	return strResult;
}
//------------------------------------------------------------------------
CInterpreter::FillObjectInfo(CInterpreter::CObjectInfo* pInfo, SVar* pVar){
	
	pInfo->m_strName = pVar->GetDebugInfo();	
	if (pInfo->m_strName.isempty())
		pInfo->m_strName="(NA)";	//Debug Info для полей структуры не сохр
	
	if (pVar->IsSimple()){
		pInfo->m_nKind=CObjectInfo::oikSimple;
		pInfo->m_vValue=*pVar;
	}	
	else{
		// Сработал на элементе массива структур
		//_ASSERT((pVar->Members.size()==0)|| (pVar->Neighbours.size()==0));
		
		if (pVar->Members.size()){	//Это структура
			pInfo->m_nKind=CObjectInfo::oikStruct;
			
			for (size_t i=0; i<pVar->Members.size();i++){
				CObjectInfo &info = (pInfo->m_Items.push_back(CObjectInfo()),pInfo->m_Items.back());			
				FillObjectInfo(&info,pVar->Members[i]);
			}
		}
		else{
			pInfo->m_nKind=CObjectInfo::oikArray;			

			for (size_t i=0; i<pVar->Neighbours.size();i++){
				CObjectInfo &info = (pInfo->m_Items.push_back(CObjectInfo()),pInfo->m_Items.back());			
				FillObjectInfo(&info,pVar->Neighbours[i]);
			}
		}
	}
}
//------------------------------------------------------------------------
grmexec_exports::object_info * CInterpreter::GetObjectInfo(cstr szObjectName,grmexec_exports::get_oi_result& result)
{
	static cstr szUnknown="???";
	static cstr szGlobal="global";
	
	result = grmexec_exports::goirUnknownError;
	if (!szObjectName)
		return NULL;

	SVar * pVar=NULL;
	CObjectInfo * pInfo =NULL;
		
	LOG_PROTECT_CODE_BEGIN
	//Это - запрос про функцию
	if (szObjectName[0]=='@')
	{
		size_t i; szObjectName++;
		for (i=0;i<m_AddrStack.Size();i++){ 
			cstr szName = m_AddrStack.GetNameAt(i);
			if ((szName) && (strcmp(szName,szObjectName)==0)){
				pInfo = new CObjectInfo;
				break;
			}
		}
		if (!pInfo)
			result = grmexec_exports::goirNotFound;		
		else {
			pInfo->m_nKind=CObjectInfo::oikFunction;
			pInfo->m_strName=szObjectName;
			
			cstr szModule=NULL; 
			size_t nSrcLine=0;
			cstr szDummy;
			m_Code.GetSrcData(m_AddrStack.GetAt(i),szModule,nSrcLine);
			if (i<m_AddrStack.Size()-1)	//Дело в том, что фактическая точка останова находится в следующей функции
				m_Code.GetSrcData(m_AddrStack.GetAt(i+1),szDummy,nSrcLine);
			else
				m_Code.GetSrcData(m_Code.GetIP(),szDummy,nSrcLine);
			
			pInfo->m_strContext=szModule;
			pInfo->m_vValue=nSrcLine;

			result=grmexec_exports::goirOK;			
		}
	}
	//Возвращение значения переменных
	else{
		long i;
		for (i=max(m_DeclaredVariables.TopFrameSize()-1,0);(!pVar)&&(i>=0);i--)
			for (size_t j=0;(!pVar)&&(j<m_DeclaredVariables.LevelSize(i));j++){
				SVar & var = m_DeclaredVariables.Get(i,j);
				if (strcmp(var.GetDebugInfo(),szObjectName)==0)
					pVar=&var;
			}
			
		if (!pVar)
			result = grmexec_exports::goirNotFound;
		else{
			pInfo = new CObjectInfo;
			try{
				if (i<0)	
					pInfo->m_strContext=szGlobal;
				else if (m_AddrStack.IsEmpty())
					pInfo->m_strContext=szUnknown;
				else
					pInfo->m_strContext=m_AddrStack.GetName();
					
				FillObjectInfo(pInfo,pVar);
			}
			catch(...){
				delete	pInfo;
				throw;
			}
			result=grmexec_exports::goirOK;
		}
	}
	LOG_PROTECT_CODE_END(MSG_EI_Locate("CInterpreter::GetObjectInfo"));
	return pInfo;
}
