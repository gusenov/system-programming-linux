#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_WORD_LEN 1000

bool isPalindrome(wchar_t str[]) {
  int l = 0, h = wcslen(str) - 1;
  while (h > l) if (str[l++] != str[h--]) return false;
  return true;
}

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  // Сервер принимает слово:

  char* myfifo = "./myfifo";  // путь к FIFO-файлу.

  // Создание именованного FIFO-файла:
  mkfifo(
    myfifo,  // путь.
    0666  // права.
  );

  // Открыть FIFO только для чтения:
  int fd = open(myfifo, O_RDONLY);

  char multibyteWord[MAX_WORD_LEN];

  // Считать из FIFO:
  read(fd, multibyteWord, sizeof(multibyteWord));

  // Закрыть:
  close(fd);

  // Конвертация слова из мультибайтовой строки символов в строку широких символов:
  wchar_t wideCharWord[MAX_WORD_LEN];
  mbstowcs(
      wideCharWord  // строка из широких символов.
    , multibyteWord  // мультибайтовая строка символов.
    , sizeof(wideCharWord) / sizeof(wchar_t)  // макс. количество интерпретируемых широких символов.
  );

  // Сервер проверяет, является ли введенное слово
  // палиндромом:
  bool result = isPalindrome(wideCharWord);

  // И передает на стандартный вывод результат проверки:
  if (result) {
    wprintf(L"Строка '%ls' является палиндромом.\n", wideCharWord);
  } else {
    wprintf(L"Строка '%ls' НЕ является палиндромом.\n", wideCharWord);
  }

  return EXIT_SUCCESS;
}
