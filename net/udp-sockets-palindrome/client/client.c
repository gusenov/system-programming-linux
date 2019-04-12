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
  wcstombs(
     multibyteWord  // мультибайтовая строка символов.
   , wideCharWord  // строка из широких символов.
   , sizeof(multibyteWord)  // макс. количество байт для записи в мультибайтовую строку символов.
  );

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
  if (sendto(
    s,  // дескриптор сокета.
    multibyteWord,  // буфер содержащей данные для отправки.
    MAX_WORD_LEN,  // размер буфера.
    0,  // флаги модифицирующие поведение сокета.
    (struct sockaddr*)&si_other,  // пункт назначения.
    slen  // размер структуры содержащей пункт назначения.
  ) == -1) {
    fwprintf(stderr, L"Не удалось отправить данные на сервер по причине:\n");
    perror(NULL);
    exit(EXIT_FAILURE);
  } else {
    wprintf(L"Данные отправлены.\n");
  }

  bool result = false;

  wprintf(L"Ожидаем результата проверки…\n");

  // Приём результата от сервера:
  if (recvfrom(
    s,  // дескриптор сокета.
    &result,  // буфер для приёма данных.
    sizeof(result),  // размер буфера для приёма данных.
    0,  // флаги модифицирующие поведение сокета.
    (struct sockaddr*)&si_other,  // здесь будет структура с адресом источника.
    &slen  // сюда вернётся размер структуры с адресом источника.
  ) == -1) {
    fwprintf(stderr, L"Не удалось принять результаты от сервера по причине:\n");
    perror(NULL);
    exit(EXIT_FAILURE);
  } else {
    wprintf(L"Данные получены.\n");
  }

  // И передача на стандартный вывод результата проверки:
  if (result) {
    wprintf(L"Строка '%ls' является палиндромом.\n", wideCharWord);
  } else {
    wprintf(L"Строка '%ls' НЕ является палиндромом.\n", wideCharWord);
  }

  close(s);

  return EXIT_SUCCESS;
}
