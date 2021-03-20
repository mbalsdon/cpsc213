#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

struct STR {
    int size;
    char* ptr;
};

char _str1[30] = "Welcome! Please enter a name:\n";
struct STR str1 = { .size = 30, .ptr = _str1 };

char _str2[11] = "Good luck, ";
struct STR str2 = { .size = 11, .ptr = _str2 };

char _str3[43] = "The secret phrase is \"squeamish ossifrage\"\n";
struct STR str3 = { .size = 43, .ptr = _str3 };

void print(struct STR* s) {
    write(1, s->ptr, s->size);
}

void proof() {
    print(&str3);
}

int main() {

    char something[128];
    print(&str1);
    int r = read(0, &something, 256);
    print(&str2);
    write(1, &something, r);
    return 0;
}