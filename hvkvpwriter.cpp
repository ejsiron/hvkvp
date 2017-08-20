/*************************************************************************************\
hvkvpwriter.cpp (Hyper-V KVP Interface)
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

#include "hvkvpwriter.h"

using std::string;

void HVKVPWriter::WriteKVP(const string& InputKey, const string& NewValue)
{
	errno = 0;
	if (InputKey.empty())
	{
		throw(EINVAL);
	}
	string FileName(HVKVP_FILE_TOHOST);
	long InitialFileSize = GetFileSize(FileName);
	if (errno) { return; }
	std::fstream KVPFile(FileName, std::ios::binary | std::ios::in | std::ios::out);
	if (KVPFile.is_open())
	{
		// Hyper-V only reads the LAST value for a key. Overwrite ALL instances in case duplicates exist. If none exist, append.
		HVKVPRecord NewKVP(InputKey, NewValue);
		HVKVPRecord CurrentKVP;
		SetFilter(InputKey);
		bool KVPWasOverwritten = false;
		long CurrentKVPStartingPosition = 0;
		while (KVPFile.good() && CurrentKVPStartingPosition < InitialFileSize)
		{
			KVPFile >> CurrentKVP;
			if(KeyMatches(CurrentKVP.Key()))
			{
				KVPFile.seekp(CurrentKVPStartingPosition + CurrentKVP.KeySize());
				KVPFile.write(NewKVP.SerializedValue(), NewKVP.ValueSize());
				KVPWasOverwritten = true;
			}
			
			CurrentKVPStartingPosition = KVPFile.tellg();
		}
		
		if (!KVPWasOverwritten && !KVPFile.bad()) // eof and probably fail will be set, but bad() is bad
		{
			KVPFile.clear();
			KVPFile.seekg(0, std::fstream::end);
			KVPFile << NewKVP;
		}
	}
	if (errno) { throw errno; }
}
