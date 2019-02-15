// Программа, являющаяся дочерним процессом:

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>
#include "userfunc.h"

#define MAX_WORD_LEN 1000

bool isPalindrome(wchar_t str[]) {
  int l = 0, h = wcslen(str) - 1;
  while (h > l) if (str[l++] != str[h--]) return false;
  return true;
}

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");

  wprintf(L"Дочерний процесс: Принимаю от пользователя некое слово…\n");

  wchar_t wideCharWord[MAX_WORD_LEN];
  inputWord(wideCharWord, sizeof(wideCharWord) / sizeof(wchar_t));

  wprintf(L"Дочерний процесс: Проверяю, является ли введенное слово палиндромом…\n");
  bool result = isPalindrome(wideCharWord);

  wprintf(L"Дочерний процесс: Информирую пользователя о результате проверки…\n");
  if (result) {
    wprintf(L"Дочерний процесс: Строка '%ls' является палиндромом.\n", wideCharWord);
  } else {
    wprintf(L"Дочерний процесс: Строка '%ls' НЕ является палиндромом.\n", wideCharWord);
  }

  return EXIT_SUCCESS;
}
