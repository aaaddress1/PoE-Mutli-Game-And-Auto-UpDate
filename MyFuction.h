﻿#pragma hdrstop
#include <windows.h>
#include <vcl.h>
#include <mmsystem.h>
#include <tlhelp32.h>
#define JMP(from, to) (int)(((int)to - (int)from) - 5);

HMODULE GetRemoteModuleHandle(unsigned long pId, char *module)
{
MODULEENTRY32 modEntry;
HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pId);

modEntry.dwSize = sizeof(MODULEENTRY32);
Module32First(tlh, &modEntry);

do
{
AnsiString n = modEntry.szModule;

if(!stricmp(n.c_str() , module))
return modEntry.hModule;
modEntry.dwSize = sizeof(MODULEENTRY32);
}
while(Module32Next(tlh, &modEntry));

return NULL;
}


bool InjectDll(String Path,void* hd){

if(NULL==hd){return 0;}
char *lpszDll=AnsiString(Path).c_str() ;
DWORD dwSize, dwWritten;
dwSize = lstrlenA( lpszDll ) + 1;
LPVOID lpBuf = VirtualAllocEx( hd, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE );
if ( NULL == lpBuf ){CloseHandle( hd );return 0;}
if ( WriteProcessMemory( hd, lpBuf, (LPVOID)lpszDll, dwSize, &dwWritten ) )
{
if ( dwWritten != dwSize ){
VirtualFreeEx( hd, lpBuf, dwSize, MEM_DECOMMIT );
CloseHandle( hd );return 0;
}
}
else{CloseHandle( hd );return 0;}
PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");
HANDLE hThread=CreateRemoteThread( hd, NULL, 0, pfnStartAddr, lpBuf, 0, NULL);
if(NULL==hThread){CloseHandle( hd );return 0;}
WaitForSingleObject( hThread, INFINITE );
VirtualFreeEx( hd, lpBuf, dwSize, MEM_DECOMMIT );
CloseHandle( hThread );
return true ;
}



String GetMyPath(HINSTANCE DllHinstDLL) {
String PathStr;	try {wchar_t chModuleDir[260*2] ;GetModuleFileNameW(DllHinstDLL, chModuleDir, 260*2);
PathStr = (chModuleDir);return PathStr;}
catch(...) {}return ExtractFilePath(Application->ExeName);
}

void AsmJump(const DWORD lpAddress, LPCVOID Function, unsigned Nops){
	DWORD OldProtection;
	VirtualProtect((LPVOID)lpAddress,10,PAGE_EXECUTE_READWRITE, &OldProtection);
	*(LPBYTE)lpAddress = 0xE9;
	*(LPDWORD)(lpAddress + 1) = (DWORD)Function - (DWORD)lpAddress - 5;
	if ((bool)Nops)
		memset(((LPBYTE)lpAddress + 5), 0x90, Nops);
	VirtualProtect((LPVOID)lpAddress,10,OldProtection, &OldProtection);
}

void AsmCall(const DWORD lpAddress, LPCVOID Function, unsigned Nops){
	DWORD OldProtection;
	VirtualProtect((LPVOID)lpAddress,10,PAGE_EXECUTE_READWRITE, &OldProtection);
	*(LPBYTE)lpAddress = 0xE8;
	*(LPDWORD)(lpAddress + 1) = (DWORD)Function - (DWORD)lpAddress - 5;
	if ((bool)Nops)
		memset(((LPBYTE)lpAddress + 5), 0x90, Nops);
	VirtualProtect((LPVOID)lpAddress,10,OldProtection, &OldProtection);
}
void MEMwrite(PVOID address, void* val, int bytes){
	DWORD d, ds;
	VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &d);
	memcpy(address, val, bytes);
	VirtualProtect(address,bytes,d,&ds);
 }
void WriteValue(long adr,int value){
DWORD OldProtection;
VirtualProtect((LPVOID)adr,0x4,PAGE_EXECUTE_READWRITE, &OldProtection);
*(int*)(adr)=value;
VirtualProtect((LPVOID)adr,0x4,OldProtection, &OldProtection);
}
