#include <stdlib.h>
#include <string.h>
#include "Util.h"
// get the substring of a string
// remember to free the returned pointer
char* substring(char* str, int start, int end) {
    int len = end - start;
    char* substr = malloc(len + 1);
    strncpy(substr, str + start, len);
    substr[len] = '\0';
    return substr;
}
