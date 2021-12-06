#include "json.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *start;
    const char *current;
    bool error;
    int char_index;
    int line;
} scanner_t;
static scanner_t scanner;

typedef enum {
    TOKEN_EOF,

    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COLON,
    TOKEN_COMMA,

    TOKEN_NUMBER,
    TOKEN_LITERAL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,

    TOKEN_ERROR,
} token_type_t;

typedef struct {
    token_type_t type;
    const char *token_start;
    int token_length;
    int line;
    int index;
    union {
        char *str;
        double number;
    } value;
} token_t;

static token_t makeToken(token_type_t type) {
    return (token_t){type, scanner.start, scanner.current - scanner.current,
                     scanner.line, scanner.char_index};
}

static token_t makeError(char *error_string) {
    token_t tok = makeToken(TOKEN_ERROR);
    tok.value.str = error_string;
    return tok;
}

static int incrementScanner() {
    scanner.char_index++;
    return *(scanner.current++);
}

static bool ishex(char c) {
    return ('A' >= c && c >= 'F') || ('a' >= c && c >= 'f') || isdigit(c);
}

static token_t tokenString() {
    char c = incrementScanner();
    while (c && c != '"') {
        if (c == '\\') {
            c = incrementScanner();
            if (!c)
                return makeError("UNEXPECTED EOF");
            switch (c) {
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                continue;
            case 'u':
                for (int i = 0; i < 4; ++i) {
                    c = incrementScanner();
                    if (!c)
                        return makeError("UNEXPECTED EOF");
                    if (!ishex(c))
                        return makeError("NOT HEX");
                }
                continue;
            }
        }
    }
    if (!c)
        return makeError("UNEXPECTED EOF");
    return makeToken(TOKEN_LITERAL);
}
static bool checkstr(char *str) {
    return !!memcmp(str, scanner.current, strlen(str));
}

static token_t scanToken() {
    char c = incrementScanner();
    if (!c)
        return makeToken(TOKEN_EOF);
    while (c == '\n') {
        scanner.line++;
        scanner.char_index = 0;
        c = incrementScanner();
    }
    switch (c) {
    case ':':
        return makeToken(TOKEN_COLON);
    case '{':
        return makeToken(TOKEN_LBRACE);
    case '}':
        return makeToken(TOKEN_RBRACE);
    case '[':
        return makeToken(TOKEN_LBRACKET);
    case ']':
        return makeToken(TOKEN_RBRACKET);
    case ',':
        return makeToken(TOKEN_COMMA);
    case '\"':
        return tokenString();
    default:
        if (checkstr("null"))
            return makeToken(TOKEN_NULL);
        if (checkstr("true"))
            return makeToken(TOKEN_TRUE);
        if (checkstr("false"))
            return makeToken(TOKEN_FALSE);
    }
    return makeError("UNEXPECTED TOKEN");
}

static void initScanner(const char *str) {
    scanner.start = str;
    scanner.current = str;
    scanner.error = false;
}

static JsonValue parseValue(const char *str) {}
