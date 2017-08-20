/*************************************************************************************\
hvkvpreader.h (Hyper-V KVP Interface)
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

#ifndef __HVKVPREADER_H_INCLUDED
#define __HVKVPREADER_H_INCLUDED 1

#include "hvkvpfile.h"

enum class KVPReadPools { HostToGuest, GuestToHost, HostToGuestIntrinsic };

class HVKVPReader : public HVKVPFile
{
public:
	KVPReadPools Pool = KVPReadPools::HostToGuestIntrinsic;
	bool InverseMatch = false;
	std::vector<HVKVPRecord> ReadKVPs(const std::string& KeyNameFilter = std::string());
};

#endif // __HVKVPREADER_H_INCLUDED