#ifndef _STRING_H
#define _STRING_H

strlen(s) {
    auto p; p = s;
    while (*(++s));
    return s - p;
}

strcpy(d, s) {
    while (*s) *(d++) = *(s++);
    *d = 0;
}

strcat(d, s) {
    while (*d) d++;
    while (*s) *(d++) = *(s++);
    *d = 0;
}

#endif
