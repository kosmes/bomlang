//
// Created by jiney on 2019-03-17.
//

#include "error.h"

static unsigned short err_count = 0;

const u16char *errstr[] = {
        L"오류",
        L"구문 오류.",
        L"식이 없습니다.",
        L"';'가 필요합니다.",
        L"자료형이 필요합니다.",
        L"식별자가 필요합니다.",
        L"'(' ')'가 올바르게 연결되지 않았습니다.",
        L"가득찬 스택에 데이터를 집어넣으려고 시도했습니다.",
        L"비어있는 스택에 데이터를 가져오려고 시도했습니다.",
        L"0으로 나누었습니다.",
        L"파일 끝이 잘못되었습니다.",
        L"잘못된 색인을 참조하였습니다.",
        L"잘못된 접근입니다.",
        L"잘못된 문자입니다.",
        L"선언하지 않은 변수입니다.",
        L"재 선언된 변수입니다.",
};

void Error(enum ERROR_CODE err) {
    err_count++;
    wprintf(L"오류 %ls\n", errstr[err]);
}

void ErrorLine(enum ERROR_CODE err, unsigned int line) {
    err_count++;
    wprintf(L"오류 [ %d 줄 ] %ls\n", line, errstr[err]);
}

void ErrorResetCount() {
    err_count = 0;
}

unsigned short ErrorGetCount() {
    return err_count;
}