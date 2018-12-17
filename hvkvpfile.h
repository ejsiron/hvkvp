/*
hvkvpfile.h (Hyper-V KVP Interface)
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
	const long GetFileSize(std::string FileName);
	HVKVPFile() {};
	const bool KeyMatches(const std::string& TestKey, const bool InvertMatch = false) const;
public:
	bool MatchExact = false;
	bool CaseInsensitive = false;
};

#endif // __HVKVPFILE_H_INCLUDED