//
// Created by jiney on 2019-03-17.
//

#include "errno.h"

void error(enum errco err, unsigned int line) {
    wprintf(L"오류 [ %d 줄 ] %ls", line, errstr[err]);
}