/*
MIT License

Copyright(c) 2018 Eric Siron

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

#ifndef __HVKVP_H_INCLUDED
#define __HVKVP_H_INCLUDED 1

#include <array>
#include <iosfwd>
#include <linux/hyperv.h>

#define HV_KVP_EXCHANGE_FIELDSIZE (HV_KVP_EXCHANGE_MAX_KEY_SIZE + HV_KVP_EXCHANGE_MAX_VALUE_SIZE)

#define HVKVP_DEFAULT_KVP_DIRECTORY	"/var/lib/hyperv/"

#define HVKVP_BASEFILENAME			".kvp_pool_"

#define HVKVP_POOL_EXTERNAL_ID					"0"		// host-to-guest pool populated by a user
#define HVKVP_POOL_GUEST_ID						"1"		// guest-to-host pool populated by a user
#define HVKVP_POOL_GUESTPARAMETER_ID			"3"		// host-to-guest pool auto-populated by the Hyper-V host

#define HVKVP_FILE_TOGUEST				HVKVP_DEFAULT_KVP_DIRECTORY HVKVP_BASEFILENAME HVKVP_POOL_EXTERNAL_ID
#define HVKVP_FILE_TOHOST				HVKVP_DEFAULT_KVP_DIRECTORY HVKVP_BASEFILENAME HVKVP_POOL_GUEST_ID
#define HVKVP_FILE_GUESTPARAMETER	HVKVP_DEFAULT_KVP_DIRECTORY HVKVP_BASEFILENAME HVKVP_POOL_GUESTPARAMETER_ID

union hvkvp
{
	struct kvp
	{
		std::array<char, HV_KVP_EXCHANGE_MAX_KEY_SIZE> key;
		std::array<char, HV_KVP_EXCHANGE_MAX_VALUE_SIZE> value;
	} pair;
	std::array<char, HV_KVP_EXCHANGE_FIELDSIZE> raw;
};

class HVKVPRecord
{
private:
	hvkvp KVP;

public:
	static const bool hvkvp_isgood(const hvkvp& KVP);
	const bool good() const { return hvkvp_isgood(KVP); }
	const bool bad() const { return !good(); }
	void Clear();
	void ClearKey();
	void ClearValue();
	const char* SerializedData() const { return KVP.raw.data(); }
	static constexpr size_t DataSize() { return HV_KVP_EXCHANGE_FIELDSIZE; }
	const char* SerializedKey() const { return KVP.pair.key.data(); }
	static constexpr size_t KeySize() { return HV_KVP_EXCHANGE_MAX_KEY_SIZE; }
	const char* SerializedValue() const { return KVP.pair.value.data(); }
	static constexpr size_t ValueSize() { return HV_KVP_EXCHANGE_MAX_VALUE_SIZE; }
	const std::string Key() const { return std::string(KVP.pair.key.data()); }
	void SetKey(const std::string& val);
	const std::string Value() const { return std::string(KVP.pair.value.data()); }
	void SetValue(const std::string& val);
	friend std::istream& operator>>(std::istream& InputStream, HVKVPRecord& kvpin);
	friend std::ostream& operator<<(std::ostream& OutputStream, const HVKVPRecord& kvpout);
	friend std::fstream& operator<<(std::fstream& OutputFileStream, const HVKVPRecord& kvpout);

	HVKVPRecord(const std::string& NewKey = std::string(), const std::string& NewValue = std::string());
	HVKVPRecord(const HVKVPRecord& rhs);
	HVKVPRecord& operator=(const HVKVPRecord& rhs);
	HVKVPRecord(HVKVPRecord&& rhs);
	HVKVPRecord& operator=(HVKVPRecord&& rhs);
	~HVKVPRecord();
};

#endif // __HVKVP_H_INCLUDED