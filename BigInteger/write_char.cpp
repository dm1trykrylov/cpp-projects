#define BUFFER_SIZE 65536
#define BASE_LENGTH 9
#include <cmath>

const unsigned long long BASE = pow(10, BASE_LENGTH);
const unsigned long long MAX_DECIMAL_DIGITS = 10000;

static unsigned int position = 0;
static unsigned char buffer[BUFFER_SIZE];
inline void flush();

struct DLinka {
  unsigned long long zifry[MAX_DECIMAL_DIGITS / BASE_LENGTH + 1];
  size_t razryady;
  bool negative;/*
  out_DLinka() {
    if (negative) write_symbol('-');
  }*/
};

inline void write_symbol(char x) {
  if (BUFFER_SIZE == position) flush();
  buffer[position] = x;
  position++;
}

inline void flush() {
  if (position) {
    fwrite(buffer, position, 1, stdout);
    position = 0;
  }
}
