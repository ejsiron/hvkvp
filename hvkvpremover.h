/*************************************************************************************\
hvkvpremover.h (Hyper-V KVP Interface)
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

#ifndef __HVKVPREMOVER_H_INCLUDED
#define __HVKVPREMOVER_H_INCLUDED 1

class HVKVPRemover : public HVKVPFile
{
private:
	std::string FileName = HVKVP_FILE_TOHOST; // at this time, we can only remove KVPs from the guest to host file
	std::string KeyToRemove;
	bool RemoveDuplicates;
	bool RemoveInvalid;
	std::vector<std::streampos> ValidKVPLocations;
	const bool IsDuplicateKey(std::vector<std::string>& FoundKeys, const std::string& KeyToCheck);
	void LocateValidKVPLocations(std::fstream& KVPFile);
	void RemoveInvalidLocations(std::fstream& KVPFile);
public:
	HVKVPRemover() { MatchExact = true; }
	bool InverseMatch = false;
	void RemoveKVP(const std::string& KeyName, const bool CleanDuplicates = false, const bool CleanInvalid = false);
	void RemoveAll();
};

#endif // __HVKVPREMOVER_H_INCLUDED