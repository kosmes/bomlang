//
// Created by jiney on 2019-03-19.
//

#ifndef BOM_ENCODING_H
#define BOM_ENCODING_H

typedef unsigned char u8char;
typedef unsigned short u16char;

typedef enum FILE_FORMATS FILE_FORMATS;

enum FILE_FORMATS {
    FORMAT_ANCII,
    FORMAT_UTF_8,
    FORMAT_UTF_8_BOM,
};

#endif //BOM_ENCODING_H
