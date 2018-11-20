// Программа поиска заданной пользователем комбинации из m байт во всех файлах текущего каталога.

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <wchar.h>

// Максимальная длина заданной пользователем комбинации из m байт:
#define M_LIMIT 255

// Длина буфера для записи одной строчки в файл результата.
#define LINE_BUFFER_SZ 100

int n = 0;  // счётчик запущенных дочерних процессов.

// Реакция на специальный сигнал SIGCHLD, 
// который каждый дочерний процесс посылает своему процесус-родителю при завершении работы: 
void handle_sigchld(int sig) {
  --n;
  //printf("--n=%d; pid=%d\n", n, getpid());  // DEBUG.
}

int main(int argc, char *argv[]) {

 // Пользователь задаёт в качестве аргументов командной строки:
 
 // имя каталога (-d, --dir),
 char *inputDir = NULL;
 int inputDirLen = 0;
 
 // строку поиска (-s, --string),
 const char *searchStr = NULL;
 int searchStrLen = 0;
 
 // файл результата (-f, --file).
 const char *outputFile = NULL;
 int outputFileLen = 0;

 
 const char* short_options = "d:s:f:";
 
 const struct option long_options[] = {
  { "dir", required_argument, NULL, 'd' },
  { "string", required_argument, NULL, 's' },
  { "file", required_argument, NULL, 'f' },
  { NULL, 0, NULL, 0 }
 };
 
 int rez;
 int option_index = -1;

 while ((rez = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
  switch(rez) {
   
   case 'd': {
    inputDirLen = strlen(optarg);
    
    if (inputDirLen <= 0) {
     printf("Ошибка! Длина имени каталога должна быть больше нуля!\n");
     return EXIT_FAILURE;
    }
    
    inputDir = strdup(optarg);
    
    // Удаление конечной косой черты:
    if ((inputDirLen > 0) && (inputDir[inputDirLen - 1] == '/'))
	inputDir[inputDirLen - 1] = '\0';
	
    printf("Имя каталога: '%s'\n", optarg);
    break;
   };
   
   case 's': {
    searchStrLen = strlen(optarg);
    
    if (searchStrLen <= 0) {
     printf("Ошибка! Длина строки поиска должна быть больше нуля!\n");
     return EXIT_FAILURE;
    }
    
    searchStr = strdup(optarg);
 
    printf("Строка поиска: '%s'\n", searchStr);
    printf("Длина строки поиска: %lu байт\n", searchStrLen * sizeof(char));
    
    if (searchStrLen * sizeof(char) >= M_LIMIT) {
     printf("Ошибка! Размер строки должен быть меньше 255 байт!\n");
     printf("Размер char составляет %lu байт.\n", sizeof(char));
     return EXIT_FAILURE;
    }
    break;
   };
   
   case 'f': {
    outputFileLen = strlen(optarg);
    
    if (outputFileLen <= 0) {
     printf("Ошибка! Длина имени файла результата должна быть больше нуля!\n");
     return EXIT_FAILURE;
    }
    
    outputFile = strdup(optarg);
    printf("Файл результата: '%s'\n", optarg);
    break;
   };
   
   default: {
    printf("Ошибка! Неизвестный аргумент '%s'!\n", optarg);
    break;
   };
   
  }  // switch
  option_index = -1;
 }  // while

 char cwd[PATH_MAX];
 if (getcwd(cwd, sizeof(cwd)) != NULL) {
  printf("Текущий каталог: %s\n", cwd);
 } else {
  perror("getcwd() error");
  return EXIT_FAILURE;
 }

 if (inputDir == NULL) {
  inputDir = cwd;
  printf("Внимание! Не указан каталог для поиска! В качестве входного будет использован текущий каталог: '%s'\n", inputDir);
 }
 
 if (searchStr == NULL) {
  printf("Ошибка! Не указана строка поиска!\n");
  return EXIT_FAILURE;
 }
 
 if (outputFile == NULL) {
  printf("Внимание! Не указан файл результата! Вывод будет осуществляться только в консоль.\n");
  return EXIT_FAILURE;
 }

 // Главный процесс открывает каталог:
 DIR *dp = opendir(inputDir);
 if (dp == NULL) {
  printf("Ошибка!\n%s\n", strerror(errno));
  return EXIT_FAILURE;
 }
 
 // Максимальное число запущенных процессов поиска заданной комбинации из m байт.
 int N;
 do {
  printf("Введите N=");
  scanf("%d", &N);
  if (N <= 0) {
   printf("Число запущенных процессов должно быть больше нуля!\n");
  }
 } while(N <= 0);
 
 // Реакция на специальный сигнал SIGCHLD, 
 // который каждый дочерний процесс посылает своему процесус-родителю при завершении работы:
 struct sigaction sa;
 sa.sa_handler = &handle_sigchld;
 if (sigemptyset(&sa.sa_mask) == -1) {
  perror(0);
  exit(1);
 }
 sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
 if (sigaction(SIGCHLD, &sa, 0) == -1) {
  perror(0);
  exit(1);
 }

 
 // Запуск для каждого файла каталога отдельного процесса поиска заданной комбинации из m байт:
 
 struct dirent *d;
 struct stat s;
 pid_t pid;

 printf("-------------------------------------------------------------------\n"); 
 printf("ИД        Полный путь                     Число          Результаты\n");
 printf("процесса  и                               просмотренных  поиска    \n");
 printf("          имя файла                       байт                     \n");
 printf("-------------------------------------------------------------------\n");
 
 while ((d = readdir(dp)) != NULL) {
  char path[strlen(inputDir) + strlen(d->d_name) + 2];
  sprintf(path, "%s/%s", inputDir, d->d_name);
  
  if (0 == stat(path, &s) && !S_ISDIR(s.st_mode)) {
  
   pid = fork();
   
   // В родительском процессе:
   if (pid > 0) {

    ++n;  // увеличиваем число запущенных процессов на единицу.
    //printf("++n=%d; pid=%d\n", n, getpid());  // DEBUG.
   
    // Число запущенных процессов в любой момент времени не превышает N:
    while (n >= N) {     
     usleep(1);
    }

   }

   // В дочернем процессе:
   if (pid == 0) {
    int fd;
    
    fd = open(path, O_RDONLY);
    if (fd < 0) {
     printf("Ошибка!\n%s\n", strerror(errno));
     return EXIT_FAILURE;
    }

    char byteReaded;
    int i = 0, numberOfBytesReaded = 0;
    bool isFound = 0;
    
    while (read(fd, &byteReaded, 1) > 0) {
     ++numberOfBytesReaded;
     
     if (byteReaded != searchStr[i]) {
      i = 0;
     } else if (byteReaded == searchStr[i]) {
      ++i;
     }

     if (i == searchStrLen) {
      isFound = true;
      i = 0;
     }
     
    }  // while
    
    if (close(fd) == -1) {
     printf("Ошибка закрытия очередного файла в котором производится поиск!\n%s\n", strerror(errno));
    }
    
    printf("%-10d%-32s%-15d%-10s\n", getpid(), path, numberOfBytesReaded, isFound ? "НАЙДЕНО" : "НЕ НАЙДЕНО");
    
    // Результаты поиска (только найденные файлы) 
    // по предыдущему формату
    // записываются в выходной файл.
    if (outputFile != NULL && isFound) {
     fd = open(outputFile, O_RDWR | O_APPEND | O_CREAT);
     if (fd < 0) {
      printf("Ошибка!\n%s\n", strerror(errno));
      return EXIT_FAILURE;
     }
     char lineBuffer[LINE_BUFFER_SZ];
     sprintf(lineBuffer, "%-10d%-32s%-15d%-10s\n", getpid(), path, numberOfBytesReaded, "НАЙДЕНО");
     write(fd, lineBuffer, strlen(lineBuffer));
     if (close(fd) == -1) {
      printf("Ошибка закрытия файла результата!\n%s\n", strerror(errno));
     }
    }
    
    sleep(1);
    break;
   }
   
  }  // if
 }  // while

 // В родительском процессе:
 if (pid > 0) {
  // Ожидание завершения всех дочерних процессов:
  int status; 
  wait(&status);
  
  if (closedir(dp) == -1) {
   printf("Ошибка закрытия каталога!\n%s\n", strerror(errno));
  }
 }
 
 return EXIT_SUCCESS;
}

