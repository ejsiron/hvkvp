/*************************************************************************************\
hvkvp.cpp (Hyper-V KVP Interface)
Copyright(C) 2017  Eric Siron

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.
\*************************************************************************************/

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

	bool NullFound = false;
	for (int CheckPos = 1; CheckPos < HV_KVP_EXCHANGE_FIELDSIZE; CheckPos++)
	{
		if (CheckPos == HV_KVP_EXCHANGE_MAX_KEY_SIZE) { NullFound = false; }	// Reset null locator because this is the first position of the value
		if (KVP.raw[CheckPos] == '\0') { NullFound = true; }
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

void HVKVPRecord::Deserialize(const std::array<char, HV_KVP_EXCHANGE_FIELDSIZE>& FileBytes, const std::streampos FileOffset)
{
	std::copy(FileBytes.begin(), FileBytes.end(), KVP.raw.begin());
	SourceStreamPosition = FileOffset;
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
	if (val.length() > 0)
	{
		auto endtext = val.end();
		if (val.length() > HV_KVP_EXCHANGE_MAX_VALUE_SIZE)
		{
			endtext = val.begin() + HV_KVP_EXCHANGE_MAX_VALUE_SIZE;
		}
		copy(val.begin(), endtext, KVP.pair.value.begin());
	}
}

HVKVPRecord::HVKVPRecord(const string& NewKey, const string& NewValue)
{
	SetKey(NewKey); SetValue(NewValue);
}

HVKVPRecord& HVKVPRecord::operator=(const HVKVPRecord& rhs)
{
	if (this != &rhs)
	{
		SourceStreamPosition = rhs.SourceStreamPosition;
		std::copy(rhs.KVP.raw.begin(), rhs.KVP.raw.end(), KVP.raw.begin());
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
	KVPtemp.SourceStreamPosition = InputStream.tellg();
	for (unsigned long i = 0; i < KVPtemp.KVP.raw.size(); i++)
	{
		if (!InputStream || InputStream.bad()) { return InputStream; }
		InputStream >> std::noskipws >> KVPtemp.KVP.raw[i];
	}

	kvpin = KVPtemp;

	return InputStream;
}

std::ostream& operator<<(std::ostream& OutputStream, const HVKVPRecord& kvpout)
{
	OutputStream << std::noskipws << string(kvpout.KVP.pair.key.data()) << ": " << string(kvpout.KVP.pair.value.data());
	return OutputStream;
}

std::fstream & operator<<(std::fstream& OutputFileStream, const HVKVPRecord& kvpout)
{
	for (size_t i = 0; i < kvpout.DataSize(); i++)
	{
		OutputFileStream.put(kvpout.KVP.raw.data()[i]);
	}
	return OutputFileStream;
}
