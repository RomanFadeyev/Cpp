/********************************************************************
	created:	2004/08/19
	created:	19:8:2004   23:00
	filename: 	x:\multitranslator\sources\mtdev\com\cominterfaces.h
	file path:	x:\multitranslator\sources\mtdev\com
	file base:	cominterfaces
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Объявление всех COM-интерфейсов
				Данный модуль подключается в BaseStructs, a тот - в 
				stdafx.h, что приводит к перекомпилированию всего проекта
				Поэтому исправлять надо здесь как можно реже
*********************************************************************/

//Этот ifdef должен быть обязательно
#if !defined(AFX_COM_INTERFACES_H)
#define AFX_COM_INTERFACES_H

#pragma once

namespace COM
{ 
	[export] __interface IApplication;
	[export] __interface IParser;

	//------------------------------------------------------------------------
	// Document
	//------------------------------------------------------------------------
	[
		object,
		uuid("C7242A99-D06C-4A9E-818D-5D3180EB8D90"),
		dual, oleautomation, helpstring("IDocument Interface"),
		pointer_default(unique)
	]
	__interface IDocument : IDispatch
	{
		[id(1), helpstring("method Save")] HRESULT Save(void);
		[id(2), helpstring("method Close")] HRESULT Close(void);
		
		[propget, id(10), helpstring("property Application")] HRESULT Application([out, retval] IApplication** pVal);
		[propget, id(11), helpstring("property Modified")] HRESULT Modified([out, retval] VARIANT_BOOL* pVal);
		[propput, id(11), helpstring("property Modified")] HRESULT Modified([in] VARIANT_BOOL newVal);
		[propget, id(12), helpstring("property Path")] HRESULT Path([out, retval] BSTR* pVal);
	};

	//------------------------------------------------------------------------
	// SourceDocument
	//------------------------------------------------------------------------
	[
		object,
		uuid("4FE902DC-9597-4d4e-81EC-179AFADCF8F7"),
		dual, oleautomation, helpstring("ISourceDocument Interface"),
		pointer_default(unique)
	]
	__interface ISourceDocument : IDocument
	{
	};

	//------------------------------------------------------------------------
	// IProjectDocument
	//------------------------------------------------------------------------
	[
		object,
		uuid("C7242A99-D06C-4A9E-818D-5D3180EB8D99"),
		dual, oleautomation, helpstring("IProjectDocument Interface"),
		pointer_default(unique)
	]
	__interface IProjectDocument : IDocument
	{
		[id(20), helpstring("method SaveAll")] HRESULT SaveAll([out,retval] VARIANT_BOOL* Result);
		[id(21), helpstring("method Build")] HRESULT Build([out,retval] VARIANT_BOOL* Result);
		[id(22), helpstring("method Run")] HRESULT Run([out] LONG* ExitCode, [out,retval] VARIANT_BOOL* Result);
		[id(23), helpstring("method Clean")] HRESULT Clean(void);

		[id(24), helpstring("method AddFile")] HRESULT AddFile([in] BSTR FilePath, [out,retval] VARIANT_BOOL* Result);
		[id(25),helpstring("method RemoveFile")] HRESULT RemoveFile([in] BSTR FilePath, [out,retval] VARIANT_BOOL* Result);

		[id(26), helpstring("method GetFileFirst")] HRESULT GetFileFirst([out] BSTR* FileName, [out,retval] VARIANT_BOOL* Result);
		[id(27), helpstring("method GetFileNext")] HRESULT GetFileNext([out] BSTR* FileName, [out,retval] VARIANT_BOOL* Result);

		[propget, id(41), helpstring("property Active")] HRESULT Active([out, retval] VARIANT_BOOL* pVal);
		[propput, id(41), helpstring("property Active")] HRESULT Active([in] VARIANT_BOOL newVal);
		
		[propget, id(42), helpstring("property TargetPath")] HRESULT TargetPath([out, retval] BSTR* pVal);
		[propget, id(43), helpstring("property TargetDirectory")] HRESULT TargetDirectory([out, retval] BSTR* pVal);
		
		[propget, id(44), helpstring("property Notes")] HRESULT Notes([out, retval] BSTR* pVal);
		[propput, id(44), helpstring("property Notes")] HRESULT Notes([in] BSTR newVal);
		
		[propget, id(45), helpstring("property Params")] HRESULT Params([out, retval] BSTR* pVal);
		[propput, id(45), helpstring("property Params")] HRESULT Params([in] BSTR newVal);
		
		[propget, id(46), helpstring("property Switches")] HRESULT Switches([out, retval] BSTR* pVal);
		[propput, id(46), helpstring("property Switches")] HRESULT Switches([in] BSTR newVal);
		
		[propget, id(47), helpstring("property LangGens")] HRESULT LangGens([out, retval] BSTR* pVal);
		[propput, id(47), helpstring("property LangGens")] HRESULT LangGens([in] BSTR newVal);
	};


	//------------------------------------------------------------------------
	// Interface IApplication
	//------------------------------------------------------------------------
	[
		object,
		uuid("EE3E904C-6AA0-401A-BCB5-A936E3B717F6"),
		dual, oleautomation, helpstring("IApplication Interface"),
		pointer_default(unique)
	]
	__interface IApplication : IDispatch
	{
		[id(1), helpstring("method Quit")] HRESULT Quit([in] VARIANT_BOOL SaveChanges);
		[id(2), helpstring("method ScreenRefresh")] HRESULT ScreenRefresh(void);
		[id(3), helpstring("method Resize")] HRESULT Resize([in] LONG Width, [in] LONG Height);
		[id(4), helpstring("method Activate")] HRESULT Activate(void);
		[id(5), helpstring("method OpenDocument")] HRESULT OpenDocument([in] BSTR Path, [out,retval] IDocument** pDoc);
		[id(6), helpstring("method FindDocument")] HRESULT FindDocument([in] BSTR Path, [out,retval] IDocument** pDoc);
		[id(7), helpstring("method CreateProject")] HRESULT CreateProject([in] BSTR Path, [out,retval] IProjectDocument** pDoc);

		[propget, id(17), helpstring("property ProjectCount")] HRESULT ProjectCount([out, retval] LONG* pVal);
		[propget, id(18), helpstring("property Projects")] HRESULT Projects([in] LONG nIndex, [out, retval] IProjectDocument** pDoc);
		[propget, id(19), helpstring("property ActiveProject")] HRESULT ActiveProject([out, retval] IProjectDocument** pVal);
		
		[propget, id(30), helpstring("property MRUDocument")] HRESULT MRUDocument([out, retval] BSTR* pVal);
		[propget, id(31), helpstring("property Visible")] HRESULT Visible([out, retval] VARIANT_BOOL* pVal);
		[propput, id(31), helpstring("property Visible")] HRESULT Visible([in] VARIANT_BOOL newVal);
		[propget, id(33), helpstring("property Parser")] HRESULT Parser([out, retval] IParser** pVal);
	};

	[ 
		export,
		uuid("0971D8E9-34A6-4846-81F3-B7E8FE27B9A3") 
	]
	enum GrammaticalSymbolClass {gscUndefined, gscTerminal, gscNonTerminal, gscStdTerminal};

	[ 
		export,
		uuid("6BE132AD-00C5-4afd-ABDB-32F1D0C013D3") 
	]
	enum GrammaticalSymbolKind	{gskUndefined, gskNormal, gskOptional, gskChildOptional};

	//------------------------------------------------------------------------
	// IGrammaticalSymbol
	//------------------------------------------------------------------------
	[
		object,
		uuid("B0E9D7B9-ECC5-44bd-8F18-BD3499B73DC2"),
		dual,	helpstring("IGrammaticalSymbol Interface"),
		pointer_default(unique)
	]
	__interface IGrammaticalSymbol : IDispatch
	{
		[propget, id(1), helpstring("property Name")] HRESULT Name([out, retval] BSTR* pVal);
		[propput, id(1), helpstring("property Name")] HRESULT Name([in] BSTR newVal);
		[propget, id(2), helpstring("property Class")] HRESULT Class([out, retval] GrammaticalSymbolClass* pVal);
		[propput, id(2), helpstring("property Class")] HRESULT Class([in] GrammaticalSymbolClass newVal);
		[propget, id(3), helpstring("property Kind")] HRESULT Kind([out, retval] GrammaticalSymbolKind* pVal);
		[propput, id(3), helpstring("property Kind")] HRESULT Kind([in] GrammaticalSymbolKind newVal);
		[propget, id(4), helpstring("property Actions")] HRESULT Actions([out, retval] BSTR* pVal);
		[propput, id(4), helpstring("property Actions")] HRESULT Actions([in] BSTR newVal);
	};

	//------------------------------------------------------------------------
	// IGrammaticalVariant
	//------------------------------------------------------------------------
	[
		object,
		uuid("6AF2BCFC-CCFF-40a2-83C6-8AD725EAE48A"),
		dual,	helpstring("IGrammaticalVariant Interface"),
		pointer_default(unique)
	]
	__interface IGrammaticalVariant : IDispatch
	{
		[id(1), helpstring("method AddSymbol")] HRESULT AddSymbol([out,retval] IGrammaticalSymbol** pSymbol);
		
		[propget, id(10), helpstring("property Actions")] HRESULT Actions([out, retval] BSTR* pVal);
		[propput, id(10), helpstring("property Actions")] HRESULT Actions([in] BSTR newVal);
	};

	//------------------------------------------------------------------------
	// IGrammaticalRule
	//------------------------------------------------------------------------
	[
		object,
		uuid("092C0FB9-67A7-4C8A-9C6E-F125CD40C498"),
		dual,	helpstring("IGrammaticalRule Interface"),
		pointer_default(unique)
	]
	__interface IGrammaticalRule : IDispatch
	{
		[id(1), helpstring("method AddVariant")] HRESULT AddVariant([out,retval] IGrammaticalVariant** pVariant);
		[id(2), helpstring("method AssembleText")] HRESULT AssembleText([out,retval] BSTR* aText);

		[propget, id(10), helpstring("property Name")] HRESULT Name([out, retval] BSTR* pVal);
		[propput, id(10), helpstring("property Name")] HRESULT Name([in] BSTR newVal);
		[propget, id(11), helpstring("property Actions")] HRESULT Actions([out, retval] BSTR* pVal);
		[propput, id(11), helpstring("property Actions")] HRESULT Actions([in] BSTR newVal);
	};

	//------------------------------------------------------------------------
	// IGrammar
	//------------------------------------------------------------------------
	[
		object,
		uuid("F27BAF30-1371-49f6-B12D-ADCA02C94E05"),
		dual,	helpstring("IGrammar Interface"),
		pointer_default(unique)
	]
	__interface IGrammar : IDispatch
	{
		[id(1), helpstring("method AddRule")] HRESULT AddRule([out,retval] IGrammaticalRule** pVariant);
		[id(2), helpstring("method AssembleText")] HRESULT AssembleText([out,retval] BSTR* aText);

		[propget, id(10), helpstring("property RuleCount")] HRESULT RuleCount([out, retval] LONG * pVal);
		[propget, id(11), helpstring("property Rules")] HRESULT Rules([in] LONG nIndex, [out, retval] IGrammaticalRule** pVal);
	};

	//------------------------------------------------------------------------
	// IParser
	//------------------------------------------------------------------------
	[
		object,
		uuid("763009C4-1A1B-4185-96A6-2BFAEF106F0D"),
		dual,	helpstring("IParser Interface"),
		pointer_default(unique)
	]
	__interface IParser : IDispatch
	{
		[id(1), helpstring("method CreateRule")] HRESULT CreateRule([out,retval] IGrammaticalRule** aRule);
		[id(2), helpstring("method CreateGrammar")] HRESULT CreateGrammar([out,retval] IGrammar** aGrammar);
		
		[id(3), helpstring("method Parse")] HRESULT Parse([in] BSTR pText, [out,retval] IGrammar** aGrammar);

		//В качестве IgnorePairList, ReplacePairList нужно передавать попарно значения, разделенные #0
		//Количество элементов должно быть четными
		[id(4), helpstring("method ComposeMainFunction")] HRESULT ComposeMainFunction([in] BSTR RootRule, [in] VARIANT_BOOL IgnoreCase, 
			BSTR IgnorePairList, BSTR ReplacePairList, [out,retval] BSTR* pText);
	};

} //of namespace
#endif