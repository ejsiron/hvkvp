/*************************************************************************************\
hvkvpfile.cpp (Hyper-V KVP Interface)
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

#include <sys/types.h>
#include <sys/stat.h>
#include "hvkvpfile.h"

// GCC versions < 4.9 ship with a partially-implemented version of regex
#if __GNUC__ < 4 || \
				  (__GNUC__ == 4 && (__GNUC_MINOR__ < 8 || \
											(__GNUC_MINOR__ == 8 && \
											 __GNUC_PATCHLEVEL__ <= 5)))

#define USE_OLD_MATCH_STYLE 1
#endif

using std::string;

long HVKVPFile::GetFileSize(string FileName)
{
	struct stat FileStatistics;
	if (stat(FileName.c_str(), &FileStatistics) == -1)
	{
		throw(errno);
	}
	return FileStatistics.st_size;
}

void HVKVPFile::SetFilter(const string& FilterString)
{
	if (FilterString.length())
	{
		UseFilter = true;
		string NewFilter;

#ifdef USE_OLD_MATCH_STYLE
		// because we can't pawn our case-sensitivity issues off on regex, upper-case the string. works in most cases
		if (CaseInsensitive)
		{
			for (auto ch : FilterString)
			{
				NewFilter.push_back((char)toupper(ch));
			}
		}
		else
		{
			NewFilter.assign(FilterString);
		}
#else
		NewFilter.assign(FilterString);
#endif
		if (!MatchExact) { NewFilter = ".*" + NewFilter + ".*"; } // regex_search is not universally implemented, either. this always works, and is cheap enough for character strings of these sizes
		Filter = std::regex(NewFilter, (CaseInsensitive ? std::regex_constants::icase : std::regex_constants::basic));
	}
	else
	{
		UseFilter = false;
		Filter = std::regex();
	}
}

const bool HVKVPFile::KeyMatches(const string& TestKey, bool InvertMatch) const
{
#ifdef USE_OLD_MATCH_STYLE
	string CasedKey;
	if (CaseInsensitive)
	{
		for (auto ch : TestKey)
		{
			CasedKey.push_back((char)toupper(ch));
		}
	}
	else
	{
		CasedKey = TestKey;
	}
	return InvertMatch xor std::regex_match(CasedKey, Filter);
#else
	return InvertMatch xor std::regex_match(TestKey, Filter);
#endif
}
