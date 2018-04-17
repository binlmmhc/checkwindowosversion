#include <windows.h>
#include <iostream>

//first time, i tried to use this library to verify os version
//#include <VersionHelpers.h>

using std::cin;
using std::cout;
using std::wcout;
using std::endl;

HKEY hkey;

BOOL QueryVersionRecord(wchar_t* ValueName);
BOOL CheckVersionThroughReg(VOID);

int main(int argc, char* argv) {

	CheckVersionThroughReg();

}


BOOL QueryVersionRecord(wchar_t* ValueName) {
	LONG RegOpenRet;
	DWORD ValueType, cbData = 0;
	wchar_t* Value = NULL;

	RegOpenRet = RegQueryValueExW(hkey,
		ValueName, NULL, &ValueType, NULL, &cbData);
	if (RegOpenRet == ERROR_SUCCESS && (cbData > 0)) {
		Value = new wchar_t[cbData + 2];
		if (Value == NULL) {
			RegCloseKey(hkey);
			wcout << L"[-]  Allocate Space Failed." << endl;
			return FALSE;
		}
		wmemset(Value, 0, cbData + 2);
	}

	RegOpenRet = RegQueryValueExW(hkey,
		ValueName, NULL, &ValueType, (LPBYTE)Value, &cbData);
	if (RegOpenRet != ERROR_SUCCESS) {
		wcout << L"[-]  Query " << ValueName <<" Failed." << endl;
		return FALSE;
	}
	else {
		wcout << L"[+]  " << ValueName << L" : " << Value << endl;
	}

	if (Value)
		delete[] Value;

	return TRUE;
}

//we can optimize this function, it contains too much same code
BOOL CheckVersionThroughReg(VOID) {
	LONG RegOpenRet;
	wchar_t VersionRecord[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

	//open the registry database
	RegOpenRet = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		VersionRecord, 0, KEY_READ, &hkey);
	if (RegOpenRet != ERROR_SUCCESS) {
		wcout << L"[-]	Open Key Failed. Error Code: " << GetLastError() << endl;
		return FALSE;
	}

	//Query Produce Name and we will extract the number in the name
	QueryVersionRecord(L"ProductName");

	//Query Build Lab, we can get some build version and os version
	QueryVersionRecord(L"BuildLab");

	//Then, we will get NT Version
	QueryVersionRecord(L"CurrentVersion");

	//we close hkey here
	if (hkey)
		RegCloseKey(hkey);

	//we can make a SYSWow64 query to judge the system bits
	wchar_t wow64[] = L"SOFTWARE\\Microsoft\\Wow64";
	RegOpenRet = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		wow64, 0, KEY_READ | KEY_WOW64_64KEY, &hkey);
	if (RegOpenRet == ERROR_SUCCESS) {
		wcout << L"[+]  System bits: 64" << endl;
	}
	else {
		wcout << L"[+]  System bits: 32" << endl;
	}

	if (hkey)
		RegCloseKey(hkey);
	

	return TRUE;

}