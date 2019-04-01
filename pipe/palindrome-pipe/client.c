#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "userfunc.h"

#define MAX_WORD_LEN 1000

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  // Клиент принимает от пользователя некое слово после чего проверяет
  // его на корректность (отсутствие знаков препинания, цифр, смешения латинских
  // и кириллических символов):

  wchar_t wideCharWord[MAX_WORD_LEN];
  inputWord(wideCharWord, sizeof(wideCharWord) / sizeof(wchar_t));

  // И отсылает его серверу:

  char* myfifo = "./myfifo";  // путь к FIFO-файлу.

  // Открыть FIFO только для записи:
  int fd = open(myfifo, O_WRONLY);

  // Конвертация слова из строки широких символов в мультибайтовую строку символов
  // для последующей её передачи дочернему процессу:
  char multibyteWord[MAX_WORD_LEN];
  wcstombs(
     multibyteWord  // мультибайтовая строка символов.
   , wideCharWord  // строка из широких символов.
   , sizeof(multibyteWord)  // макс. количество байт для записи в мультибайтовую строку символов.
  );

  // Запись:
  write(fd, multibyteWord, strlen(multibyteWord) + 1);

  // Закрыть:
  close(fd);

  return EXIT_SUCCESS;
}
