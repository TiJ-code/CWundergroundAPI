#ifndef TEST_H
#define TEST_H

#pragma once

#include <stdio.h>

#define ASSERT(cond, msg)                                                      \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "TEST FAILED: %s\n", msg);                               \
      return 1;                                                                \
    }                                                                          \
  } while (0)

#endif /* TEST_H */
