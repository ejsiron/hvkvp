# hvkvp
Use in Linux guests under Hyper-V to quickly and easily send and receive information with the management operating system using the [Hyper-V Data Exchange Service](https://docs.microsoft.com/en-us/virtualization/hyper-v-on-windows/reference/integration-services#hyper-v-data-exchange-service-kvp).

## Hyper-V KVP Data Exchange Quirks

Due to interactions between Windows' case-sensitivity and Linux's case-insensitivity, it is possible to create logical duplicates in KVPs that only the Linux side can discern. *When the key names of two or more separate KVPs differ only by case, Windows will only access the **last** one.* Linux can view and manipulate all of them. Take care when using this utility to create KVP data to send to the host.

For detailed information on KVP in Hyper-V, please see the links at the end of this document.

## Usage

hvkvp operates in three modes: read, write, and delete.
Use the built-in help for instructions and examples:

* ```hvkvp --help```
* ```hvkvp read --help```
* ```hvkvp write --help```
* ```hvkvp delete --help```

## Read Examples

Use the **read** parameter to read KVP data.

### Example: Read all built-in parameters from the host

```bash
hvkvp read -s params
```

Shows automatically-generated information from the host.

### Example: Read the name of the virtual machine

```bash
hvkvp read -s params -k VirtualMachineName
```

Reads the value of the "VirtualMachineName" key from the auto-generated parameters.

### Example: Read the parameters pertaining to the physical system

```bash
hvkvp read -s params -k physicalhost -i -m off
```

Retrieves the value of "PhysicalHostName" and "PhysicalHostNameFullyQualified" from the auto-generated parameters by performing a case-insensitive partial match on the key names.

### Example: Read all host-side user-generated values

```bash
hvkvp read -s in
```

Retrieves all custom KVPs sent by the host. This will be empty on most systems. Use the -i and -m switches as shown in prior examples to customize results.

## Write Examples

Use the **write** parameter to send KVP data to the host.

### Example: Send a new KVP value to the host

```bash
sudo hvkvp write -k MyNewKey -v 'My New Value!'
```

Writes a new key/value pair that can be received by the host.

### Example: Demonstrate a quasi-duplicate value

```bash
sudo hvkvp write -k mynewkey -v 'Another New Value!'
```

This entry uses a key that differs from the previous entry only by case. Both KVPs will appear, but the host will only be able to retrieve this one (because it appears last).

### Example: Overwrite a value

```bash
sudo hvkvp write -k mynewkey -v 'This one is second'
```

Overwrites the KVP with exactly the same name.

### Example: Clear a value but leave the key

```bash
sudo hvkvp write -k mynewkey
```

Sets the named key to an empty string.

### Example: Set or overwrite a value, case-insensitive matching

```bash
sudo hvkvp write -k mynewkey -v 'Overwrite both' -i
```

Sets the supplied value on all keys that match letter-for-letter while ignoring case.

### Example: Overwrite the values of keys using a partial match

```bash
sudo hvkvp write -k key -v 'Overwrite all' -i -m off
```

Sets the supplied value on all keys whose names include the supplied key name, ignoring case. ***Partial match is a potentially dangerous operation.***

## Delete Examples

These examples illustrate how to use the **delete** parameter to delete guest-to-host keys. You cannot use this command on any other branch, as host-to-guest keys can only be deleted from the host side.

### Example: Delete a KVP

```bash
sudo hvkvp delete -k MyTest
```

Deletes the guest-to-host KVP named "MyTest".

### Example: Clean Exact Duplicates

```bash
sudo hvkvp delete -d
```

Deletes all KVPs with precisely matching duplicate keys except the last one.

### Example: Clean Case-Insensitive Duplicates

```bash
sudo hvkvp delete -di
```

Deletes all KVPs with names matching letter-for-letter, case insensitive. Can also use ```-m off``` for partial matches.

## Unpack the Tarball

If you download the tarball release rather than cloning the source files, use the **tar** command to unpack it.

```bash
tar xzf hvkvp.tar.gz
```

## Install Instructions

Use the standard **make** command to build the program from source. Note that your compiler must conform to the C++14 standard.

```bash
make
sudo make install
```

## Uninstall Instructions

The installer copies the file "hvkvp" to the system /bin folder. You can use the "uninstaller" in the makefile or delete the file manually.

```bash
sudo make uninstall
```

or

```bash
sudo rm -f /bin/hvkvp
```

## Other Make Instructions

Delete compiled and linked files: ```make clean```

Create compressed tarball: ```make tar```

## Installing on Red Hat distributions

Current versions of Red Hat Enterprise Linux and CentOS ship with GCC 4.8.5 as the official compiler. Unfortunately, that version contains a Standard Library with only partial implementations of certain features that do not clearly indicate their limitations and fail silently when called.

To install on these distributions, you will need to side load a newer compiler. RHEL customers will need to check with Red Hat for current instructions; you may require a particular license.

Follow these instructions for CentOS:

### Prerequisites

Install the prerequisites:

```bash
sudo yum install centos-release-scl devtoolset-7-gcc*
```

If you use Visual Studio or some other remote tool that operates gcc via the gdb server, run this as well:

```bash
sudo yum install devtoolset-7-gdb*
```

### Enable the new compiler

You can enable the compiler for only your current running session or permanently enable it.

To temporarily enable it:

```bash
scl enable devtoolset-7 bash
```

To permanently enable the compiler, decide whether you want to enable it for the entire system or only for your personal account.

* Personal account: edit the *~/.bash_profile* file
* All users: add a new .sh file (such as *gcc.sh* ) under */etc/profile.d/gcc.sh*

Inside that file, enter the following:

```bash
source scl_source enable devtoolset-7
```

The change will take effect at your next login, or you can run the script immediately.

## For More Information

General information about Hyper-V KVP Exchange: http://www.altaro.com/hyper-v/hyper-v-key-value-pair-data-exchange-part-1-explanation/

Article on Hyper-V KVP Exchange in Linux: http://www.altaro.com/hyper-v/key-value-pair-data-exchange-3-linux/
