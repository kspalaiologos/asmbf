#include "../bfasmapi.h"

int f() {
  return 48;
}

int o(int c) {
  printf("%cello\n", 24 + c);
}

int io_example(void) {
  bf_set_input_func(&f);
  bf_set_output_func(&o);
  bfasm("in_ r1");
  bfasm("out r1");
  return 0;
}

int add(int a, int b) {
  bf_set_reg(3, a);
  bf_set_reg(4, b);
  bfasm("add r3, r4");
  return bf_get_reg(3);
}

int func_example(void) {
  printf("%d\n", add(64, add(16, 48)));
  return 0;
}

int main(void) {
  return io_example() || func_example();
}
