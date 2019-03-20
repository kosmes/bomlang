//
// Created by jiney on 2019-03-17.
//

#include "error.h"

const u16char *errstr[] = {
        L"오류",
        L"구문 오류.",
        L"식이 없습니다.",
        L"';'가 필요합니다.",
        L"'(' ')'가 올바르게 연결되지 않았습니다.",
        L"가득찬 스택에 데이터를 집어넣으려고 시도했습니다.",
        L"비어있는 스택에 데이터를 가져오려고 시도했습니다.",
        L"0으로 나누었습니다.",
        L"파일 끝이 잘못되었습니다.",
        L"잘못된 색인을 참조하였습니다.",
        L"잘못된 접근입니다.",
        L"선언하지 않은 변수입니다.",
};

void error(enum ERROR_CODE err) {
    wprintf(L"오류 %ls\n", errstr[err]);
}

void error_line(enum ERROR_CODE err, unsigned int line) {
    wprintf(L"오류 [ %d 줄 ] %ls\n", line, errstr[err]);
}