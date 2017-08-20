/*************************************************************************************\
hvkvpremover.cpp (Hyper-V KVP Interface)
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
#include <unistd.h>
#include "hvkvpfile.h"
#include "hvkvpremover.h"

using std::string;

static int TruncateFile(const string& FileName, std::streampos NewLength)
{
	return truncate(FileName.c_str(), NewLength);
}

const bool HVKVPRemover::IsDuplicateKey(std::vector<string>& FoundKeys, const string& KeyToCheck)
{
	errno = 0;
	std::regex DuplicatesFilter(KeyToCheck, (CaseInsensitive ? std::regex_constants::icase : std::regex_constants::basic));
	for (auto FoundKey : FoundKeys)
	{
		if (regex_match(FoundKey, DuplicatesFilter)) { return true; }
	}
	FoundKeys.push_back(KeyToCheck);
	return false;
}

void HVKVPRemover::LocateValidKVPLocations(std::fstream& KVPFile)
{
	errno = 0;

	// Hyper-V only reads the LAST instance of a key. To facilitate duplicate removal, work backward to gather information
	long RecordLength = HVKVPRecord::DataSize();
	long InitialFileSize = GetFileSize(FileName);

	if ((InitialFileSize % RecordLength) && !RemoveInvalid)
	{
		errno = EUCLEAN;
		return;
	}

	SetFilter(KeyToRemove);
	HVKVPRecord CurrentKVP;
	std::vector<std::string> FoundKeys;
	std::streampos RecordStartingPosition;

	bool Keep;

	KVPFile.seekg(0, std::fstream::end);
	while (KVPFile.seekg(-RecordLength, KVPFile.cur))
	{
		Keep = false;
		RecordStartingPosition = KVPFile.tellg();
		KVPFile >> CurrentKVP;
		if (CurrentKVP.bad()) { Keep = !RemoveInvalid; }
		else
		{
			if (!KeyMatches(CurrentKVP.Key(), InverseMatch))
			{	// xor would be cleaner but would force the duplicate check to be run when unnecessary
				if (!RemoveDuplicates || !IsDuplicateKey(FoundKeys, CurrentKVP.Key())) { Keep = true; }
			}	// else: this is the key that the user asked to remove, so do nothing; this will leave "Keep" unset, effectively marking this KVP for deletion
		}

		if (Keep) { ValidKVPLocations.push_back(RecordStartingPosition); }
		if (!KVPFile.bad()) { KVPFile.clear(); }
		KVPFile.seekg(RecordStartingPosition, KVPFile.beg);
	}
	if (errno == EINVAL) { errno = 0; } // the while loop terminates when we attempt to set seek to a negative position, which causes errno to be set to EINVAL
}

void HVKVPRemover::RemoveInvalidLocations(std::fstream & KVPFile)
{
	// use each valid location as a source to sequentially overwrite file, then truncate the file
	KVPFile.clear();
	HVKVPRecord SourceKVP;
	long RecordLength = HVKVPRecord::DataSize();
	std::vector<std::streampos>::reverse_iterator ValidLocations = ValidKVPLocations.rbegin();
	for (unsigned long i = 0; i < ValidKVPLocations.size(); i++)
	{
		KVPFile.seekg(ValidLocations[i], KVPFile.beg);
		KVPFile >> SourceKVP;
		if (!KVPFile.bad()) { KVPFile.clear(); }
		if (KVPFile.good())
		{
			KVPFile.seekp((i * RecordLength), KVPFile.beg);
			KVPFile << SourceKVP;
		}
	}
	if (!KVPFile.bad()) { KVPFile.clear(); }
	if (KVPFile.good())
	{
		std::streampos FileLength = KVPFile.tellp();
		KVPFile.close();
		TruncateFile(FileName, FileLength);
	}
}

void HVKVPRemover::RemoveKVP(const string& inKeyName, const bool inRemoveDuplicates, const bool inRemoveInvalid)
{
	errno = 0;
	KeyToRemove = inKeyName;
	RemoveDuplicates = inRemoveDuplicates;
	RemoveInvalid = inRemoveInvalid;
	
	std::fstream KVPFile(FileName, std::ios::binary | std::ios::in | std::ios::out);
	if (KVPFile.is_open())
	{
		LocateValidKVPLocations(KVPFile);
		if (errno) { throw(errno); }
		RemoveInvalidLocations(KVPFile);
	}
	if (errno) { throw(errno); }
	return;
}

void HVKVPRemover::RemoveAll()
{
	errno = 0;
	std::ofstream KVPFile(FileName);
}