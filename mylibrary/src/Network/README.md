2020-07-24
1、对 CHttpRequest 增加 校验参数是否为空，把参数导出到CHttpRespMaker(用于验签之用)
    bool ValidParam(std::vector<CMyString> &vecParam, CMyString &strResult);

    int32_t BuildRespMaker(CHttpRespMaker &respMaker);
    int32_t BuildRespMaker(std::vector<CMyString &vecParam, CHttpRespMaker &respMaker);

2、在 CHttpRespMaker中增加修改参数类型的函数
    bool setFieldType(const CMyString &key, char cType);
    

