/*************************************************************************************\
main.cpp (Hyper-V KVP Interface)
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

#include <getopt.h>
#include <iostream>
#include "hvkvpreader.h"
#include "hvkvpremover.h"
#include "hvkvpwriter.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

string GeneralHelp =
"Hyper-V KVP Interface      (c) 2017 Eric Siron\n"
"Usage: hvkvp COMMAND [OPTIONS]\n\n"
"Available Commands\n"
"------------------\n"
"read                       Read KVP(s)\n"
"write                      Write a KVP\n"
"delete                     Delete KVP(s)\n\n"
"Options\n"
"-------\n"
"-k, --key KEY              Key to operate on\n"
"-s, --keyset KEYSET        out: Read guest-to-host key(s)\n"
"                           in: Read host-to-guest key(s)\n"
"                           params: Read auto-set parameter(s)\n"
"-v, --value [VALUE]        Value for key\n"
"-i, --ignorecase           Ignore UPPERCASE/lowercase of key(s)\n"
"-m, --exactmatch on|off    on: key must match exactly (doesn't override -i)\n"
"                           off: partial key match (USE WITH CAUTION!)\n"
"-n, --invertmatch          Operate on keys that DO NOT match -k.\n"
"                           Can combine with -i and -m\n"
"-x, --noout                No output after \"write\" or \"delete\"\n"
"-h, --help                 Show help. Use with a command to see examples\n"
;

string ReadHelp =
"Hyper-V KVP Interface      (c) 2017 Eric Siron\n"
"Usage: hvkvp read [OPTIONS]\n"
"Options (Ignores -v/--value and -x/--noout parameters)\n"
"-------\n"
"-k, --key [KEY]            Key to operate on\n"
"-s, --keyset KEYSET        out: Read guest-to-host key(s)\n"
" (Default in)              in: Read host-to-guest key(s)\n"
"                           params: Read auto-set parameter(s)\n"
"-i, --ignorecase           Ignore UPPERCASE/lowercase of key(s)\n"
"-m, --exactmatch on|off    on: key must match exactly (doesn't override -i)\n"
"                           off: partial key match (default for read)\n"
"-n, --invertmatch          Operate on keys that DO NOT match -k.\n"
"                           Can combine with -i and -m\n"
"-x, --noout                No output after \"read\" after \"write\" or \"delete\"\n"
"-h, --help                 Show this help text\n"
"Examples\n"
"hvkvp read -s out\n"
": Reads all KVPs in the guest-to-host list\n\n"
"hvkvp read -k VirtualMachineId\n"
": Reads the \"VirtualMachineId\" field from the host\n\n"
"hvkvp read -k virtualmachineid -i\n"
": Reads the \"VirtualMachineId\" field from the host\n"
;

string WriteHelp =
"Hyper-V KVP Interface      (c) 2017 Eric Siron\n"
"Usage: hvkvp write [OPTIONS]\n"
"Options (Ignores -n/--invertmatch and -s/--keyset parameter\nWorks with guest-to-host only!\n"
"-------\n"
"-k, --key KEY              Key to operate on\n"
"-v, --value [VALUE]        Value for key\n"
"-i, --ignorecase           Ignore UPPERCASE/lowercase of key(s)\n"
"-m, --exactmatch on|off    on: key must match exactly (default for write)\n"
"                           off: partial key match (USE WITH CAUTION!)\n"
"                           Can combine with -i and -m\n"
"-x, --noout                No output after \"write\" or \"delete\"\n"
"-h, --help                 Show this help text\n"
"Examples\n"
"hvkvp write -k GuestReport -v \"My report\"\n"
": Creates or overwrites key \"GuestReport\" with value \"My Report\"\n\n"
"hvkvp write -k guestreport -v \"My report\" -i\n"
": Creates/overwrites key \"GuestReport\", ignoring case, with value \"My Report\"\n\n"
"hvkvp write -k guestrep -v \"My report\" -i -m off\n"
": Creates/Overwrites any key containing \"guestrep\", ignoring case,\n  with value \"My Report\"\n"
;

string DeleteHelp =
"Hyper-V KVP Interface      (c) 2017 Eric Siron\n"
"Usage: hvkvp delete [OPTIONS]\n"
"Options (Ignores -s/--keyset parameter; works with guest-to-host only)\n"
"-------\n"
"-k, --key KEY              Key to operate on\n"
"-v, --value [VALUE]        Value for key\n"
"-i, --ignorecase           Ignore UPPERCASE/lowercase of key(s)\n"
"-m, --exactmatch on|off    on: key must match exactly (default for remove)\n"
"                           off: partial key match (USE WITH CAUTION!)\n"
"-n, --invertmatch          Operate on keys that DO NOT match -k.\n"
"                           Can combine with -i and -m\n"
"-x, --noout                No output after \"write\" or \"delete\"\n"
"-c, --cleaninvalid         Detects and removes invalid KVPs\n"
"-d, --cleanduplicates      Detects and removes duplicate KVPs\n\n"
"-A, --DELETE_ALL           Deletes all guest-to-host KVPs\n"
"-h, --help                 Show this help text\n"
"Examples\n"
"hvkvp delete -k GuestReport\n"
": Deletes KVP with key \"GuestReport\"\n\n"
"hvkvp delete -k guestreport -ni\n"
": Deletes all KVPs EXCEPT \"GuestReport\", ignoring case\n\n"
"hvkvp write -ci\n"
": Deletes all invalid and duplicate KVPs\n"
;

enum opmodes
{
	None,
	Read,
	Write,
	Delete
};

int main(int argc, char **argv)
{
	string Key("");
	string Value("");
	int HelpRequested = 0;
	int CaseInsensitive = 0;
	bool MatchExactOverride = false;
	int MatchExact = 0;
	int InvertMatch = 0;
	int NoOutput = 0;
	int CleanInvalid = 0;
	int CleanDuplicates = 0;
	int DeleteAll = 0;
	KVPReadPools ReadSource = KVPReadPools::HostToGuest;

	static struct option long_options[] = {
		{ "help",				no_argument, &HelpRequested, 1 },
		{ "keyset",				required_argument, 0, 's' },
		{ "key",					required_argument, 0, 'k' },
		{ "value",				required_argument, 0, 'v' },
		{ "ignorecase",		no_argument, &CaseInsensitive, 1 },
		{ "matchexact",		required_argument, 0, 'm' },
		{ "invertmatch",		no_argument, &InvertMatch, 1 },
		{ "noout",				no_argument, &NoOutput, 1 },
		{ "cleaninvalid",		no_argument, &CleanInvalid, 1 },
		{ "cleanduplicates",	no_argument, &CleanDuplicates, 1 },
		{ "DELETE_ALL",		no_argument, &DeleteAll, 1 },
		{0, 0, 0, 0}
	};

	int EnteredOptionCharacter;
	int EnteredOptionIndex;
	opmodes OpMode = None;

	if (argv[1])
	{
		if (!strncmp(argv[1], "read", 4)) { OpMode = Read; }
		else if (!strncmp(argv[1], "write", 5)) { OpMode = Write; }
		else if (!strncmp(argv[1], "delete", 6)) { OpMode = Delete; }
	}

	while (EnteredOptionCharacter != -1)
	{
		EnteredOptionCharacter = getopt_long(argc, argv, "hs:k:v:im:nxcdA", long_options, &EnteredOptionIndex);
		switch (EnteredOptionCharacter)
		{
		case 'k':
			if (optarg) { Key.assign(optarg); }
			break;
		case 's':
			if (optarg)
			{
				switch (optarg[0])
				{
				case 'o':
				case 'O':
					ReadSource = KVPReadPools::GuestToHost;
					break;
				case 'p':
				case 'P':
					ReadSource = KVPReadPools::HostToGuestIntrinsic;
					break;
				default:
					ReadSource = KVPReadPools::HostToGuest;
					break;
				}
			}
			break;
		case 'v':
			Value.assign(optarg);
			break;
		case 'i':
			CaseInsensitive = 1;
			break;
		case 'm':
		{
			MatchExactOverride = true;
			string MatchArg = string(optarg);
			if (MatchArg == "on") { MatchExact = 1; }
			else if (MatchArg == "off") { MatchExact = 0; }
			else { HelpRequested = 1; }
		}
		break;
		case 'n':
			InvertMatch = 1;
			break;
		case 'x':
			NoOutput = 1;
			break;
		case 'c':
			CleanInvalid = 1;
			break;
		case 'd':
			CleanDuplicates = 1;
			break;
		case 'A':
			DeleteAll = 1;
			break;
		case -1:
			break;
		default:
			HelpRequested = true;
			break;
		}
	}

	if (OpMode == Write)
	{
		if (HelpRequested) { cout << WriteHelp; }
		else if (Key.empty())
		{
			cout << "Key name (-k/--key) must be specified for a write operation" << endl;
			return EINVAL;
		}
		else
		{
			HVKVPWriter Writer;
			Writer.CaseInsensitive = CaseInsensitive;
			if (MatchExactOverride) { Writer.MatchExact = MatchExact; }
			try
			{
				Writer.WriteKVP(Key, Value);
			}
			catch (int ErrorCode)
			{
				errno = ErrorCode;
			}
			catch (...)
			{
				cout << "Unknown error occurred during a write operation" << endl;
			}
		}
	}
	else if (OpMode == Delete)
	{
		if (HelpRequested) { cout << DeleteHelp; }
		else if (Key.empty() && !DeleteAll && !CleanDuplicates && !CleanInvalid)
		{
			cout << "Delete must include a key (-k/--key) and/or at least one of the following: \n"
				"   -c/--cleaninvalid\n   -d/--cleanduplicates\n   -A/--DELETE_ALL" << endl;
			return EINVAL;
		}
		else
		{
			HVKVPRemover Remover;
			if (DeleteAll)
			{
				try
				{
					Remover.RemoveAll();
				}
				catch (int ErrorCode)
				{
					errno = ErrorCode;
				}
				catch (...)
				{
					cout << "Unknown error occurred while attempting to delete all outbound KVPs" << endl;
				}
			}
			else
			{
				Remover.CaseInsensitive = CaseInsensitive;
				Remover.InverseMatch = InvertMatch;
				if (MatchExactOverride) { Remover.MatchExact = MatchExact; }
			}
			try
			{
				Remover.RemoveKVP(Key, CleanDuplicates, CleanInvalid);
			}
			catch (int ErrorCode)
			{
				errno = ErrorCode;
			}
			catch (...)
			{
				cout << "Unknown error occurred during a delete operation" << endl;
			}
		}
	}
	else if (OpMode == None)
	{
		cout << GeneralHelp;
	}

	// other operations must fall through to an opportunity to read
	if (!errno && OpMode != None && !NoOutput)
	{
		HVKVPReader Reader;
		if (HelpRequested)
		{
			cout << ReadHelp;
		}
		else
		{
			if (OpMode == Read)
			{
				Reader.CaseInsensitive = CaseInsensitive;
				Reader.InverseMatch = InvertMatch;
				if (MatchExactOverride) { Reader.MatchExact = MatchExact; }
				Reader.Pool = ReadSource;
			}
			else
			{
				// we only get here if a write or delete operation occurred, and those can only target G2H
				Reader.Pool = KVPReadPools::GuestToHost;
				Key.assign("");
			}
			try
			{
				for (auto KVP : Reader.ReadKVPs(Key))
				{
					cout << KVP << endl;
				}
			}
			catch (int ErrorCode)
			{
				errno = ErrorCode;
			}
			catch (...)
			{
				cout << "Unknown error occurred during a read operation" << endl;
			}
		}
	}

	if (errno) { cerr << strerror(errno) << endl; }
	return errno;
}
