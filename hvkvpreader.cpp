/*************************************************************************************\
hvkvpreader.cpp (Hyper-V KVP Interface)
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
				FoundKVPs.push_back(KVPIn);
			}
		}
	}
	else
	{
		throw(strerror(errno));
	}
	return FoundKVPs;
}