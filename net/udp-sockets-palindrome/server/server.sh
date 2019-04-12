#!/bin/bash

# Примеры вызова:
#  "./server.sh"
#  "./server.sh" "1234"
#  "./server.sh" "1234" "127.0.0.1"

output_file_path="./server.out"

rm "$output_file_path"

server_port="$1"
server_ip="$2"

gcc -Wall "server.c" "../common/paraminput.h" "../common/paraminput.c" -o "$output_file_path"

if [[ -n "$server_port" ]]; then
  if [[ -n "$server_ip" ]]; then
    "$output_file_path" "$server_port" "$server_ip"
  else
    "$output_file_path" "$server_port"
  fi
else
  "$output_file_path"
fi
