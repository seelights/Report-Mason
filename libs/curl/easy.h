#ifndef EASY_H
#define EASY_H

// Minimal stub for easy.h (Curl easy interface)
// This file would normally contain easy interface function declarations

// Function declarations
CURL *curl_easy_init(void);
void curl_easy_cleanup(CURL *curl);
int curl_easy_setopt(CURL *curl, int option, ...);
int curl_easy_perform(CURL *curl);
int curl_easy_getinfo(CURL *curl, int info, ...);
void curl_easy_reset(CURL *curl);
const char *curl_easy_strerror(int);

#endif // EASY_H
