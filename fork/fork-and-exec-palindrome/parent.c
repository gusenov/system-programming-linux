// Программа, реализующая запуск дочерних процессов:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/wait.h>
#include "userfunc.h"

#define MAX_WORD_LEN 1000

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");


  wprintf(L"Родительский процесс принимает от пользователя некое слово…\n");
  wchar_t wideCharWord[MAX_WORD_LEN];
  inputWord(wideCharWord, sizeof(wideCharWord) / sizeof(wchar_t));

  // Конвертация слова из строки широких символов в мультибайтовую строку символов
  // для последующей её передачи дочернему процессу:
  char multibyteWord[MAX_WORD_LEN];
  wcstombs(
     multibyteWord  // мультибайтовая строка символов.
   , wideCharWord  // строка из широких символов.
   , sizeof(multibyteWord)  // макс. количество байт для записи в мультибайтовую строку символов.
  );


  // Затем дочерний процесс проверяет, является ли введенное слово палиндромом:

  int status;
  pid_t child_id = fork();

  // Идентификатор процесса-потомка отличен от идентификатора процесса-родителя:
  if (child_id > 0) {
      // Код, который выполняется в процессе-родителе:
      wprintf(L"Создан процесс-потомок с идентификатором %d.\n", child_id);

      // Ожидание завершения процесса-потомка:
      waitpid(child_id, &status, 0);

  } else if (child_id == 0) {
      // Код, который выполняется в процессе-потомке:
      execl("./child.out", "child.out", multibyteWord, (char*)NULL);
      perror("exec failed!");

  } else if (child_id == -1) {
      perror("Не удалось выполнить fork() - системный вызов, \
          создающий новый процесс (потомок)!\n");
      exit(1);
  }


  wprintf(L"Родительский процесс информирует пользователя о результате проверки…\n");
  switch (WEXITSTATUS(status)) {
    case EXIT_SUCCESS:
      wprintf(L"Строка '%ls' является палиндромом.\n", wideCharWord);
      break;
    case EXIT_FAILURE:
      wprintf(L"Строка '%ls' НЕ является палиндромом.\n", wideCharWord);
      break;
  }


  return EXIT_SUCCESS;
}
