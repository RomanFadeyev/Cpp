/********************************************************************
	created:	2003/06/03
	created:	3:6:2003   21:12
	filename: 	$(Projects)\sources\grm compiler\utils\ewqueue.h
	file path:	$(Projects)\sources\grm compiler\utils
	file base:	ewqueue
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_WEQUEUE_H__18E1F8FB_01C3_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_WEQUEUE_H__18E1F8FB_01C3_11D4_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include <BaseStructs.h>
#include <OutStreams.h>
#include <BaseUtils.h>
#include <list>

namespace errors{
	bool is_ew();
	bool is_errors();
	bool is_warnings();
}

namespace errors
{
	enum err_code{
		errNone,
		errUnknownError,
		errInternalError,
		errGeneral,
		errUserBreak,
		errFileNotFound,
		errFileNotOpened,
		errSourceEmpty,
		errIllegalSymbol,
		errSuperfluousLeftBrace,
		errSuperfluousLeftBracket,
		errSuperfluousLeftSbracket,
		errSuperfluousRightBrace,
		errSuperfluousRightBracket,
		errSuperfluousRightSbracket,
		errUnterminatedString,
		errTooManyChars,
		errBadNumFormat,
		errPreprocessorError,
		errILWithoutEIL,
		errOLWithoutEOL,
		errWrongUseOfDirective,
		
		//Synntax Errors
		errGeneralSyntaxError,
		errNeedStringConst,
		errBadRegularExpression,
		errUnresolvedRule,
		errSymbolUndefined,
		errBadOperationWithNonArray,
		errLValueRequired,
		errBadTypeCast,
		errMustBeSimpleType,
		errBadParamCount,
		errBadOperationInSubroutine,
		errOperationOutOfSubroutine,
		errSubroutineHasNoRetVal,
		errFunctionHasNoRetVal,
		errFunctionHasNoEntryPoint,


		wrngGeneral,
		wrngFileNotOpened,
		wrngTruncatedAssign,
		wrngStrangeTypeCast,
		wrngStrangeUseOfType,
		wrngNoReferences,
		wrngUndefinedReturnValue,
		wrngStrangeTerminal,
		wrngLeftSidedRecursion,
		wrngNoEntryPoint
	};

	//base class for lexic error -----------------------------------------
	class error
	{
	protected:
		friend class ew_enumerator;
		enum class_type{ctError, ctWarning};

		string_std		m_strText;
		err_code		m_nCode;
		bool			m_bPositioned;
		bool			m_bPrinted;
		class_type		m_nClassType;
		cstr translate(char_t ch);
		
		bool is_error() const {return m_nClassType==ctError;}
		bool is_warning() const {return m_nClassType==ctWarning;}

		virtual cstr class_name() const	{return "error";}
		void make(err_code code, const string_smart & s, const structs::text_location & pos);
		void make(err_code code, const string_smart & s);
	public:
		error();
		virtual ~error();

		void output(out_streams::out_stream & stream) const	{stream<<text();}
		cstr text()		const								{return m_strText.c_str();}
		err_code code()	const								{return m_nCode;}
	};
	
	//base class for warnings---------------------------------------------
	class warning: public error
	{
	protected:
		override cstr class_name() const	{return "warning";}
	public:
		warning(){
			m_nClassType=ctWarning;
		}
	};

	//base class for positioned errors------------------------------------
	class pos_error: public error
	{
	};

	//base class for positioned warings-----------------------------------
	class pos_warning: public warning
	{
	};

	//error/warning enumerator--------------------------------------------
	class ew_enumerator
	{
	public:
		typedef std::list<error>	list_error;
	protected:
		list_error		m_lstErrors;
		
	public:
		void add		(const error &, bool bPrintAtOnce=true);

		void clear();
		void clear_e();
		void clear_w();

		size_t count_ew	() const;
		size_t count_e	() const;
		size_t count_w	() const;

		size_t output_ew(out_streams::out_stream & stream) ;
		size_t output_e	(out_streams::out_stream & stream) ;
		size_t output_w	(out_streams::out_stream & stream) ;
	};
	extern ew_enumerator main_enumerator;
	
	inline void output_ew(out_streams::out_stream & stream = out_streams::outs){
		main_enumerator.output_ew(stream);
	}

	inline void output_w(out_streams::out_stream & stream = out_streams::outs){
		main_enumerator.output_w(stream);
	}

	inline void output_e(out_streams::out_stream & stream = out_streams::outs){
		main_enumerator.output_e(stream);
	}

	inline bool is_ew(){
		return main_enumerator.count_ew()>0; 
	}

	inline bool is_errors(){
		return main_enumerator.count_e()>0; 
	}

	inline bool is_warnings(){
		return main_enumerator.count_w()>0; 
	}

	inline void clear_ew(){
		main_enumerator.clear();	
	}

	inline void clear_e(){
		main_enumerator.clear_e();
	}

	inline void clear_w(){
		main_enumerator.clear_w();
	}

	class ew_pos_corrector
	{
	public:
		struct mapped_position{
			size_t		m_nLine;
			string_std	m_strMappedFileName;
			size_t		m_nMappedLine;
			
			mapped_position(size_t nLine, string_adapter strMappedFileName, size_t nMappedLine): 
				m_nLine(nLine), m_strMappedFileName(strMappedFileName), m_nMappedLine(nMappedLine) {}
			
			size_t map_from(size_t nLine) const {
				return nLine-(m_nLine-m_nMappedLine)-1;
			}
		};

		class list_mapped_pos: public list<mapped_position>
		{
		public:
			const mapped_position * GetMappedPosition(size_t nLine) const {
				const mapped_position * pPos = NULL;

				for (const_iterator i = begin(); i!=end(); pPos = i.operator->(), ++i)
					if ((*i).m_nLine>=nLine)
						break;
				
				return pPos;
			}
		};

		typedef map<string_case_isense,list_mapped_pos> map_lists;
	private:
		map_lists m_mapFiles;
	public:
		void AddPosition(string_adapter strFile, size_t nLine, string_adapter strMappedFile, size_t nMappedLine);
		void GetMappedPosition(string_adapter strFile, size_t nLine, string_smart & strMappedFile, size_t & nMappedLine);
		const list_mapped_pos & GetMappedPositionsForFile(string_adapter strFile) const;

		void Clear() {m_mapFiles.clear();}
	};
	
	extern ew_pos_corrector main_pos_corrector;

	//automatic output of errors and warning, while you're leaving scope--
	struct ew_auto_output{
		~ew_auto_output(){
			output_ew(out_streams::outs);
		}
	};

	//error stub (no error)-----------------------------------------------
	struct e_no_error: public error
	{
		e_no_error(){
			m_nCode=errNone;
		}
	};
	//general_error ------------------------------------------------------
	struct  general_error: public error	//Нужно, в основном для CEWQueue
	{
		general_error(cstr szText){
			make(errGeneral,szText);
		}
	};
	//unrecognized error---------------------------------------------------
	struct e_unknown_error: public pos_error
	{
		e_unknown_error(const structs::text_location & pos){
			make(errUnknownError,"unknown error",pos);
		}
	};
	//internal error -----------------------------------------------------
	struct internal_error: public error
	{
		internal_error (cstr szText){
			make(errInternalError,Format("internal error \"%s\"",szText));
		}
	};
	//user break ---------------------------------------------------------
	struct e_user_break: public error
	{
		e_user_break(){
			make(errUserBreak,"build cancelled");
		}
	};
	//file not found  ----------------------------------------------------
	struct e_file_not_found: public error
	{
		e_file_not_found(cstr szFileName){
			make(errFileNotFound,Format("file '%s' was not opened",szFileName));
		}
	};
	//can't open file for friting ----------------------------------------
	struct e_file_was_not_opened: public error
	{
		e_file_was_not_opened(cstr szFileName){
			make(errFileNotOpened,Format("file '%s' can't be opened for writing",szFileName));
		}
	};

	//source is empty  ---------------------------------------------------
	struct e_source_empty: public error
	{
		e_source_empty(cstr szFileName){
			make(errSourceEmpty,Format("module %s is empty",szFileName));
		}
	};

	//preprocessor error -------------------------------------------------
	struct e_preprocessor_error: public error
	{
		e_preprocessor_error(cstr szFileName){
			make(errPreprocessorError,Format("preprocessor failed with module %s",szFileName));
		}
	};

	//Wrong use of directive %s
	struct e_wrong_use_of_directive: public pos_error
	{
		e_wrong_use_of_directive(cstr szText, const structs::text_location & pos){
			make(errWrongUseOfDirective,Format("Wrong use of directive %s",szText),pos);
		}
	};

	//#IL directive is not matched with #EIL
	struct e_il_without_eil: public pos_error
	{
		e_il_without_eil(const structs::text_location & pos){
			make(errILWithoutEIL,"#IL directive does not match with #EIL",pos);
		}
	};

	//#OL directive is not matched with #EOL
	struct e_ol_without_eol: public pos_error
	{
		e_ol_without_eol(const structs::text_location & pos){
			make(errOLWithoutEOL,"#OL directive does not match with #EOL",pos);
		}
	};

	//illegal symbol -----------------------------------------------------
	struct e_illegal_symbol: public pos_error
	{
		e_illegal_symbol(char_t ch, const structs::text_location & pos){
			make(errIllegalSymbol,Format("illegal symbol %s",translate(ch)),pos);
		}
	};
	//superfluous '{' -----------------------------------------------------
	struct e_superfluous_left_brace: public pos_error
	{
		e_superfluous_left_brace(const structs::text_location & pos){
			make(errSuperfluousLeftBrace,"compound statement has superfluous '{'",pos);
		}
	};                
	//superfluous '(' -----------------------------------------------------
	struct e_superfluous_left_bracket: public pos_error
	{
		e_superfluous_left_bracket(const structs::text_location & pos){
			make(errSuperfluousLeftBracket,"compound statement has superfluous '('",pos);
		}
	};      
	//superfluous '[' -----------------------------------------------------
	struct e_superfluous_left_sbracket: public pos_error
	{
		e_superfluous_left_sbracket(const structs::text_location & pos){
			make(errSuperfluousLeftSbracket,"compound statement has superfluous '['",pos);
		}
	};      
	//superfluous '}' -----------------------------------------------------
	struct e_superfluous_right_brace: public pos_error
	{
		e_superfluous_right_brace(const structs::text_location & pos){
			make(errSuperfluousRightBrace,"compound statement has superfluous '}'",pos);
		}
	};                
	//superfluous ')' -----------------------------------------------------
	struct e_superfluous_right_bracket: public pos_error
	{
		e_superfluous_right_bracket(const structs::text_location & pos){
			make(errSuperfluousRightBracket,"compound statement has superfluous ')'",pos);
		}
	};      
	//superfluous ']' -----------------------------------------------------
	struct e_superfluous_right_sbracket: public pos_error
	{
		e_superfluous_right_sbracket(const structs::text_location & pos){
			make(errSuperfluousRightSbracket,"compound statement has superfluous ']'",pos);
		}
	};      
	//Unterminated string ------------------------------------------------
	struct e_unterminated_string: public pos_error
	{
		e_unterminated_string(const structs::text_location & pos){
			make(errUnterminatedString,"unterminated string",pos);
		}
	};      
	//Character constant has more than 2 symbols--------------------------
	struct e_too_many_chars: public pos_error
	{
		e_too_many_chars(const structs::text_location & pos){
			make(errTooManyChars,"character constant must be one or two characters long",pos);
		}
	};      
	//Bad numerical format------------------------------------------------
	struct e_bad_num_format: public pos_error{
		e_bad_num_format(const structs::text_location & pos){
			make(errBadNumFormat,"bad numerical format",pos);
		}
	};
	//general_syntax_error-----------------------------------------------------
	//Скорее всего, это временно
	struct  general_syntax_error: public pos_error
	{
		general_syntax_error(cstr szText, const grm_structs::located_text & Arg){
			string_smart s(szText); s.replace("%s",Arg.GetSzText());
			make(errors::errGeneralSyntaxError,s,Arg.GetStartLocation());
		}
		general_syntax_error(const grm_structs::sntx_error & Data){
			make(errors::errGeneralSyntaxError,Data.GetText(),Data.GetLocation());
		}
	};
	//Unresolved rule-----------------------------------------------------
	struct e_unresolved_rule: public error{
		e_unresolved_rule(cstr szRuleName){
			make(errUnresolvedRule,Format("unresolved rule '%s'",szRuleName));
		}
	};
	//bad regular expression ---------------------------------------------
	struct e_bad_regular_expression : public pos_error{
		e_bad_regular_expression(const grm_structs::located_text & Object,size_t nDummy){
			make(errBadRegularExpression,"regular expression is incorrect",Object.GetStartLocation());
		}
	};
	//Object must be string constant -------------------------------------
	struct e_need_string_const: public pos_error{
		e_need_string_const(const grm_structs::located_text & Object,size_t nDummy){
			make(errNeedStringConst,Format("variant '%s' must be string constant",Object.GetSzText()),Object.GetStartLocation());
		}
	};
	//Symbol is endefined ------------------------------------------------
	struct e_symbol_undefined: public pos_error{
		e_symbol_undefined(const grm_structs::located_text & Object,size_t nDummy){
			make(errSymbolUndefined,Format("symbol '%s' is undefined",Object.GetSzText()),Object.GetStartLocation());
		}
	};
	//Bad operation with non-array ---------------------------------------
	struct e_bad_operation_with_non_array : public pos_error{
		e_bad_operation_with_non_array (const grm_structs::located_text & Object,size_t nDummy){
			make(errBadOperationWithNonArray,Format("incorrect operation with non-array variant '%s'",Object.GetSzText()),Object.GetStartLocation());
		}
	};
	//l-value required ---------------------------------------
	struct e_l_value_required : public pos_error{
		e_l_value_required (const grm_structs::located_text & Object,size_t nDummy){
			make(errLValueRequired,"l-value required",Object.GetStartLocation());
		}
	};
	//assign to empty type -----------------------------------------------
	struct e_bad_type_cast : public pos_error{
		e_bad_type_cast (const structs::located_typed_text & LeftObject,const structs::located_typed_text & RightObject){
			make(errBadTypeCast,Format("bad type cast from '%s' to '%s'",RightObject.GetSzTypeName(),LeftObject.GetSzTypeName()),RightObject.GetStartLocation());
		}
	};
	//function can't return value --------------------------------------
	struct e_operand_must_be_simple_type : public pos_error{
		e_operand_must_be_simple_type(const grm_structs::located_text & Object,size_t nDummy){
			make(errMustBeSimpleType,Format("variant '%s' must be of simple type",Object.GetSzText()),Object.GetStartLocation());
		}
	};
	//Bad paramcount ----------------------------------------
	struct e_bad_param_count : public pos_error{
		e_bad_param_count (const grm_structs::located_text & Object,size_t nDummy){
			make(errBadParamCount,Format("calling function '%s' with incorrect number of parameters",Object.GetSzText()),Object.GetStartLocation());
		}
	};
	//Bad operation in subroutine ----------------------------------------
	struct e_bad_operation_in_subroutine : public pos_error{
		e_bad_operation_in_subroutine (const grm_structs::located_text & Object,size_t nDummy){
			make(errBadOperationInSubroutine,"this operation can't be used in rule's subroutine",Object.GetStartLocation());
		}
	};
	//Оperation out of subroutine ----------------------------------------
	struct e_operation_out_of_subroutine : public pos_error{
		e_operation_out_of_subroutine (const grm_structs::located_text & Object,size_t nDummy){
			make(errOperationOutOfSubroutine,"this operation must be used only in rule's subroutine",Object.GetStartLocation());
		}
	};
	//Subroutine can't return value --------------------------------------
	struct e_subroutine_has_no_ret_val : public pos_error{
		e_subroutine_has_no_ret_val(const grm_structs::located_text & Object,size_t nDummy){
			make(errSubroutineHasNoRetVal,"rule's subroutine has no return value",Object.GetStartLocation());
		}
	};
	//function can't return value --------------------------------------
	struct e_function_has_no_ret_val : public pos_error{
		e_function_has_no_ret_val(const grm_structs::located_text & Object,size_t nDummy){
			make(errFunctionHasNoRetVal,"this function has no return value",Object.GetStartLocation());
		}
	};
	//external function exists in library, but has no declared entry point
	struct e_function_has_no_entry_point : public pos_error{
		e_function_has_no_entry_point(const grm_structs::located_text & Object,size_t nDummy){
			make(errFunctionHasNoEntryPoint,"external function exists in library but has no entry point",Object.GetStartLocation());
		}
	};

	// warnings 
	//
	struct  general_warning: public warning
	{
		general_warning(cstr szText){
			make(wrngGeneral,szText);
		}
	};
	//can't open file for friting ----------------------------------------
	struct w_file_was_not_opened: public warning
	{
		w_file_was_not_opened(cstr szFileName){
			make(wrngFileNotOpened,Format("file '%s' can't be opened for writing",szFileName));
		}
	};
	struct  w_truncated_assignment: public pos_warning
	{
		w_truncated_assignment(const structs::located_typed_text & FromType,const structs::located_typed_text & ToType){
			make(wrngTruncatedAssign,Format("truncation from '%s' to '%s'",FromType.GetSzTypeName(),ToType.GetSzTypeName()),ToType.GetStartLocation());
		}
	};
	struct  w_strange_type_cast: public pos_warning
	{
		w_strange_type_cast(const structs::located_typed_text & FromType,const structs::located_typed_text & ToType){
			make(wrngStrangeTypeCast,Format("strange type cast from '%s' to '%s'",FromType.GetSzTypeName(),ToType.GetSzTypeName()),	ToType.GetStartLocation());
		}
	};
	struct  w_strange_use_of_type: public pos_warning
	{
		//Второй параметр nDummy не нужен абсолютно, но без него комплятор путается
		//в приведениях типов
		w_strange_use_of_type(const structs::located_typed_text & Text,size_t nDummy){
			make(wrngStrangeUseOfType,Format("strange use of variant '%s' of type '%s'",Text.GetSzText(),Text.GetSzTypeName()),Text.GetStartLocation());
		}
	};
	struct  w_no_entry_point: public warning
	{
		w_no_entry_point(){
			make(wrngNoEntryPoint,"function 'main' is absent, there is no entry point");
		}
	};
	struct  w_no_references: public pos_warning
	{
		w_no_references(const grm_structs::located_text & Symbol, size_t nDummy){
			make(wrngNoReferences,Format("there are no references to '%s'",Symbol.GetSzText()),Symbol.GetStartLocation());
		}
	};
	struct w_undefined_return_value: public pos_warning
	{
		w_undefined_return_value(const grm_structs::located_text & Symbol, size_t nDummy){
			make(wrngUndefinedReturnValue,Format("return value of function '%s' might be undefined",Symbol.GetSzText()),Symbol.GetStartLocation());
		}
	};
	struct w_strange_terminal: public pos_warning
	{
		w_strange_terminal(const grm_structs::located_text & Symbol, size_t nDummy){
			make(wrngStrangeTerminal,Format("terminal %s has white spaces, it looks like a missprint",Symbol.GetSzText()),Symbol.GetStartLocation());
		}
	};
	struct w_left_sided_recursion: public pos_warning
	{
		w_left_sided_recursion(cstr szRuleName, size_t nDummy){
			make(wrngLeftSidedRecursion,Format("rule %s produces left sided recursion",szRuleName));
		}
	};
};

#endif // !defined(AFX_WEQUEUE_H__18E1F8FB_01C3_11D4_A1F4_0000E8D9F1D7__INCLUDED_)
