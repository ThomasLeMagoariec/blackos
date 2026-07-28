#include "printf_engine.h"
#include <stdbool.h>

static const char hex_lower[] = "0123456789abcdef";
static const char hex_upper[] = "0123456789ABCDEF";

static int emit_unsigned(putc_fn out, void* ctx, unsigned long long val, int radix, const char* digits) {
    char buf[32]; // Enough for 64-bit binary
    int pos = 0;
    do {
        buf[pos++] = digits[val % radix];
        val /= radix;
    } while (val > 0);
    for (int i = pos - 1; i >= 0; i--) {
        out(buf[i], ctx);
    }
    return pos;
}

static int emit_signed(putc_fn out, void* ctx, long long val, int radix, const char* digits) {
    if (val < 0) {
        out('-', ctx);
        return 1 + emit_unsigned(out, ctx, (unsigned long long)(-val), radix, digits);
    }
    return emit_unsigned(out, ctx, (unsigned long long)val, radix, digits);
}

typedef enum { LEN_DEFAULT, LEN_HH, LEN_H, LEN_L, LEN_LL } length_mod_t;

static const char* parse_length(const char* p, length_mod_t* len) {
    *len = LEN_DEFAULT;
    if (*p == 'h') {
        p++;
        if (*p == 'h') { *len = LEN_HH; p++; } else { *len = LEN_H; }
    } else if (*p == 'l') {
        p++;
        if (*p == 'l') { *len = LEN_LL; p++; } else { *len = LEN_L; }
    }
    return p;
}

typedef int (*spec_handler_t)(putc_fn out, void* ctx, va_list* args, length_mod_t len);

static int handle_c(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    (void)len;
    out((char)va_arg(*args, int), ctx);
    return 1;
}

static int handle_s(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    (void)len;
    const char* s = va_arg(*args, const char*);
    if (!s) s = "(null)";
    int written = 0;
    while (*s) { out(*s++, ctx); written++; }
    return written;
}

static int handle_d(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    long long val;
    switch (len) {
        case LEN_LL: val = va_arg(*args, long long); break;
        case LEN_L:  val = va_arg(*args, long); break;
        default:     val = va_arg(*args, int); break;
    }
    return emit_signed(out, ctx, val, 10, hex_lower);
}

static int handle_u(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    unsigned long long val;
    switch (len) {
        case LEN_LL: val = va_arg(*args, unsigned long long); break;
        case LEN_L:  val = va_arg(*args, unsigned long); break;
        default:     val = va_arg(*args, unsigned int); break;
    }
    return emit_unsigned(out, ctx, val, 10, hex_lower);
}

static int handle_x(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    unsigned long long val;
    switch (len) {
        case LEN_LL: val = va_arg(*args, unsigned long long); break;
        case LEN_L:  val = va_arg(*args, unsigned long); break;
        default:     val = va_arg(*args, unsigned int); break;
    }
    return emit_unsigned(out, ctx, val, 16, hex_lower);
}

static int handle_X(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    unsigned long long val;
    switch (len) {
        case LEN_LL: val = va_arg(*args, unsigned long long); break;
        case LEN_L:  val = va_arg(*args, unsigned long); break;
        default:     val = va_arg(*args, unsigned int); break;
    }
    return emit_unsigned(out, ctx, val, 16, hex_upper);
}

static int handle_o(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    unsigned long long val;
    switch (len) {
        case LEN_LL: val = va_arg(*args, unsigned long long); break;
        case LEN_L:  val = va_arg(*args, unsigned long); break;
        default:     val = va_arg(*args, unsigned int); break;
    }
    return emit_unsigned(out, ctx, val, 8, hex_lower);
}

static int handle_p(putc_fn out, void* ctx, va_list* args, length_mod_t len) {
    (void)len;
    unsigned long long val = (unsigned long long)(uintptr_t)va_arg(*args, void*);
    out('0', ctx); out('x', ctx);
    return 2 + emit_unsigned(out, ctx, val, 16, hex_lower);
}

static const spec_handler_t spec_table[128] = {
    ['c'] = handle_c,
    ['s'] = handle_s,
    ['d'] = handle_d,
    ['i'] = handle_d,
    ['u'] = handle_u,
    ['x'] = handle_x,
    ['X'] = handle_X,
    ['o'] = handle_o,
    ['p'] = handle_p,
};

int vprintfn(putc_fn out, void* ctx, const char* fmt, va_list args) {
    int written = 0;
    while (*fmt) {
        if (*fmt != '%') { out(*fmt++, ctx); written++; continue; }
        fmt++;
        if (*fmt == '%') { out('%', ctx); written++; fmt++; continue; }

        length_mod_t len;
        fmt = parse_length(fmt, &len);

        int c = (unsigned char)*fmt;
        if (c < 128 && spec_table[c]) {
            written += spec_table[c](out, ctx, &args, len);
        } else {
            out('%', ctx); out((char)c, ctx); written += 2;
        }
        if (*fmt) fmt++;
    }
    return written;
}

typedef struct {
    char* buf;
    uint32_t size;
    uint32_t pos;
} buf_ctx_t;

static void buf_putc(char c, void* raw_ctx) {
    buf_ctx_t* ctx = (buf_ctx_t*)raw_ctx;
    if (ctx->size > 0 && ctx->pos + 1 < ctx->size) {
        ctx->buf[ctx->pos] = c;
    }
    ctx->pos++;
}

int vsnprintf(char* buf, uint32_t size, const char* fmt, va_list args) {
    buf_ctx_t ctx = { .buf = buf, .size = size, .pos = 0 };
    int written = vprintfn(buf_putc, &ctx, fmt, args);
    if (size > 0) {
        buf[ctx.pos < size ? ctx.pos : size - 1] = '\0';
    }
    return written;
}

int snprintf(char* buf, uint32_t size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}
