#pragma once
#include <signal.h>
#include <stdint.h>

#define PORT "35000"
#define debugger raise(SIGTRAP)
#define PAYLOAD_LEN 4096
#define HEADER_LEN sizeof(uint32_t)

uint32_t read_full(int fd, char *buf, int n);

uint32_t write_full(int fd, const char *buf, int n);
