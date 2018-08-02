
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TDCCustomAttribData.h"
#include "TDCmapping.h"

#include "..\shared\misc.h"
#include "..\shared\datehelper.h"
#include "..\shared\timehelper.h"

/////////////////////////////////////////////////////////////////////////////

TDCCADATA::TDCCADATA(const CString& sValue, TCHAR cSep) 
{ 
	Set(sValue, cSep); 
}

TDCCADATA::TDCCADATA(LPCTSTR szValue, TCHAR cSep) 
{ 
	Set(CString(szValue), cSep); 
}

TDCCADATA::TDCCADATA(double dValue) 
{ 
	Set(dValue); 
}

TDCCADATA::TDCCADATA(double dValue, TDC_UNITS nUnits) 
{ 
	Set(dValue, nUnits); 
}

TDCCADATA::TDCCADATA(const CStringArray& aValues) 
{ 
	Set(aValues); 
}

TDCCADATA::TDCCADATA(const CStringArray& aValues, const CStringArray& aExtra) 
{ 
	Set(aValues, aExtra); 
}

TDCCADATA::TDCCADATA(int nValue) 
{ 
	Set(nValue); 
}

TDCCADATA::TDCCADATA(const COleDateTime& dtValue) 
{ 
	Set(dtValue); 
}

TDCCADATA::TDCCADATA(bool bValue) 
{ 
	Set(bValue); 
}

TDCCADATA::TDCCADATA(const TDCCADATA& data)
{
	*this = data;
}

TDCCADATA& TDCCADATA::operator=(const TDCCADATA& data)
{
	sData = data.sData;
	sExtra = data.sExtra;

	return *this;
}

BOOL TDCCADATA::operator==(const TDCCADATA& data) const 
{ 
	return ((sData == data.sData) && 
			(sExtra == data.sExtra)); 
}

BOOL TDCCADATA::operator!=(const TDCCADATA& data) const 
{ 
	return ((sData != data.sData) || 
			(sExtra != data.sExtra)); 
}

BOOL TDCCADATA::HasExtra() const
{
	if (!sData.IsEmpty())
		return !sExtra.IsEmpty();

	ASSERT(sExtra.IsEmpty());
	return FALSE;
}

BOOL TDCCADATA::IsEmpty() const 
{ 
	return (sData.IsEmpty() && sExtra.IsEmpty()); 
}

void TDCCADATA::Clear() 
{ 
	sData.Empty(); 
	sExtra.Empty();
}

const CString& TDCCADATA::AsString() const 
{ 
	ASSERT(sExtra.IsEmpty());

	return sData; 
}

double TDCCADATA::AsDouble() const 
{ 
	ASSERT(sExtra.IsEmpty());

	return _ttof(sData); 
}

int TDCCADATA::AsInteger() const 
{ 
	ASSERT(sExtra.IsEmpty());

	return _ttoi(sData); 
} 

COleDateTime TDCCADATA::AsDate() const 
{ 
	ASSERT(sExtra.IsEmpty());

	if (sData.IsEmpty())
		return CDateHelper::NullDate();

	return _ttof(sData); 
}

bool TDCCADATA::AsBool() const 
{ 
	ASSERT(sExtra.IsEmpty());

	return !IsEmpty(); 
}

int TDCCADATA::AsArray(CStringArray& aValues) const
{
	ASSERT(sExtra.IsEmpty());

	return AsArray(sData, aValues);
}

int TDCCADATA::AsArrays(CStringArray& aValues, CStringArray& aExtra) const
{
	AsArray(sExtra, aExtra);

	return AsArray(sData, aValues);
}

int TDCCADATA::AsArray(const CString& sValue, CStringArray& aValues) 
{ 
	// Special case: 1 empty value
	if (sValue == _T("\n"))
	{
		aValues.RemoveAll();
		aValues.Add(_T(""));
		return 1;
	}
	
	// else
	return Misc::Split(sValue, aValues, '\n', TRUE); 
}

double TDCCADATA::AsTimePeriod(TDC_UNITS& nUnits) const
{
	if (IsEmpty())
	{
		nUnits = TDCU_HOURS;
		return 0.0;
	}
	
	// else
	nUnits = GetTimeUnits();
	return _ttof(sData);
}

TDC_UNITS TDCCADATA::GetTimeUnits() const
{
	if (IsEmpty())
		return TDCU_HOURS;
	
	TDC_UNITS nUnits = (TDC_UNITS)Misc::Last(sData);
	
	if (!IsValidUnits(nUnits))
		nUnits = TDCU_NULL;
	
	return nUnits;
}

void TDCCADATA::Set(double dValue) 
{ 
	sData.Format(_T("%lf"), dValue); 
}

void TDCCADATA::Set(int nValue) 
{ 
	sData.Format(_T("%d"), nValue); 
}

void TDCCADATA::Set(const COleDateTime& dtValue) 
{ 
	sData.Format(_T("%lf"), dtValue); 
}

void TDCCADATA::Set(const CString& sValue, TCHAR cSep) 
{ 
	if (cSep != 0)
	{
		CStringArray aItems;
		
		if (Misc::Split(sValue, aItems, cSep, TRUE))
		{
			Set(aItems);
			return;
		}
	}
	
	// all else
	sData = sValue; 
}

void TDCCADATA::Set(double dValue, TDC_UNITS nUnits) 
{ 
	ASSERT(IsValidUnits(nUnits));
	sData.Format(_T("%lf:%lc"), dValue, (TCHAR)nUnits); 
}

void TDCCADATA::Set(bool bValue, TCHAR nChar) 
{ 
	sData.Empty(); 
	
	if (bValue)
	{
		if (nChar > 0)
			sData.Insert(0, nChar);
		else
			sData.Insert(0, '+');
	}
}

void TDCCADATA::Set(const CStringArray& aValues) 
{ 
	Set(aValues, sData);
}

void TDCCADATA::Set(const CStringArray& aValues, const CStringArray& aExtra) 
{ 
	Set(aValues, sData);
	Set(aExtra, sExtra);
}

void TDCCADATA::Set(const CStringArray& aValues, CString& sValue)
{
	// Special case: 1 empty value
	if ((aValues.GetSize() == 1) && aValues[0].IsEmpty())
		sValue = '\n';
	else
		sValue = Misc::FormatArray(aValues, '\n'); 
}

CString TDCCADATA::FormatAsArray(TCHAR cSep) const
{
	if (cSep == '\n')
		return sData;
	
	CString sArray(sData);
	
	if (cSep != 0)
		sArray.Replace('\n', cSep);
	else
		sArray.Replace(_T("\n"), Misc::GetListSeparator());
	
	return sArray;
}

CString TDCCADATA::FormatAsDate(BOOL bISO) const
{
	if (IsEmpty())
		return sData;

	// else
	COleDateTime date(AsDate());
	DWORD dwFlags = DHFD_NOSEC;
	
	if (CDateHelper::DateHasTime(date))
		dwFlags |= DHFD_TIME;
	
	if (bISO)
		dwFlags |= DHFD_ISO;
	
	return CDateHelper::FormatDate(date, dwFlags);
}

CString TDCCADATA::FormatAsTimePeriod(int nDecimalPlaces) const
{
	TDC_UNITS nUnits;
	double dTime = AsTimePeriod(nUnits);

	return CTimeHelper().FormatTime(dTime, TDC::MapUnitsToTHUnits(nUnits), nDecimalPlaces);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CTDCCustomAttributeDataMap::Lookup(LPCTSTR key, TDCCADATA& rValue) const
{
	return CMap<CString, LPCTSTR, TDCCADATA, TDCCADATA&>::Lookup(Misc::ToUpper(key), rValue);
}

TDCCADATA& CTDCCustomAttributeDataMap::operator[](LPCTSTR key)
{
	return CMap<CString, LPCTSTR, TDCCADATA, TDCCADATA&>::operator[](Misc::ToUpper(key));
}

void CTDCCustomAttributeDataMap::SetAt(LPCTSTR key, TDCCADATA& newValue)
{ 
	(*this)[key] = newValue; 
}

void CTDCCustomAttributeDataMap::Copy(const CTDCCustomAttributeDataMap& mapData)
{
	Misc::CopyStrT<TDCCADATA>(mapData, *this);
}

BOOL CTDCCustomAttributeDataMap::MatchAll(const CTDCCustomAttributeDataMap& mapData) const
{
	return Misc::MatchAllStrT<TDCCADATA>(*this, mapData);
}

/////////////////////////////////////////////////////////////////////////////

