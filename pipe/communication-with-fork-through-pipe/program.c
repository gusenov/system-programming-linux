#include <stdio.h>  // ВВОД/ВЫВОД.
#include <stdlib.h>  // функция rand(), константа EXIT_SUCCESS.
#include <time.h>

// FORK, WAIT, PIPE:
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


// Количество случайных чисел:
#define NUM_CNT 5


// Главная функция:
int main(int argc, char **argv) {
    int idx;  // индекс.
    int num;  // число.
    int sum;  // сумма.
    int product;  // произведение.
    time_t t;
    
    int status;  // информация о статусе завершения дочернего процесса.
    int filedes[2];  // два файловых дескриптора.
    FILE *reader;  // поток данных на чтение.
    FILE *writer;  // поток данных на запись.
    pid_t child_id;  // идентификатор процесса-потомка.
    
    // Инициализация генератора псевдослучайных чисел:
    srand((unsigned) time(&t));
    
    pipe(filedes);  // создание анонимного канала.
    reader = fdopen(filedes[0], "r");  // открытие потока данных на чтение.
    writer = fdopen(filedes[1], "w");  // открытие потока данных на запись.
    child_id = fork();  // порождение дочернего процесса.
    
    // Идентификатор процесса-потомка отличен от идентификатора процесса-родителя:
    if (child_id > 0) {
        // Код, который выполняется в процессе-родителе:
        
        // Родитель передает NUM_CNT случайных чисел от 0 до 49:
        for (idx = 1; idx <= NUM_CNT; idx++) {
            num = rand() % 50;
            fprintf(writer, "%d\n", num);
            fflush(writer);  // отправить данные в программный канал немедленно!
            printf("Родитель передал число %d.\n", num);
        }

        // Ожидание завершения процесса-потомка:
        waitpid(child_id, &status, 0);
        
        fscanf(reader, "%d %d", &sum, &product);
        printf("Потомок вернул сумму %d.\n", sum);
        printf("Потомок вернул произведение %d.\n", product);

    } else if (child_id == 0) {
        // Код, который выполняется в процессе-потомке:

        sum = 0;
        product = 1;
        for (idx = 1; idx <= NUM_CNT; idx++) {
            fscanf(reader, "%d", &num);
            sum += num;
            product *= num;
        }
        
        // Потомок возвращает сумму и произведение:
        fprintf(writer, "%d %d\n", sum, product);

    } else if (child_id == -1) {
        perror("Не удалось выполнить fork() - системный вызов, \
            создающий новый процесс (потомок)!\n");
        exit(1);
    }
    
    return EXIT_SUCCESS;
}
