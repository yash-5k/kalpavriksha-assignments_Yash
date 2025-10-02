#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPRESSION_LENGTH 150

// Useful flags to signal error conditions
int error_divide_by_zero = 0;
int error_invalid_syntax = 0;

// Trims leading and trailing spaces from the string input
void trim_spaces(const char **text) {
    while (**text && isspace(**text)) (*text)++;
}

// Reads and converts the next integer from the string
int read_integer(const char **text) {
    trim_spaces(text);
    int sign = 1;
    if (**text == '-') {
        sign = -1;
        (*text)++;
    }
    if (!isdigit(**text)) {
        error_invalid_syntax = 1;
        return 0;
    }
    int value = 0;
    while (isdigit(**text)) {
        value = value * 10 + (**text - '0');
        (*text)++;
    }
    return sign * value;
}

// Handles '*' and '/' operations with precedence
int calculate_md(const char **text) {
    trim_spaces(text);
    int result = read_integer(text);
    trim_spaces(text);
    while (**text == '*' || **text == '/') {
        char op = **text;
        (*text)++;
        int right_side = read_integer(text);
        if (error_invalid_syntax) return 0;
        if (op == '*')
            result *= right_side;
        else {
            if (right_side == 0) {
                error_divide_by_zero = 1;
                return 0;
            }
            result /= right_side;
        }
        trim_spaces(text);
    }
    return result;
}

// Handles '+' and '-' operations
int calculate_as(const char **text) {
    trim_spaces(text);
    int result = calculate_md(text);
    trim_spaces(text);
    while (**text == '+' || **text == '-') {
        char op = **text;
        (*text)++;
        int right_side = calculate_md(text);
        if (error_invalid_syntax || error_divide_by_zero) return 0;
        if (op == '+') result += right_side;
        else result -= right_side;
        trim_spaces(text);
    }
    return result;
}

// Checks if the input has forbidden characters
int contains_bad_character(const char *input) {
    while (*input) {
        if (!isdigit(*input) && !isspace(*input) &&
            *input != '+' && *input != '-' && *input != '*' && *input != '/')
            return 1;
        input++;
    }
    return 0;
}

// Removes starting and ending quotes, if present
void remove_quotes(char *text) {
    int len = strlen(text);
    if (len >= 2 && text[0] == '"' && text[len-1] == '"') {
        // Remove ending quote
        text[len-1] = '\0';
        // Remove starting quote
        memmove(text, text+1, len-1);
    }
}

int main() {
    char expression[MAX_EXPRESSION_LENGTH];
    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        printf("Error: Invalid expression.\n");
        return 0;
    }
    // Remove newline at the end, if any
    expression[strcspn(expression, "\n")] = '\0';

    // If user entered string with quotes, strip them
    remove_quotes(expression);

    // Check for any disallowed characters
    if (contains_bad_character(expression)) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    // Start parsing and calculating
    const char *scan = expression;
    int answer = calculate_as(&scan);

    trim_spaces(&scan);
    if (error_invalid_syntax || *scan != '\0') {
        printf("Error: Invalid expression.\n");
    } else if (error_divide_by_zero) {
        printf("Error: Division by zero.\n");
    } else {
        printf("%d\n", answer);
    }
    return 0;
}
