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

void syntax_error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("Syntax Error: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

// Stretchy buffers
typedef struct buf_hdr
{
    size_t len;
    size_t cap;
    char buf[0];
} buf_hdr;

#define BUF(x) x*
#define buf__hdr(b) ((buf_hdr*)((char*)(b) - offsetof(buf_hdr, buf)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n) (buf__fits((b), (n)) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b)  ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b)  ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b)  ((b) + buf_len(b))
#define buf_push(b, ...) (buf__fit((b), 1), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_free(b) ((b) ? free(buf__hdr(b)), (b) = NULL : 0)

void* buf__grow(const void* buf, size_t new_len, size_t elem_size)
{
    assert(buf_cap(buf) <= (SIZE_MAX - 1) / 2);
    size_t new_cap = MAX(1 + 2 * buf_cap(buf), new_len);
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(buf_hdr, buf)) / elem_size);
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
} intern;

static intern* interns;

const char* str_intern_range(const char* start, const char* end)
{
    size_t len = end - start;
    for(intern* it = interns; it != buf_end(interns); it++)
    {
        if (it->len == len && strncmp(it->str, start, len) == 0)
        {
            return it->str;
        }
    }
    char* str = malloc(len + 1);
    memcpy(str, start, len);
    str[len] = 0;
    buf_push(interns, ((intern){ len, str }));
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

void common_test()
{
    buf_test();
    str_intern_test();
}