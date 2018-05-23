// Preamble
#include <stdio.h>

typedef unsigned char uchar;
typedef signed char schar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long llong;
typedef unsigned long long ullong;

// Forward declarations
typedef union IntOrPtr IntOrPtr;
typedef struct Vector Vector;
typedef struct T T;

// Sorted declarations
#line 3 "C:\\Users\\sloyd3\\Documents\\src\\compilers\\Bitwise\\Ion\\Ion\\\\test1.ion"

#line 6

#line 9

#line 11
char c = 1;

#line 12
uchar uc = 1;

#line 13
schar sc = 1;

enum { N = (((char)(42)) + (8)) != (0) };

#line 19
uchar h(void);

#line 17
typedef int (A[(1) + ((2) * (sizeof(h())))]);

#line 41
typedef IntOrPtr U;

#line 27
int g(U u);

#line 47
union IntOrPtr {
    int i;
    int (*p);
};

#line 31
void k(void (*vp), int (*ip));

#line 36
void f1(void);

#line 43
int example_test(void);

#line 99
int fact_rec(int n);

#line 90
int fact_iter(int n);

#line 52
int (escape_to_char[256]) = {[110] = 10, [114] = 13, [116] = 9, [118] = 11, [98] = 8, [97] = 7, [48] = 0};

#line 62
int (array[11]) = {1, 2, 3, [10] = 4};

int is_even(int digit);

#line 80
int i;

struct Vector {
    int x;
    #line 83
    int y;
};

void f2(Vector v);

#line 112
T (*p);

#line 110
enum { n = (1) + (sizeof(p)) };

#line 114
struct T {
    int (a[n]);
};

void benchmark(int n);

#line 126
int va_test(int x, ...);

#line 130
typedef int (*F)(int, ...);

void test_ops(void);

#line 162
int main(int argc, char (*(*argv)));

// Function definitions
#line 19
uchar h(void) {
    #line 21
    int (*p) = &((int){0});
    ulong x = ((uint){1}) + ((long){2});
    int y = +(c);
    return x;
}

int g(U u) {
    return u.i;
}

void k(void (*vp), int (*ip)) {
    vp = ip;
    ip = vp;
}

void f1(void) {
    int (*p) = &((int){0});
    *(p) = 42;
}

#line 43
int example_test(void) {
    return (fact_rec(10)) == (fact_iter(10));
}

#line 64
int is_even(int digit) {
    int b = 0;
    switch (digit) {
    case 0:
    case 2:
    case 4:
    case 5:
    case 8: {
        #line 68
        b = 1;
        break;
    }
    }
    #line 70
    return b;
}

#line 86
void f2(Vector v) {
    v = (Vector){0};
}

int fact_iter(int n) {
    int r = 1;
    for (int i = 0; (i) <= (n); i++) {
        #line 94
        r *= i;
    }
    return r;
}

int fact_rec(int n) {
    if ((n) == (0)) {
        #line 102
        return 1;
    } else {
        #line 106
        return (n) * (fact_rec((n) - (1)));
    }
}

#line 118
void benchmark(int n) {
    int r = 1;
    for (int i = 1; (i) <= (n); i++) {
        #line 122
        r *= i;
    }
}

int va_test(int x, ...) {
    return 0;
}

#line 132
void test_ops(void) {
    float pi = 3.140000;
    float f = 0.000000;
    f = +(pi);
    f = -(pi);
    int n = -(1);
    n = ~(n);
    f = (f) * (pi);
    f = (pi) / (pi);
    n = (3) % (2);
    n = (n) + ((uchar)(1));
    int (*p) = &(n);
    p = (p) + (1);
    n = ((p) + (1)) - (p);
    n = (n) << (1);
    n = (n) >> (1);
    int b = ((p) + (1)) > (p);
    b = ((p) + (1)) >= (p);
    b = ((p) + (1)) < (p);
    b = ((p) + (1)) <= (p);
    b = ((p) + (1)) == (p);
    b = (1) > (2);
    b = (1.230000) <= (pi);
    n = 255;
    b = (n) & (~(1));
    b = (n) & (1);
    b = ((n) & (~(1))) ^ (1);
    b = (p) && (pi);
}

int main(int argc, char (*(*argv))) {
    test_ops();
    int b = example_test();
    puts("Hello world!");
    int c = getchar();
    printf("You wrote \'%c\'\n", c);
    va_test(1);
    va_test(1, 2);
    return 0;
}
