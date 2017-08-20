/*************************************************************************************\
hvkvpfile.h (Hyper-V KVP Interface)
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

#ifndef __HVKVPFILE_H_INCLUDED
#define __HVKVPFILE_H_INCLUDED 1

#include <regex>
#include "hvkvp.h"

class HVKVPFile
{
private:
	std::regex Filter;
protected:
	bool UseFilter;
	void SetFilter(const std::string& FilterString);
	long GetFileSize(std::string FileName);
	HVKVPFile() {};
	const bool KeyMatches(const std::string& TestKey, const bool InvertMatch = false) const;
public:
	bool MatchExact = false;
	bool CaseInsensitive = false;
};

#endif // __HVKVPFILE_H_INCLUDED