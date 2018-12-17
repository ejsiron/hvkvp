/*
hvkvpreader.cpp (Hyper-V KVP Interface)
Copyright(C) 2018  Eric Siron

MIT License

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
#include <vector>
#include "hvkvpreader.h"

using std::string;

static string SelectSourceFile(KVPReadPools SourcePool)
{
	string SourceFile;
	switch (SourcePool)
	{
	case KVPReadPools::GuestToHost:
		SourceFile = HVKVP_FILE_TOHOST;
		break;
	case KVPReadPools::HostToGuest:
		SourceFile = HVKVP_FILE_TOGUEST;
		break;
	default:
		SourceFile = HVKVP_FILE_GUESTPARAMETER;
		break;
	}
	return SourceFile;
}

std::vector<HVKVPRecord> HVKVPReader::ReadKVPs(const string& KeyNameFilter)
{
	std::vector<HVKVPRecord> FoundKVPs;
	string SourceFile = SelectSourceFile(Pool);
	std::ifstream KVPSourceFile(SourceFile, std::ios::binary);
	if (KVPSourceFile.is_open())
	{
		SetFilter(KeyNameFilter);
		HVKVPRecord KVPIn;
		while (KVPSourceFile >> KVPIn)
		{
			if (!UseFilter || (KeyMatches(KVPIn.Key(), InverseMatch)))
			{
				FoundKVPs.emplace_back(KVPIn);
			}
		}
	}
	else
	{
		throw(strerror(errno));
	}
	return FoundKVPs;
}