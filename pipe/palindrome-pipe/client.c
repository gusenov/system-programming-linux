#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "userfunc.h"

#define MAX_WORD_LEN 1000

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  if (argc != 2) {
    wprintf(L"Использование: <исполняемый файл> <имя FIFO-файла>\n");
    exit(EXIT_FAILURE);
  }

  // Клиент принимает от пользователя некое слово после чего проверяет
  // его на корректность (отсутствие знаков препинания, цифр, смешения латинских
  // и кириллических символов):

  wchar_t wideCharWord[MAX_WORD_LEN];
  inputWord(wideCharWord, sizeof(wideCharWord) / sizeof(wchar_t));

  // И отсылает его серверу:

  char* myfifo = argv[1];  // путь к FIFO-файлу.

  // Открыть FIFO только для записи:
  int fd = open(myfifo, O_WRONLY);
  if (fd == -1) {
    // Много функций возвращают -1 при ошибке и устанавливают числовое значение
    // переменной errno. Функция perror выводит переданный ей аргумент и далее
    // осмысленное описание ошибки в зависимости от значения errno.
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  // Конвертация слова из строки широких символов в мультибайтовую строку символов
  // для последующей её передачи дочернему процессу:
  char multibyteWord[MAX_WORD_LEN];
  wcstombs(
     multibyteWord  // мультибайтовая строка символов.
   , wideCharWord  // строка из широких символов.
   , sizeof(multibyteWord)  // макс. количество байт для записи в мультибайтовую строку символов.
  );

  // Запись:
  size_t count = strlen(multibyteWord) + 1;
  ssize_t num = write(fd, multibyteWord, count);

  if (num == -1) {
    perror(NULL);
  } else if (num == 0) {
    wprintf(L"Внимание! Ничего не записано.\n");
  } else if (num != count) {
    wprintf(L"Внимание! Не было записано ожидаемое число байт.\n");
  }

  // Закрыть:
  if (close(fd) == -1) {
    perror(NULL);
  }

  return EXIT_SUCCESS;
}
