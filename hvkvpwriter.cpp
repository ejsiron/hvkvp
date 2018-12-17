/*
hvkvpwriter.cpp (Hyper-V KVP Interface)
Copyright(C) 2017  Eric Siron

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
#include "hvkvpfile.h"
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
	long InitialFileSize{ GetFileSize(FileName) };
	if (errno) { return; }
	std::fstream KVPFile(FileName, std::ios::binary | std::ios::in | std::ios::out);
	if (KVPFile.is_open())
	{
		// Hyper-V only reads the LAST value for a key. Overwrite ALL instances in case duplicates exist. If none exist, append.
		HVKVPRecord NewKVP(InputKey, NewValue);
		HVKVPRecord CurrentKVP;
		SetFilter(InputKey);
		bool KVPWasOverwritten{ false };
		long CurrentKVPStartingPosition{ 0 };
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
