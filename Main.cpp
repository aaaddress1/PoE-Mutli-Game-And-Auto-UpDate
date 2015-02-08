#include <vcl.h>
#pragma hdrstop
#pragma argsused
#include "Data.h"
#include "MyFuction.h"
#include "HookCreateProcess.h"

void PatchGGInInDer()
{
	long ggin = (long)GetModuleHandle("ggplugin.dll")  ;
	for (int i = 0; i < 0xFFFFFFF ; i++)
	{
		if (
			*(long*)(ggin+i+0x01)==0xFF6AEC8B &&
			*(BYTE*)(ggin+i+0x05)==0x68 &&
			*(long*)(ggin+i+0x0A)==0x0000A164 &&
			*(long*)(ggin+i+0x11)==0x00B4EC81 &&
			*(long*)(ggin+i+0x11)==0x00B4EC81 )
		{
			MEMwrite((void*)(ggin+i),(void*)"\x31\xC0\xC3",3);
			return;
		}
	}
	MessageBoxA(0,"Patch Fail. 競時通之記憶體也許有大量變化導致無法自動搜索.",0,0);
}
void PatchPoE_Muti()
{
	long ggin = (long)GetModuleHandle("PathofExile.exe")  ;   //PathofExile.exe+243231
	for (int i = 0; i < 0xFFFFFFF ; i++)
	{
		if (*(long*)(ggin+i+0x00)==0x0001023D &&
			*(long*)(ggin+i+0x03)==0x29750000 )
		{
			//MessageBoxA(0,AnsiString(IntToHex((int)(ggin+i),8)).c_str() ,0,0);
			MEMwrite((void*)(ggin+i+0x05),(void*)"\xEB",1);
			return;
		}
	}
	MessageBoxA(0,"Patch Fail. POE之記憶體也許有大量變化導致無法自動搜索.",0,0);
}

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved){
	if(reason == DLL_PROCESS_ATTACH){
		g_hinstDll = hinst;
		MyPath = String(GetMyPath(hinst));
		CurrentProcessPath = (Application->ExeName);
		LoadCreateProcessHook();

		if ((CurrentProcessPath).Pos("PathOfExile"))
		{
			PatchPoE_Muti();
		}else if ((CurrentProcessPath).Pos("GarenaMessenger")) {
			PatchGGInInDer();
		}

	}else if(reason == DLL_PROCESS_DETACH){
		ExitProcess(NULL);
	}
	return true;
}
