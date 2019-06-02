#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include "userfunc.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../common/paraminput.h"

#define MAX_WORD_LEN 1000

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  // Клиент принимает от пользователя некое слово после чего проверяет его на
  // корректность (отсутствие знаков препинания, цифр, смешения латинских и
  // кириллических символов):

  wchar_t wideCharWord[MAX_WORD_LEN];
  inputWord(wideCharWord, sizeof(wideCharWord) / sizeof(wchar_t));

  // Конвертация слова из строки широких символов в мультибайтовую строку символов
  // для последующей её передачи серверу:
  char multibyteWord[MAX_WORD_LEN];
  memset(multibyteWord, 0, sizeof(multibyteWord));
  size_t numOfBytesToSent = wcstombs(
     multibyteWord  // мультибайтовая строка символов.
   , wideCharWord  // строка из широких символов.
   , sizeof(multibyteWord)  // макс. количество байт для записи в мультибайтовую строку символов.
  );

  wprintf(L"Количество данных для отправки = %zu\n", numOfBytesToSent);

  // И отсылает его серверу:

  struct sockaddr_in si_other;
  socklen_t slen = sizeof(si_other);
  memset((char*)&si_other, 0, sizeof(si_other));  // обнулить структуру.
	si_other.sin_family = AF_INET;  // IPv4.

  char ip_buffer[16];
  char* ip = NULL;
  uint16_t port;

  // Клиентская часть предоставляет пользователю возможность ввода IP-адреса
  // и номера порта серверной части:

  if (argc >= 2) {
    ip = parse_ip(argv[1], ip_buffer, &si_other.sin_addr);
  } else {
    ip = input_ip(ip_buffer, &si_other.sin_addr);
  }
  wprintf(L"IP-адрес сервера = %s\n", ip);

  if (argc >= 3) {
    port = parse_port(argv[2]);
  } else {
    port = input_port();
  }
  wprintf(L"Порт сервера = %hd\n", port);

  si_other.sin_port = htons(port);
  // htons() конвертирует unsigned short integer из порядка байтов хоста в
  // сетевой порядок байтов.

  fflush(stdout);

  // Создать UDP-сокет:
  int s = socket(
    AF_INET,  // коммуникационный домен: AF_INET для IPv4; AF_INET6 для IPv6.
    SOCK_DGRAM,  // тип сокета: SOCK_STREAM для TCP; SOCK_DGRAM для UDP;
    IPPROTO_UDP  // протокол для использования сокетом.
  );
  if (s == -1) {
    fwprintf(stderr, L"Не удалось создать UDP-сокет по причине:\n");

    // Много функций возвращают -1 при ошибке и устанавливают числовое значение
    // переменной errno. Функция perror выводит переданный ей аргумент и далее
    // осмысленное описание ошибки в зависимости от значения errno.
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  // Отправка:

  // Общее число отправленных байт:
  ssize_t totalNumOfCharSent = 0;

  // Будем делать отправку до тех пор пока не отправим всё:
  do {

    ssize_t numOfCharSent = sendto(
      s,  // дескриптор сокета.
      multibyteWord + totalNumOfCharSent,  // буфер содержащий данные для отправки.
      numOfBytesToSent - totalNumOfCharSent + 1,  // количество байт для отправки.
      0,  // флаги модифицирующие поведение сокета.
      (struct sockaddr*)&si_other,  // пункт назначения.
      slen  // размер структуры содержащей пункт назначения.
    );

    if (numOfCharSent == -1) {
      fwprintf(stderr, L"Не удалось отправить данные на сервер по причине:\n");
      perror(NULL);
      exit(EXIT_FAILURE);
    } else {
      wprintf(L"%jd символов отправлено.\n", numOfCharSent);
    }

    // Увеличиваем общее число отправленных байт:
    totalNumOfCharSent += numOfCharSent;

  } while (totalNumOfCharSent < numOfBytesToSent);

  close(s);

  return EXIT_SUCCESS;
}
