// Программа, реализующая запуск дочерних процессов:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/wait.h>
#include <wchar.h>

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");

  int status;
  pid_t child_id = fork();

  // Идентификатор процесса-потомка отличен от идентификатора процесса-родителя:
  if (child_id > 0) {
      // Код, который выполняется в процессе-родителе:
      wprintf(L"Родительский процесс: Создал процесс-потомок с идентификатором %d.\n", child_id);

      wprintf(L"Родительский процесс: Ожидаю завершения процесса-потомка…\n");

      waitpid(child_id, &status, 0);

  } else if (child_id == 0) {
      // Код, который выполняется в процессе-потомке:
      execl("./child.out", "child.out", "", (char*)NULL);
      perror("exec failed!");

  } else if (child_id == -1) {
      perror("Родительский процесс: Не удалось выполнить fork() - системный вызов, \
          создающий новый процесс (потомок)!\n");
      exit(1);
  }

  wprintf(L"Родительский процесс: Дождался завершения процесса-потомка.\n");

  return EXIT_SUCCESS;
}
