#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "common.c"
#include "lex.c"
#include "ast.c"

void run_tests()
{
    common_test();
    lex_test();
}

int main(int ArgCount, char** Args)
{
    run_tests();
    return 0;
}