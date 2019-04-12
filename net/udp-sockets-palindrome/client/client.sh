#!/bin/bash

# Примеры вызова:
#  "./client.sh"
#  "./client.sh" "127.0.0.1" "1234"

output_file_path="./client.out"

rm "$output_file_path"

server_ip="$1"
server_port="$2"

gcc -Wall "client.c" "userfunc.h" "userfunc.c" "../common/paraminput.h" "../common/paraminput.c" -o "$output_file_path"
if [[ -n "$server_ip" ]]; then
  if  [[ -n "$server_port" ]]; then
    "$output_file_path" "$server_ip" "$server_port"
  else
    "$output_file_path" "$server_ip"
  fi
else
  "$output_file_path"
fi
