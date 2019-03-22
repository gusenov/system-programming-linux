#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include <wchar.h>
#include <linux/limits.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

bool checkpath(const char* path) {
  DIR* dir = opendir(path);
  if (dir) {
      wprintf(L"Каталог существует.\n");
      closedir(dir);
      return true;
  }
  else if (ENOENT == errno) {
      wprintf(L"Каталог не существует!\n");
  } else {
      wprintf(L"Не удалось открыть каталог!\n");
  }
  return false;
}

// Процедура для ввода слова:
void inputdir(wchar_t* const wPath, int len, char* cPath) {
	do {
    // Каталог задается пользователем:
		wprintf(L"Задайте каталог>");
    fgetws(wPath
         , len  // мак. количество символов для чтения, включая нуль-символ.
         , stdin
    );

		wPath[wcscspn(wPath, L"\n")] = '\0';
		wprintf(L"Пользователь задал каталог: '%ls'\n", wPath);

    wcstombs(
       cPath  // мультибайтовая строка символов.
     , wPath  // строка из широких символов.
     , PATH_MAX  // макс. количество байт для записи в мультибайтовую строку символов.
    );

    // Устойчивость к некорректному пользовательскому вводу:
  } while (!checkpath(cPath));
}

void list(const char* path) {
  char full_path[PATH_MAX];

  DIR* dir = opendir(path);
  if (dir) {
      struct dirent *item;
      while ((item = readdir(dir)) != NULL) {

        struct stat st;
        full_path[0]='\0';
        strcat(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, item->d_name);
        stat(full_path, &st);
        long long size = (long long)st.st_size;

        wprintf(size == 0 ? L"*%-49s" : L"%-50s", item->d_name);
        wprintf(L"%-20lld байт", size);
        wprintf(L"\t\t%s", ctime(&st.st_mtime));

      }
      closedir(dir);
  }
}

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");

  char cPath[PATH_MAX];
  wchar_t wPath[PATH_MAX / sizeof(wchar_t)];  // 1 wchar_t - 4 байта.
  inputdir(wPath, sizeof(wPath) / sizeof(wchar_t), cPath);

  wprintf(L"Список файлов:\n");
  list(cPath);

  return EXIT_SUCCESS;
}
