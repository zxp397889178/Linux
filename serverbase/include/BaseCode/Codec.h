#ifndef _GFX_BASELIB_CODEC_H_
#define _GFX_BASELIB_CODEC_H_

//mysql 与 iconv 中使用的字符集名称是不同的，需要构建对应关系
//mysql5的字符集 https://dev.mysql.com/doc/refman/5.1/zh/charset.html#charset-charsets
//iconv的字符集 http://blog.csdn.net/jaylong35/article/details/6126289

const char* GetIconvCharset(const char *mysql_charset);
const char* GetMysqlCharset(const char *iconv_charset);

/*!
对字符串进行语言编码转换
参数:
    fromCharset 原始编码，比如"GB2312",的按照iconv支持的写
    toCharset   转换的目的编码
    toStr       转换后的数据保存到这个指针里，需要在外部分配内存
    cbToStr     存储转换后数据的内存大小
    fromStr     原始需要转换的字符串
    cbFromStr   原始字符串长度
返回值：
    0           转换成功
    -1          参数错误
    -2          不支持字符串转换
    -3          toStr的空间不足
    -4          fromStr有不可转换的字符，转换失败
    -5          其他错误
*/
int Convert(const char *fromCharset, const char *toCharset, 
    char *toStr, size_t &cbToStr, const char *fromStr, const size_t cbFromStr);

int FileConvert(const char *fromCharset, const char *toCharset,
    const char* fromFile, const char* toFile, const char* lineEnding);

int utf8_to_gbk(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);
int gbk_to_utf8(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);
int utf16_to_gbk(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);
int gbk_to_utf16(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);


// 判断字符串量否为 utf8 编码
bool IsUTF8String(const char* str, int32_t length);


// 判断是否为指定的编码，并且返回字符数（一个字字，或一个字母，均为一个字符）
// RETURN: -1  not code
//         >=0 encode character size
// length: character size in str
int32_t IsUtf8Code(const std::string &str, int32_t &length);
int32_t IsGB2312Code(const std::string &str, int32_t &length);
int32_t IsBig5Code(const std::string &str, int32_t &length);
int32_t IsGBKCode(const std::string &str, int32_t &length);

#endif  //_GFX_BASELIB_CODEC_H_



