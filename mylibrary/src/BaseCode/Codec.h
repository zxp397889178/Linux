#ifndef _GFX_BASELIB_CODEC_H_
#define _GFX_BASELIB_CODEC_H_

//mysql �� iconv ��ʹ�õ��ַ��������ǲ�ͬ�ģ���Ҫ������Ӧ��ϵ
//mysql5���ַ��� https://dev.mysql.com/doc/refman/5.1/zh/charset.html#charset-charsets
//iconv���ַ��� http://blog.csdn.net/jaylong35/article/details/6126289

const char* GetIconvCharset(const char *mysql_charset);
const char* GetMysqlCharset(const char *iconv_charset);

/*!
���ַ����������Ա���ת��
����:
    fromCharset ԭʼ���룬����"GB2312",�İ���iconv֧�ֵ�д
    toCharset   ת����Ŀ�ı���
    toStr       ת��������ݱ��浽���ָ�����Ҫ���ⲿ�����ڴ�
    cbToStr     �洢ת�������ݵ��ڴ��С
    fromStr     ԭʼ��Ҫת�����ַ���
    cbFromStr   ԭʼ�ַ�������
����ֵ��
    0           ת���ɹ�
    -1          ��������
    -2          ��֧���ַ���ת��
    -3          toStr�Ŀռ䲻��
    -4          fromStr�в���ת�����ַ���ת��ʧ��
    -5          ��������
*/
int Convert(const char *fromCharset, const char *toCharset, 
    char *toStr, size_t &cbToStr, const char *fromStr, const size_t cbFromStr);

int FileConvert(const char *fromCharset, const char *toCharset,
    const char* fromFile, const char* toFile, const char* lineEnding);

int utf8_to_gbk(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);
int gbk_to_utf8(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);
int utf16_to_gbk(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);
int gbk_to_utf16(char *to_string, size_t &to_size, const char *from_string, const size_t from_size);


// �ж��ַ�������Ϊ utf8 ����
bool IsUTF8String(const char* str, int32_t length);


// �ж��Ƿ�Ϊָ���ı��룬���ҷ����ַ�����һ�����֣���һ����ĸ����Ϊһ���ַ���
// RETURN: -1  not code
//         >=0 encode character size
// length: character size in str
int32_t IsUtf8Code(const std::string &str, int32_t &length);
int32_t IsGB2312Code(const std::string &str, int32_t &length);
int32_t IsBig5Code(const std::string &str, int32_t &length);
int32_t IsGBKCode(const std::string &str, int32_t &length);

#endif  //_GFX_BASELIB_CODEC_H_



