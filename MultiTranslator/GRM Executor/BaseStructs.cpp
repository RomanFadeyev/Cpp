// BaseStructs.cpp: implementation of the CBaseStructs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseStructs.h"

//------------------------------------------------------------------------
void SVar::Print(out_stream& Msg)
{
	if (IsSimple()){
		Msg<<as_cstr();
	}
	else{ 
		Msg<<'{' ;
		for (size_t i=0;i<Members.size();i++)
		{
			if (i!=0)
				Msg<<',';
			Members[i]->Print(Msg);
		}
		Msg<<'}';
	}
}
//Связка с реально существующим объектом
void SVar::SetLValueSrc(SVar  & src)
{
	m_pSrc=&src;
	_ASSERT(IsSimple()==src.IsSimple());
	for (size_t i=0;i<Members.size();i++){
		_ASSERT(_CrtIsValidHeapPointer(src.Members[i]));
		Members[i]->SetLValueSrc(*src.Members[i]);
	}
	for (size_t i=1;i<Neighbours.size();i++){
		_ASSERT(_CrtIsValidHeapPointer(src.Neighbours[i]));
		Neighbours[i]->SetLValueSrc(*src.Neighbours[i]);
	}
}
//------------------------------------------------------------------------
SVar & SVar::CopyRef(SVar & src)
{
	*this=src;
	//Связка с реально существующим объектом
	SetLValueSrc(src);
	return *this;
}
//------------------------------------------------------------------------
SVar & SVar::operator = (const SVar & src)
{
	_ASSERT(m_pAlias==NULL);
	_ASSERT(_CrtIsValidPointer(& src,sizeof(SVar),true));
	this->Clear();
	this->m_bLValueSign= src.m_bLValueSign;	
	this->m_pType=src.m_pType;
	this->m_pSrc=src.m_pSrc;
	this->pStrSrc=src.pStrSrc;
	if (IsSimple()){
		variant::operator=(src);
		if ((src.GetArrayDim()>0) && (src.Neighbours[0]==&src)){
			CreateArray(src.GetArrayDim());		
			for (size_t i=1;i<Neighbours.size();i++)
				Neighbours[i]->operator = (*src.Neighbours[i]);
		}
	}
	else
		for (size_t i=0;i<src.Members.size();i++)
			this->Members.push_back(new SVar(*src.Members[i]));
	return *this;
}
//------------------------------------------------------------------------
SVar & SVar::Assign(const SVar & src)
{
	if (IsSimple())
	{
		variant::type t= get_type();
		variant::operator =(src);
		set_type(t);
	}
	else
	{
		_ASSERT(this->m_pType==src.m_pType);
		_ASSERT(this->Members.size()==src.Members.size());
		*this=src;
	}
	return *this;
}
//------------------------------------------------------------------------
SVar & SVar::Assign(const int iVal)
{
	_ASSERT(IsSimple());
	_ASSERT(get_type()==vtINT);
	variant::m_iVal=iVal;
	return *this;
}
//------------------------------------------------------------------------
SVar & SVar::Assign(cstr szVal)
{
	_ASSERT(IsSimple());
	_ASSERT(get_type()==vtSTRING);
	variant::m_strVal=szVal;
	return *this;
}
//------------------------------------------------------------------------
void SVar::Clear()
{
	variant::clear();
	DestroyArray();

	for (size_t i=0;i<Members.size();i++)
	{
		this->Members[i]->DestroyArray();
		delete (this->Members[i]);
	}
	Members.clear();
}
//------------------------------------------------------------------------
void SVar::CreateArray(long HowMany)
{
	_ASSERT(!Neighbours.size());
	if (HowMany==0) return ;
	Neighbours.push_back(this);
	for (long i=0;i<HowMany-1;i++){	// -1 - потому что первый уже создан
		SVar * pVar = new SVar(*this->m_pType);
		pVar->SetLValueSign(this->m_bLValueSign);
		Neighbours.push_back(pVar);
	}
	
}
//------------------------------------------------------------------------	
void SVar::DestroyArray()
{
	if (Neighbours.size()==0)
		return;
	_ASSERT(this==Neighbours[0]);
	
	for (size_t i=1;i<Neighbours.size();i++)
		delete Neighbours[i];
	Neighbours.clear();
}
