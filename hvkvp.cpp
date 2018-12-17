/*
MIT License

Copyright(c) 2018 Eric Siron

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <fstream>
#include <iostream>
#include "hvkvp.h"

using std::string;

const bool HVKVPRecord::hvkvp_isgood(const hvkvp& KVP)
{
	/***************************\
	1: key and value start with non-null characters and end with null characters; no null characters before non-null
	2: key cannot be all null (at least 1st character must be non-null)
	3: value can be all null
	\***************************/
	if (KVP.raw[0] == '\0') { return false; }

	bool NullFound{ false };
	for (int CheckPos{ 1 }; CheckPos < HV_KVP_EXCHANGE_FIELDSIZE; ++CheckPos)
	{
		if (CheckPos == HV_KVP_EXCHANGE_MAX_KEY_SIZE) { NullFound = false; }	// Reset null locator because this is the first position of the value
		if (KVP.raw[CheckPos] == 0) { NullFound = true; }
		else { if (NullFound) { return false; } }
	}
	return true;
}

void HVKVPRecord::Clear()
{
	ClearKey();
	ClearValue();
}

void HVKVPRecord::ClearKey()
{
	std::fill(KVP.pair.key.begin(), KVP.pair.key.end(), 0);
}

void HVKVPRecord::ClearValue()
{
	std::fill(KVP.pair.value.begin(), KVP.pair.value.end(), 0);
}

void HVKVPRecord::SetKey(const string& val)
{
	ClearKey();
	if (val.length() > 0)
	{
		auto endtext = val.end();
		if (val.length() > HV_KVP_EXCHANGE_MAX_KEY_SIZE)
		{
			endtext = val.begin() + HV_KVP_EXCHANGE_MAX_KEY_SIZE;
		}
		copy(val.begin(), endtext, KVP.pair.key.begin());
	}
}

void HVKVPRecord::SetValue(const string& val)
{
	ClearValue();
	auto endtext = val.length() > HV_KVP_EXCHANGE_MAX_VALUE_SIZE ?
		val.begin() + HV_KVP_EXCHANGE_MAX_VALUE_SIZE
		:
		val.end();
	copy(val.begin(), endtext, KVP.pair.value.begin());
}

HVKVPRecord::HVKVPRecord(const string& NewKey, const string& NewValue)
{
	SetKey(NewKey); SetValue(NewValue);
}

HVKVPRecord::HVKVPRecord(const HVKVPRecord& rhs)
{
	*this = rhs;
}

HVKVPRecord& HVKVPRecord::operator=(const HVKVPRecord& rhs)
{
	if (this != &rhs)
	{
		std::copy(rhs.KVP.raw.begin(), rhs.KVP.raw.end(), KVP.raw.begin());
	}
	return *this;
}

HVKVPRecord::HVKVPRecord(HVKVPRecord&& rhs)
{
	*this = rhs;
}

HVKVPRecord& HVKVPRecord::operator=(HVKVPRecord&& rhs)
{
	if (this != &rhs)
	{
		std::swap(this->KVP, rhs.KVP);
	}
	return *this;
}

HVKVPRecord::~HVKVPRecord()
{
	Clear();
}

std::istream& operator>>(std::istream& InputStream, HVKVPRecord& kvpin)
{
	HVKVPRecord KVPtemp;
	for (unsigned long i{ 0 }; i < KVPtemp.KVP.raw.size(); ++i)
	{
		if (!InputStream || InputStream.bad()) { return InputStream; }
		InputStream >> std::noskipws >> KVPtemp.KVP.raw[i];
	}

	kvpin = KVPtemp;

	return InputStream;
}

std::ostream& operator<<(std::ostream& OutputStream, const HVKVPRecord& kvpout)
{
	return OutputStream << std::noskipws << string(kvpout.KVP.pair.key.data()) << ": " << string(kvpout.KVP.pair.value.data());
}

std::fstream & operator<<(std::fstream& OutputFileStream, const HVKVPRecord& kvpout)
{
	for (size_t i{ 0 }; i < kvpout.DataSize(); ++i)
	{
		OutputFileStream.put(kvpout.KVP.raw.data()[i]);
	}
	return OutputFileStream;
}
