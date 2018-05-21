#include <stdio.h>

// Forward declarations
#line 1 "test1.ion"
int example_test(void);
typedef union IntOrPtr IntOrPtr;
#line 10 "test1.ion"
void f(void);
typedef struct Vector Vector;
#line 23 "test1.ion"
int fact_iter(int n);
#line 32 "test1.ion"
int fact_rec(int n);
typedef struct T T;

// Ordered declarations
#line 1 "test1.ion"
int example_test(void) {
{return (fact_rec(10)) == (fact_iter(10));
}
#line 32 "test1.ion"
int fact_rec(int n) {
{if ((n) == (0)) {
{#line 35 "test1.ion"
"return 1;
;} else {
{#line 39 "test1.ion"
"return (n) * (fact_rec((n) - (1)));
;}
}
#line 23 "test1.ion"
int fact_iter(int n) {
{int r = 1;
;for (int i = 2; (i) <= (n); i++) {
{#line 27 "test1.ion"
"r *= i;
;}
}return r;
}
#line 5 "test1.ion"
union IntOrPtr {
MZ進nt i;
MZ進nt (*p);
};
#line 10 "test1.ion"
void f(void) {
{IntOrPtr u1 = (IntOrPtr){.i = 42};
;IntOrPtr u2 = (IntOrPtr){.p = (int *)(42)};
;u1.i = 0;
;u2.p = (int *)(0);
}
#line 17 "test1.ion"
int i;
#line 19 "test1.ion"
struct Vector {
MZ進nt x;
MZ進nt y;
};
#line 45 "test1.ion"
T (*p);
#line 43 "test1.ion"
enum { n = (1) + (sizeof(p)) };
#line 47 "test1.ion"
struct T {
MZ進nt (a[n]);
};