/*************************************************************************************\
hvkvp.h (Hyper-V KVP Interface)
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

#ifndef __HVKVP_H_INCLUDED
#define __HVKVP_H_INCLUDED 1

#include <array>
#include <fstream>
#include <linux/hyperv.h>

#define HV_KVP_EXCHANGE_FIELDSIZE (HV_KVP_EXCHANGE_MAX_KEY_SIZE + HV_KVP_EXCHANGE_MAX_VALUE_SIZE)

#define HVKVP_DEFAULT_KVP_DIRECTORY	"/var/lib/hyperv/"

#define HVKVP_BASEFILENAME			".kvp_pool_"

#define HVKVP_POOL_EXTERNAL_ID			"0"		// host-to-guest pool populated by a user
#define HVKVP_POOL_GUEST_ID				"1"		// guest-to-host pool populated by a user
#define HVKVP_POOL_GUESTPARAMETER_ID	"3"		// host-to-guest pool auto-populated by the Hyper-V host

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
	std::streampos SourceStreamPosition = -1;

public:
	static const bool hvkvp_isgood(const hvkvp& KVP);
	const bool good() const { return hvkvp_isgood(KVP); }
	const bool bad() const { return !good(); }
	void Clear();
	void ClearKey();
	void ClearValue();
	const char* SerializedData() const { return KVP.raw.data(); }
	static const size_t DataSize() { return HV_KVP_EXCHANGE_FIELDSIZE; }
	const char* SerializedKey() const { return KVP.pair.key.data(); }
	static const size_t KeySize() { return HV_KVP_EXCHANGE_MAX_KEY_SIZE; }
	const char* SerializedValue() const { return KVP.pair.value.data(); }
	static const size_t ValueSize() { return HV_KVP_EXCHANGE_MAX_VALUE_SIZE; }
	void Deserialize(const std::array<char, HV_KVP_EXCHANGE_FIELDSIZE>& FileBytes, const std::streampos FileOffset);
	const std::string Key() const { return std::string(KVP.pair.key.data()); }
	void SetKey(const std::string& val);
	const std::string Value() const { return std::string(KVP.pair.value.data()); }
	void SetValue(const std::string& val);
	friend std::istream& operator>>(std::istream& InputStream, HVKVPRecord& kvpin);
	friend std::ostream& operator<<(std::ostream& OutputStream, const HVKVPRecord& kvpout);
	friend std::fstream& operator<<(std::fstream& OutputFileStream, const HVKVPRecord& kvpout);

	HVKVPRecord(const std::string& NewKey = std::string(), const std::string& NewValue = std::string());
	HVKVPRecord& operator=(const HVKVPRecord&);
	~HVKVPRecord();
};

#endif // __HVKVP_H_INCLUDED