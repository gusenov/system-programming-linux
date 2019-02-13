// Программа, являющаяся дочерним процессом:

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>

#define MAX_WORD_LEN 1000

bool isPalindrome(wchar_t str[]) {
  int l = 0, h = wcslen(str) - 1;
  while (h > l) if (str[l++] != str[h--]) return false;
  return true;
}

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");

  wprintf(L"Дочерний процесс проверяет, является ли введенное слово палиндромом…\n");

  // Конвертация слова из мультибайтовой строки символов в строку широких символов:
  wchar_t wideCharWord[MAX_WORD_LEN];
  mbstowcs(
      wideCharWord  // строка из широких символов.
    , argv[1]  // мультибайтовая строка символов.
    , sizeof(wideCharWord) / sizeof(wchar_t)  // макс. количество интерпретируемых широких символов.
  );

  if (isPalindrome(wideCharWord)) {
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
