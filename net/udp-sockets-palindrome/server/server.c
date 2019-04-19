#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../common/paraminput.h"

#define MAX_WORD_LEN 1000

bool isPalindrome(wchar_t str[]) {
  int l = 0, h = wcslen(str) - 1;
  while (h > l) if (str[l++] != str[h--]) return false;
  return true;
}

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  uint16_t port;
  char* ip = NULL;
  char ip_buffer[16];

  if (argc >= 2) {
    port = parse_port(argv[1]);
  } else {
    port = input_port();
  }
  wprintf(L"Порт сервера = %hd\n", port);

  struct sockaddr_in si_me;
  memset((char*)&si_me, 0, sizeof(si_me));  // обнулить структуру.
  si_me.sin_family = AF_INET;  // IPv4.

  si_me.sin_port = htons(port);
  // htons() конвертирует unsigned short integer из порядка байтов хоста в
  // сетевой порядок байтов.

  if (argc >= 3) {
    ip = parse_ip(argv[2], ip_buffer, &si_me.sin_addr);
    wprintf(L"IP-адрес сервера = %s\n", ip);
  } else {
    // Привязать сокет на все локальные интерфейсы:
  	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    // htonl() конвертирует unsigned integer из порядка байтов хоста в
    // сетевой порядок байтов.

    wprintf(L"IP-адрес сервера = все локальные интерфейсы\n");
  }

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

  // Связать сокет с портом:
  if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
    fwprintf(stderr, L"Не удалось связать сокет с портом %hd по причине:\n", port);
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  // Сервер принимает слово:

  char multibyteWord[MAX_WORD_LEN];

  struct sockaddr_in si_other;
  socklen_t slen = sizeof(si_other);

  wprintf(L"Ожидаем данные от клиента…\n");

  // Общее число полученных байт:
  ssize_t totalNumOfCharRecv = 0;

  do {
    // Блокирующий вызов:
    ssize_t numOfCharRecv = recvfrom(
      s,  // дескриптор сокета.
      multibyteWord + totalNumOfCharRecv,  // буфер приложения для приёма данных.
      MAX_WORD_LEN - totalNumOfCharRecv,  // размер буфера.
      0,  // флаги модифицирующие поведение сокета.
      (struct sockaddr*)&si_other,  // адрес источника.
      &slen  // размер структуры в которую возвращается адрес источника.
    );

    if (numOfCharRecv == -1) {
      fwprintf(stderr, L"Не удалось принять данные от клиента по причине:\n");
      perror(NULL);
      exit(EXIT_FAILURE);
    } else {
      wprintf(L"%jd байт получено.\n", numOfCharRecv);
    }

    // Увеличиваем общее число полученных байт:
    totalNumOfCharRecv += numOfCharRecv;

    // Будем дочитывать недополученное пока не встретим в полученном нуль-символ.
  } while (multibyteWord[totalNumOfCharRecv - 1] != '\0');

  // Конвертация слова из мультибайтовой строки символов в строку широких символов:
  wchar_t wideCharWord[MAX_WORD_LEN];
  mbstowcs(
      wideCharWord  // строка из широких символов.
    , multibyteWord  // мультибайтовая строка символов.
    , sizeof(wideCharWord) / sizeof(wchar_t)  // макс. количество интерпретируемых широких символов.
  );

  // Сервер проверяет, является ли введенное слово палиндромом:
  bool result = isPalindrome(wideCharWord);

  // И передает на стандартный вывод результат проверки:
  if (result) {
    wprintf(L"Строка '%ls' является палиндромом.\n", wideCharWord);
  } else {
    wprintf(L"Строка '%ls' НЕ является палиндромом.\n", wideCharWord);
  }

  wprintf(L"Отправка результата клиенту.\n");

  // Ответ клиенту:

  // Будем делать отправку до тех пор пока не отправим результирующий байт:
  ssize_t numOfByteSent = 0;
  do {
    numOfByteSent = sendto(
      s,  // дескриптор сокета.
      &result,  // буфер содержащий данные для отправки.
      sizeof(result),  // количество байт для отправки.
      0,  // флаги модифицирующие поведение сокета.
      (struct sockaddr*)&si_other,  // пункт назначения.
      slen  // размер структуры содержащей пункт назначения.
    );

    if (numOfByteSent == -1) {
      fwprintf(stderr, L"Не удалось отправить данные на сервер по причине:\n");
      perror(NULL);
      exit(EXIT_FAILURE);
    } else {
      wprintf(L"%jd байт отправлен.\n", numOfByteSent);
    }

  } while (numOfByteSent != 1);

  close(s);

  return EXIT_SUCCESS;
}
