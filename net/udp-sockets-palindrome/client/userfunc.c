// Код пользовательского интерфейса:

#include <stdio.h>
#include <string.h>
#include "userfunc.h"

// Процедура для ввода слова:
void inputWord(wchar_t* const word, int len)
{
	do
	{
    // Программа принимает от пользователя некое слово:
		wprintf(L"Введите слово>");
    fgetws(word
        , len  // мак. количество символов для чтения, включая нуль-символ.
        , stdin
    );

		word[wcscspn(word, L"\n")] = '\0';
		wprintf(L"Вы ввели слово: '%ls'\n", word);

  // После чего проверяют его на корректность
  // (отсутствие знаков препинания, цифр,
  // смешения латинских и кириллических символов):
  } while (!checkWord(word));


}

// Проверка слова на корректность:
bool checkWord(const wchar_t* word)
{
	bool hasEnglish = false, hasRussian = false;

	int wordLen = wcslen(word);
	if (wordLen == 0) {
		wprintf(L"Ошибка! Пустая строка!\n");
		return false;
	}

	for (int i = 0; i < wordLen; ++i)
	{
		switch (word[i])
		{
		case L',':
    case L'.':
    case L'!':
    case L'?':
    case L';':
    case L':':
    case L'-':
    case L'"':
    case L'(':
    case L')':
			wprintf(L"Ошибка! В слове присутствуют знаки препинания!\n");
			return false;
		default:
			break;
		}

		if (word[i] >= L'0' && word[i] <= L'9')
		{
			wprintf(L"Ошибка! В слове присутствуют цифры!\n");
			return false;
		}
		else if ((word[i] >= L'A' && word[i] <= L'Z')
          || (word[i] >= L'a' && word[i] <= L'z'))
		{
			hasEnglish = true;
		}
		else if ((word[i] >= L'а' && word[i] <= L'я')
          || (word[i] >= L'А' && word[i] <= L'Я'))
		{
			hasRussian = true;
		}
		else
		{
			wprintf(L"Ошибка! Некорректный ввод!\n");
			return false;
		}

		if (hasEnglish && hasRussian)
		{
			wprintf(L"Ошибка! Смешение латинских и кириллических символов!\n");
			return false;
		}
	}
	return true;
}
