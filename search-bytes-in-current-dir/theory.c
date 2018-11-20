// Теоретические сведения:

// Открыть каталог:
DIR *opendir (const char *dirname);

// Закрыть каталог:
int closedir( DIR *dirptr);

// Чтение записей каталога:
struct dirent *readdir(DIR *dirptr);

struct dirent {
 // Индексный дескриптор файла:
 long d_ino;

 // Смещение данного элемента в реальном каталоге:
 off_t d_off;
 
 // Длина структуры:
 unsigned short d_reclen;

 // Начало массива символов, задающего имя элемента каталога:
 char d_name [1];
 // Имя ограничено нулевым байтом и содержит не более MAXNAMLEN символов.
};

// Возврат указателя в начало каталога на первую запись:
void rewindir(DIR *dirptr);

// Получение текущего каталога:
char *getcwd(char *name, size_t size);

// Создать дочерний процесс:
pid_t fork (void);

// Остановить родительский процесс до тех пор, 
// пока дочерний процесс не прекратит выполнение:
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);  // pid - заданный процесс. 

// Перегрузка исполняемой программы:
int execl(char *pathname, char *arg0, arg1, ..., argn, NULL);
int execle(char *pathname, char *arg0, arg1, ..., argn, NULL, char **envp);
int execlp(char *pathname, char *arg0, arg1, ..., argn, NULL);
int execlpe(char *pathname, char *arg0, arg1, ..., argn, NULL, char **envp);
int execv(char *pathname, char *argv[]);
int execve(char *pathname, char *argv[],char **envp);
int execvp(char *pathname, char *argv[]);
int execvpe(char *pathname, char *argv[],char **envp);

// Создать новую нить:
int pthread_create(
 pthread_t *thread, 
 const pthread_attr_t *attr, 
 void *(*start_routine)( void*), void *arg);

// Завершить функцию потока:
pthread_exit();

// Перевод нити в состояние ожидания:
int pthread_join (pthread_t thread, void **status_addr);

// Текущее время в секундах с 01.01.1970.
time_t time (time_t *tt);

struct tm * localtime(time_t *tt);

int gettimeofday(struct timeval *tv, struct timezone *tz);

struct timeval {
 long tv_sec;  // секунды.
 long tv_usec;  // микросекунды.
};

struct tm {
 int tm_sec;  // секунды.
 int tm_min;  // минуты.
 int tm_hour;  // часы.
 int tm_mday;  // день месяца.
 int tm_mon;  // месяц.
 int tm_year;  // год.
 int tm_wday;  // день недели.
 int tm_yday;  // день в году.
 int tm_isdst;  // летнее время.
};

