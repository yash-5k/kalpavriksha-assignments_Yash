#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_EXPRESSION_LENGTH 150
#define PARSE_ERROR LLONG_MAX

const char *scan;

void trim_spaces() {
    while (isspace(*scan)) {
        scan++;
    }
}

long long read_number() {
    trim_spaces();
    long long value = 0;

    if (!isdigit(*scan)) {
        return PARSE_ERROR; // Error: expected a number
    }

    while (isdigit(*scan)) {
        value = value * 10 + (*scan - '0');
        scan++;
    }
    return value;
}

long long calculate_md() {
    long long result = read_number();
    if (result == PARSE_ERROR) return PARSE_ERROR;

    while (1) {
        trim_spaces();
        if (*scan == '*') {
            scan++;
            long long num = read_number();
            if (num == PARSE_ERROR) return PARSE_ERROR;
            result *= num;
        } else if (*scan == '/') {
            scan++;
            long long num = read_number();
            if (num == PARSE_ERROR) return PARSE_ERROR;
            if (num == 0) return PARSE_ERROR; // Division by zero
            result /= num;
        } else {
            break;
        }
    }
    return result;
}

long long calculate_as() {
    long long result = calculate_md();
    if (result == PARSE_ERROR) return PARSE_ERROR;

    while (1) {
        trim_spaces();
        if (*scan == '+') {
            scan++;
            long long num = calculate_md();
            if (num == PARSE_ERROR) return PARSE_ERROR;
            result += num;
        } else if (*scan == '-') {
            scan++;
            long long num = calculate_md();
            if (num == PARSE_ERROR) return PARSE_ERROR;
            result -= num;
        } else {
            break;
        }
    }
    return result;
}

int main() {
    char expression[MAX_EXPRESSION_LENGTH];

    printf("Enter an expression: ");
    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        return 1;
    }

    expression[strcspn(expression, "\n")] = '\0';

    scan = expression;
    long long answer = calculate_as();

    trim_spaces();

    if (answer == PARSE_ERROR) {
        printf("Error: Invalid expression or division by zero.\n");
    } else if (*scan != '\0') {
        // If there are characters left over, the syntax was wrong
        printf("Error: Invalid expression.\n");
    } else {
        printf("Result: %lld\n", answer);
    }

    return 0;
}