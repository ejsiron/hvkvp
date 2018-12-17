/*
hvkvpremover.h (Hyper-V KVP Interface)
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

#ifndef __HVKVPREMOVER_H_INCLUDED
#define __HVKVPREMOVER_H_INCLUDED 1

class HVKVPRemover : public HVKVPFile
{
private:
	std::string FileName{ HVKVP_FILE_TOHOST }; // can only remove KVPs from the guest to host file
	std::string KeyToRemove{};
	bool RemoveDuplicates{ false };
	bool RemoveInvalid{ false };
	std::vector<std::streampos> ValidKVPLocations{};
	const bool IsDuplicateKey(std::vector<std::string>& FoundKeys, const std::string& KeyToCheck);
	void LocateValidKVPLocations(std::fstream& KVPFile);
	void RemoveInvalidLocations(std::fstream& KVPFile);
public:
	HVKVPRemover() { MatchExact = true; }
	bool InverseMatch{ false };
	void RemoveKVP(const std::string& KeyName, const bool CleanDuplicates = false, const bool CleanInvalid = false);
	void RemoveAll();
};

#endif // __HVKVPREMOVER_H_INCLUDED