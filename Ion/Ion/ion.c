#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#define MAX(x,y) (x > y ? x : y)

void* xrealloc(void* ptr, size_t num_bytes)
{
	ptr = realloc(ptr, num_bytes);
	if (!ptr)
	{
		perror("xrealloc failed");
		exit(1);
	}
	return ptr;
}

void* xmalloc(size_t num_bytes)
{
	void* ptr = malloc(num_bytes);
	if (!ptr)
	{
		perror("xmalloc failed");
		exit(1);
	}
	return ptr;
}

static char err_buff[1024 * 1024];

void fatal(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("FATAL: ");
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
	exit(1);
}

// Stretchy buffers
typedef struct buf_hdr
{
	size_t len;
	size_t cap;
	char buf[0];
} buf_hdr;

#define buf__hdr(b) ((buf_hdr*)((char*)(b) - offsetof(buf_hdr, buf)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n) (buf__fits((b), (n)) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b)  ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b)  ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit(b, 1), (b)[buf_len(b)] = (x), buf__hdr(b)->len++)
#define buf_free(b) ((b) ? free(buf__hdr(b)), (b) = NULL : 0)

void* buf__grow(const void* buf, size_t new_len, size_t elem_size)
{
	size_t new_cap = MAX(1 + 2 * buf_cap(buf), new_len);
	assert(new_len <= new_cap);
	size_t new_size = offsetof(buf_hdr, buf) + new_cap * elem_size;
	buf_hdr* new_hdr;
	if (buf) {
		new_hdr = xrealloc(buf__hdr(buf), new_size);
	}
	else {
		new_hdr = xmalloc(new_size);
		new_hdr->len = 0;
	}
	new_hdr->cap = new_cap;
	return new_hdr->buf;
}

void buf_test()
{
	int* asdf = NULL;
	int N = 1027;
	for (int i = 0; i<N; i++)
		buf_push(asdf, i);
	assert(buf_len(asdf) == N);
	buf_free(asdf);
}

typedef struct intern_str {
	size_t len;
	const char* str;
} intern_str;

static intern_str* interns;

const char* str_intern_range(const char* start, const char* end)
{
	size_t len = end - start;
	for (size_t i = 0; i < buf_len(interns); i++)
	{
		if (interns[i].len == len && strncmp(interns[i].str, start, len) == 0)
		{
			return interns[i].str;
		}
	}
	char* str = malloc(len + 1);
	memcpy(str, start, len);
	str[len] = 0;
	buf_push(interns, ((intern_str){ len, str }));
	return str;
}

const char* str_intern(const char* str)
{
	return str_intern_range(str, str + strlen(str));
}

void str_intern_test()
{
	char x[] = "hello";
	char y[] = "hello";
	assert(x != y);
	const char* px = str_intern(x);
	const char* py = str_intern(y);
	assert(px == py);
	char z[] = "hello!";
	const char* pz = str_intern(z);
	assert(pz != px);
}


typedef enum TokenKind
{
	TOKEN_LAST_CHAR = 127,
	TOKEN_INT,
	TOKEN_NAME,
} TokenKind;

typedef struct Token
{
	TokenKind kind;
	const char* start;
	const char* end;
	union {
		int32_t val;
		const char* name;
	};
} Token;

const char* token_kind_name(TokenKind kind)
{
	// TODO(scott): fix this
	static char buf[256];
	switch (kind)
	{
		case TOKEN_INT: {
			sprintf(buf, "integer");
		} break;
		case TOKEN_NAME: {
			sprintf(buf, "name");
		} break;
		default: {
			if (kind < 128 && isprint(kind))
			{
				sprintf(buf, "%c", (char)kind);
			}
			else
			{
				sprintf(buf, "<ASCII %d>", kind);
			}
		} break;
	}
	return buf;
}

Token token;
const char* stream;

const char* keyword_if;
const char* keyword_for;
const char* keyword_while;

void init_keywords()
{
	keyword_if = str_intern("if");
	keyword_for = str_intern("for");
	keyword_while = str_intern("while");
}

void next_token()
{
	token.start = stream;
	switch (*stream)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		int32_t val = 0;
		while (isdigit(*stream))
		{
			val *= 10;
			val += *stream - '0';
			stream++;
		}			
		token.kind = TOKEN_INT;
		token.val = val;
	} break;

	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_': {
		const char* start = stream;
		while (isalnum(*stream) || *stream == '_')
			stream++;
		token.kind = TOKEN_NAME;
		token.name = str_intern_range(token.start, stream);
	} break;

	default:
		token.kind = *stream++;
		break;
	}
	token.end = stream;
}

void init_stream(const char* str)
{
	stream = str;
	next_token();
}

void print_token(Token token)
{
	switch (token.kind)
	{
	case TOKEN_INT:
		printf("TOKEN INT: %d\n", token.val);
		break;
	case TOKEN_NAME:
		printf("TOKEN NAME: %.*s\n", token.end - token.start, token.start);
		break;
	default:
		printf("TOKEN: '%c'\n", token.kind);
		break;
	}
}

void lex_test()
{
	char* source = "XY+(XY)_Hello1!234+554";
	stream = source;
	next_token();
	while (token.kind)
	{
		//print_token(token);
		next_token();
	}
}

inline bool is_token(TokenKind kind)
{
	return token.kind == kind;
}

inline bool is_token_name(const char* name)
{
	return token.kind == TOKEN_NAME && token.name == name;
}

inline bool match_token(TokenKind kind)
{
	if (is_token(kind))
	{
		next_token();
		return true;
	}
	else
	{
		return false;
	}
}

inline bool expect_token(TokenKind kind)
{
	if (is_token(kind))
	{
		next_token();
		return true;
	}
	else
	{
		fatal("expected token: %s, got %s\n", token_kind_name(kind), token_kind_name(token.kind));
		return false;
	}
}

/*
expr3 = INT | '(' expr ')'
expr2 = -expr2 | expr3
expr1 = expr2 ([/*] expr2)*
expr0 = expr1 ([+-] expr1)*
expr = expr0
*/
int parse_expr();

int parse_expr3()
{
	if (is_token(TOKEN_INT))
	{
		int val = token.val;
		next_token();
		return val;
	}
	else if (match_token('('))
	{
		int val = parse_expr();
		expect_token(')');
		return val;
	}
	else
	{
		fatal("expected integer or '(', got %s", token_kind_name(token.kind));
		return 0;
	}
}

int parse_expr2()
{
	if (match_token('-'))
	{
		return -parse_expr2();
	}
	else
	{
		return parse_expr3();
	}
}

int parse_expr1()
{
	int val = parse_expr2();
	while (is_token('/') || is_token('*'))
	{
		char op = token.kind;
		next_token();
		int rval = parse_expr2();
		if (op == '/')
		{
			assert(rval != 0);
			val /= rval;
		}
		else
		{
			val *= rval;
		}
	}
	return val;
}

int parse_expr0()
{
	int val = parse_expr1();
	while (is_token('+') || is_token('-'))
	{
		char op = token.kind;
		next_token();
		int rval = parse_expr1();
		if (op == '+')
		{
			val += rval;
		}
		else
		{
			val -= rval;
		}
	}
	return val;
}

int parse_expr()
{
	return parse_expr0();
}

int test_parse_expr(const char* expr)
{
	init_stream(expr);
	return parse_expr();
}

#define TEST_EXPR(x) assert(test_parse_expr(#x) == (x))
void parse_test()
{
	TEST_EXPR(1);
	TEST_EXPR((1));
	TEST_EXPR(-1);
	TEST_EXPR(1-2-3);
	TEST_EXPR(2*3+4*5);
	TEST_EXPR(2*(3+4)*5);
	TEST_EXPR(2*-3+-10/5);
}
#undef TEST_EXPR

int main(int ArgCount, char** Args)
{
	buf_test();
	lex_test();
	str_intern_test();
	parse_test();
	return 0;
}