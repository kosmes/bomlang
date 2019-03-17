//
// Created by jiney on 2019-03-17.
//

#include "errno.h"

const wchar_t *errstr[] = {
        L"구문 오류.",
        L"식이 없습니다.",
        L"';'가 필요합니다.",
        L"'(' ')'가 올바르게 연결되지 않았습니다.",
};

void error(enum errco err, unsigned int line) {
    wprintf(L"오류 [ %d 줄 ] %ls", line, errstr[err]);
}