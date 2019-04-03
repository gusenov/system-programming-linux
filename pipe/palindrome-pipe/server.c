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

  if (argc != 2) {
    wprintf(L"Использование: <исполняемый файл> <имя FIFO-файла>\n");
    exit(EXIT_FAILURE);
  }

  // Сервер принимает слово:

  char* myfifo = argv[1];  // путь к FIFO-файлу.

  if (access(myfifo, F_OK ) != -1 ) {
    wprintf(L"Ошибка! Указанный файл уже существует!\n");
    exit(EXIT_FAILURE);
  }

  // Создание именованного FIFO-файла:
  if (mkfifo(
    myfifo,  // путь.
    0666  // права.
  ) == -1) {
    // Много функций возвращают -1 при ошибке и устанавливают числовое значение
    // переменной errno. Функция perror выводит переданный ей аргумент и далее
    // осмысленное описание ошибки в зависимости от значения errno.
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  // Открыть FIFO только для чтения:
  int fd = open(myfifo, O_RDONLY);

  if (fd == -1) {
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  char multibyteWord[MAX_WORD_LEN];

  // Считать из FIFO:
  ssize_t num = read(fd, multibyteWord, sizeof(multibyteWord));
  if (num == -1) {
    perror(NULL);
  }

  // Закрыть:
  if (close(fd) == -1) {
    perror(NULL);
  }

  // Удалить:
  if (remove(myfifo) == -1) {
    perror(NULL);
  }

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
