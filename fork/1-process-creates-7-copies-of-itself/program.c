#include <stdio.h>  // ВВОД/ВЫВОД.
#include <stdlib.h>  // константа EXIT_SUCCESS.

// FORK, WAIT:
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


// Какое-то действие, неважно какое:
void doaction(int data) {
    printf("Процесс № %d выполняет какое-то действие...\n", data);
}

// Главная функция:
int main(int argc, char **argv) {
    
    for (int i = 1; i <= 7; i++) {
        // Идентификатор процесса-потомка отличен от идентификатора процесса-родителя:
        pid_t child_id = fork();
        
        if (child_id > 0) {
            // Код, который выполняется в процессе-родителе:
            printf("Создан процесс-потомок № %d.\n", i);
            
            // Ожидание завершения процесса-потомка:
            int status; 
            waitpid(child_id, &status, 0);
            
            continue;

        } else if (child_id == 0) {
            // Код, который выполняется в процессе-потомке:
            if (i <= 3) {
                doaction(i);
            }
            break;

        } else if (child_id == -1) {
            perror("Не удалось выполнить fork() - системный вызов, \
                создающий новый процесс (потомок)!\n");
            exit(1);
        }
        
    }
    
    return EXIT_SUCCESS;
}
