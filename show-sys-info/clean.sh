#!/bin/bash

sudo umount "./test_directory"
rm -rf "./test_directory"

sudo losetup -d "$(pwd)/my_block_buffered_special_file"
rm -f "./my_block_buffered_special_file"

rm -f "./empty_file.bin"
