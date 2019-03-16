// Приложение, обеспечивающее получение системной информации:

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/utsname.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

int main(int argc, char *argv[]) {

  // Вывод имени компьютера:
  char hostname[HOST_NAME_MAX];
  if (gethostname(hostname, HOST_NAME_MAX)) {
    perror("gethostname() error");
    return EXIT_FAILURE;
  } else {
    if (printf("Host name: %s\n", hostname) < 0) {
      perror("printf() error");
      return EXIT_FAILURE;
    }
  }

  // Вывод имени пользователя:
  char username[LOGIN_NAME_MAX];
  if (getlogin_r(username, LOGIN_NAME_MAX)) {
    perror("getlogin_r() error");
    return EXIT_FAILURE;
  } else {
    if (printf("User name: %s\n", username) < 0) {
      perror("printf() error");
      return EXIT_FAILURE;
    }
  }

  // Вывод версии ядра операционной системы:
  struct utsname info;
  if (uname(&info)) {
    perror("uname() error");
    return EXIT_FAILURE;
  } else {
    char* my_kernel_version = info.release;
    if (printf("Kernel version: %s\n", my_kernel_version) < 0) {
      perror("printf() error");
      return EXIT_FAILURE;
    }
  }

  // Системные вызовы getdomainname, mknod, mount:

  char buf[32];
  if (getdomainname(buf, sizeof buf)) {
    perror("getdomainname() error");
    return EXIT_FAILURE;
  } else {
    if (printf("Domain name: %s\n", buf) < 0) {
      perror("printf() error");
      return EXIT_FAILURE;
    }
  }

  // Вывод текущей директории, чтобы понимать, где выполняется программа:
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
      if (printf("Current working dir: %s\n", cwd) < 0) {
        perror("printf() error");
        return EXIT_FAILURE;
      }
  } else {
      perror("getcwd() error");
      return EXIT_FAILURE;
  }


  // Пример использования системных вызовы mknod и mount
  // для создания дискового раздела в файле и его монтирования:

  // Создание пустого бинарного файла размером bs * count:
  system("rm -f empty_file.bin");
  system("dd if=/dev/zero of=empty_file.bin bs=1024k count=10 status=none");

  // Создание блочного (буферизованного) специального файла:

  system("rm -f my_block_buffered_special_file");

  // Ниже следует системный вызов mknod аналогичный команде:
  //system("sudo mknod my_block_buffered_special_file b 7 100500");

  char fn[] = "my_block_buffered_special_file";
  if (mknod(fn, S_IFBLK, makedev(7, 100500))) {
    // Когда  создается  специальный блочный или символьный файл,
    // то после типа файла должны быть заданы
    // старший и младший номера устройства.

    perror("mknod() error");
    return EXIT_FAILURE;
  }

  // Подключаем его как полноценное блочное устройство:
  system("sudo losetup \"$(pwd)/my_block_buffered_special_file\" empty_file.bin");
  //system("losetup -a");

  // Создание файловой системы:
  system("sudo mkfs.ext2 -F -q /dev/loop100500");

  // Монтирование:

  system("rm -rf ./test_directory");
  system("mkdir ./test_directory");

  // Ниже следует системный вызов mount аналогичный команде:
  //system("sudo mount /dev/loop100500 ./test_directory");

  if (mount("/dev/loop100500", "./test_directory", "ext2", 0, NULL)) {
    perror("mount() error");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
