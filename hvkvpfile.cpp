/*
hvkvpfile.cpp (Hyper-V KVP Interface)
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

#include <sys/types.h>
#include <sys/stat.h>
#include "hvkvpfile.h"

// GCC versions < 4.9 ship with a partially-implemented version of regex that will silently fail
#ifdef __GNUC__
static_assert(
	__GNUC__ > 4 ||
		(__GNUC__ == 4 && (__GNUC_MINOR__ > 8 ||
			(__GNUC_MINOR__ == 8 &&	__GNUC_PATCHLEVEL__ > 5))), "Compile with GCC version > 4.8.5");
#endif

using std::string;

const long HVKVPFile::GetFileSize(string FileName)
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

		NewFilter.assign(FilterString);

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
	if (MatchExact)
	{
		return InvertMatch xor std::regex_match(TestKey, Filter);
	}
	else
	{
		return InvertMatch xor std::regex_search(TestKey, Filter);
	}
}
