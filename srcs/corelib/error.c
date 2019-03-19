//
// Created by jiney on 2019-03-17.
//

#include "error.h"

const wchar_t *errstr[] = {
        L"구문 오류.",
        L"식이 없습니다.",
        L"';'가 필요합니다.",
        L"'(' ')'가 올바르게 연결되지 않았습니다.",
        L"가득찬 스택에 데이터를 집어넣으려고 시도했습니다.",
        L"비어있는 스택에 데이터를 가져오려고 시도했습니다.",
        L"0으로 나누었습니다.",
};

void error(enum ERROR_CODE err, unsigned int line) {
    wprintf(L"오류 [ %d 줄 ] %ls\n", line, errstr[err]);
}