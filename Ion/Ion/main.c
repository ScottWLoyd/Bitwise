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
#include <inttypes.h>

#include "common.c"
#include "lex.c"
#include "ast.h"
#include "ast.c"
#include "print.c"
#include "parse.c"
#include "resolve.c"

void run_tests(void)
{
    common_test();
    lex_test();
    //print_test();
	parse_test();
	resolve_test();    
}

int main(int ArgCount, char** Args)
{
    run_tests();
    getchar();
    return 0;
}