#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <QString>

enum SType
{
    TR,
    EN
};

static QString baseApi = "https://speech.googleapis.com/v1/speech:recognize";
static QString apiKey = "your api";

#endif // CONSTANTS_H
