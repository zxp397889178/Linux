/*
 * =====================================================================================
 *
 *       Filename:  HttpFunc.cpp
 *
 *    Description:  http 定义的函数
 *
 *        Version:  1.0
 *        Created:  2020年02月13日 23时18分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "HttpFunc.h"
#include <openssl/md5.h>
#include "TcpClient.h"


/*
 * 返回值：  1 处理结果未知，需要对resp进一步处理
 *           0 处理成功
 *          -1 URL错误
 *          -3 文件无法打开
 *          -5 文件超过2.1G
 *          -7 地址无法连接
 *         -11 接收数据错误
 */
int32_t HttpSendFile(const CMyString &strUrl, const CMyString &strFile, http::CHttpRespone &resp)
{
    CMyString strHost;
    int32_t port;
    CMyString strCgi;
    CMyString strParam;
    bool bSSL(false);
    if (!decode_url(strUrl, strHost, port, strCgi, strParam, bSSL))
    {
        LogFatal("[%s:%s:%d]decode url [%s] failure", __FILE__, __func__, __LINE__, strUrl.c_str());
        return -1;
    }

    FILE *fp = fopen(strFile.c_str(), "rb");
    if (fp == NULL)
    {
        LogFatal("[%s:%s:%d]open file %s failure.", __FILE__, __func__, __LINE__, strFile.c_str());
        return -3;
    }
    CAutoFile guard(fp);
    fseek(fp, 0, SEEK_END);
    int32_t fileSize = ftell(fp);
    if (fileSize < 0)
    {
        LogFatal("[%s:%s:%d]file %s size overflow", __FILE__, __func__, __LINE__, strFile.c_str());
        return -5;
    }
    fseek(fp, 0, SEEK_SET);

    net::CTcpClient tcp;
    int32_t ret = tcp.Open(strHost, port, bSSL);
    if (ret < 0)
    {
        LogFatal("[%s:%s:%d]connect to %s:%d failure.", __FILE__, __func__, __LINE__, strHost.c_str(), port);
        return -7;
    }

    // 查询文件类型
    CMyString strContentType;
    if (!GetContentType(strFile, strContentType))
    {
        strContentType = "text/plain";
    }

    // 构建HTTP头
    CMyString strHeader;
    if (strParam.empty())
        strHeader.Format("POST /%s HTTP/1.1\r\n", strCgi.c_str());
    else
        strHeader.Format("POST /%s?%s HTTP/1.1\r\n", strCgi.c_str(), strParam.c_str());
    strHeader.Append("User-Agent: webmw/1.0.22\r\n");
    strHeader.Append("Host: %s:%d\r\n", strHost.c_str(), port);
    strHeader.Append("Cache-Control: no-cache\r\n");
    strHeader.Append("Connection: keep-alive\r\n");
    strHeader.Append("Accept: */*\r\n");
    strHeader.Append("Content-Type: %s\r\n", strContentType.c_str());
    strHeader.Append("Content-Disposition: attachment; filename=\"%s\" \r\n", strFile.c_str());

    int32_t result(0);
    int32_t sendSize(0);
    char szText[1024] = "";
    size_t cbText=sizeof(szText);
    if (fileSize < 2048)
    {
        // 包文在10K以内，直接发送
        CTempBuffer tmpbuf(fileSize+1);
        fseek(fp, 0, SEEK_SET);
        size_t size = fread(tmpbuf.data(), 1, fileSize, fp);
        strHeader.Append("Accept-Encoding: *\r\n");
        strHeader.Append("Content-Length: %d\r\n", fileSize);
        strHeader.Append("\r\n");

        // 发送请求数据
        ret = tcp.SendMsg((void*)strHeader.c_str(), strlen(strHeader.c_str())) ;
        if (ret < 0)
        {
            LogFatal("[%s:%s:%d]send header to %s:%d failure, ret=%d",
                    __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
            return -9;
        }
        sendSize += ret;
        ret = tcp.SendMsg((void*)tmpbuf.data(), size);
        if (ret < 0)
        {
            LogFatal("[%s:%s:%d]send data to %s:%d failure, ret=%d",
                    __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
            return -9;
        }
        sendSize += ret;
        LogDebug("send %d bytes to %s:%d succeed.", sendSize, strHost.c_str(), port);

        result = 0;
        do
        {
            ret = tcp.RecvMsg(szText, cbText);
            if (ret > 0)
            {
                resp.read((void*)&szText[0], ret);
            }
            else if (ret==0)
            {
                break;
            }
            else
            {
                result = -11;
                break;
            }
        }while(true);
        if (result < 0) return result;
        int32_t code = resp.getStatusCode();
        if (code ==0 || code==200)
        {
            return 0;
        }
        return 1;
    }

    // 超过2K的包文，以 chunk 模式发送
    // TODO: 暂不支持压缩方式
    //strHeader.Append("Accept-Encoding: gzip, deflate, br\r\n");
    strHeader.Append("Accept-Encoding: *\r\n");
    strHeader.Append("Transfer-Encoding: chunked\r\n");
    // strHeader.Append("Trailer: Content-MD5\r\n");
    strHeader.Append("\r\n");

    ret = tcp.SendMsg((void*)strHeader.c_str(), strlen(strHeader.c_str()));
    if (ret < 0)
    {
        LogFatal("[%s:%s:%d]send header to %s:%d failure, ret=%d", __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
        return -9;
    }
    sendSize += ret;
    printf("%ssend header, %d bytes:%s\n%s\n", _color_yellow, ret, _color_end, strHeader.c_str());

    int32_t chunkIndex(0);
    // 以chunk模式发送数据
    byte_t chunkData[1024] = {0, };
    size_t cbChunk = sizeof(chunkData);
    char szChunkHeader[32] = "";
    char szChunkEndl[32] = "";
    strcpy(szChunkEndl, "\r\n");
    // 同时计算MD5值
    MD5_CTX ctx;
    uint8_t md5[16] = {0,};
    MD5_Init(&ctx);
    fseek(fp, 0, SEEK_SET);
    do {
        size_t size = fread(chunkData, 1, cbChunk, fp);
        MD5_Update(&ctx, chunkData, size);
        sprintf(szChunkHeader, "%x\r\n", (uint32_t)size);
        ret = tcp.SendMsg((void*)szChunkHeader, strlen(szChunkHeader));
        if (ret < 0)
        {
            LogFatal("[%s:%s:%d]send chunk data to %s:%d failure, ret=%d", __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
            return -9;
        }
        sendSize += ret;
        ret = tcp.SendMsg(chunkData, size);
        if (ret < 0)
        {
            LogFatal("[%s:%s:%d]send chunk data to %s:%d failure, ret=%d", __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
            return -9;
        }
        sendSize += ret;
        ret = tcp.SendMsg((void*)szChunkEndl, strlen(szChunkEndl));
        if (ret < 0)
        {
            LogFatal("[%s:%s:%d]send chunk data to %s:%d failure, ret=%d", __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
            return -9;
        }
        sendSize += ret;
        printf("send chunked, index:%s%d%s size:%s%lu%s\n", _color_yellow, ++chunkIndex, _color_end, _color_yellow, size, _color_end);

    }while(!feof(fp));
    MD5_Final(md5, &ctx);

    // 发送结束标识
    char szChunkFinish[32] = "";
    strcpy(szChunkFinish, "0\r\n\r\n");
    ret = tcp.SendMsg((void*)szChunkFinish, strlen(szChunkFinish));
    if (ret < 0)
    {
        LogFatal("[%s:%s:%d]send chunk finish flag to %s:%d failure, ret=%d", __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
        return -9;
    }
    sendSize += ret;
    printf("%schunck send finished%s.\n\n\n", _color_red, _color_end);

    // 计算md5结果
    char szContentMD5[128] = "";
    char *pc = &szContentMD5[0];
    //strcpy(szContentMD5, "Content-MD5: ");
    //char *pc = &szContentMD5[strlen(szContentMD5)];
    for(int32_t i(0); i<(int32_t)sizeof(md5); ++i)
    {
        uint8_t c = md5[i];
        sprintf(pc+(i*2), "%02x", c);
    }
    //strcat(szContentMD5, "\r\n");
    //ret = tcp.SendMsg((void*)szContentMD5, strlen(szContentMD5));
    //if (ret < 0)
    //{
    //    LogFatal("[%s:%s:%d]send chunk MD5 to %s:%d failure, ret=%d", __FILE__, __func__, __LINE__, strHost.c_str(), port, ret);
    //    return -9;
    //}
    //printf("send md5 checksum, %d bytes, data size:%d\n",  ret, sendSize);
    //sendSize += ret;
    //printf("md5:%s\n", szContentMD5);

    LogDebug("total send %d bytes, file size %d bytes, md5:%s", sendSize, fileSize, szContentMD5);
    printf("total send %s%d%s bytes, file size %s%d%s bytes, md5:%s%s%s\n", 
            _color_red, sendSize, _color_end, 
            _color_red, fileSize, _color_end,
            _color_yellow, szContentMD5, _color_end);

    // 接收数据
    result = 0;
    //uint64_t t1 = Now();
    do {
        if (resp.IsFinished()) break;
        ret = tcp.RecvMsg(szText, cbText, 10);
        if (ret > 0)
        {
            resp.read((void*)szText, ret);
            printf("recv data:[\n%s%s%s\n]\n", _color_green, szText, _color_end);
        }
        else if (ret == 0)
        {
            // printf("pending recv data...\n");
            //if (Now() >= t1 + 5000)
            {
                break;
            }
        }
        else
        {
            result = -11;
            break;
        }
    }while(true);

    printf("last recv result:%s%d%s\n", _color_green, ret, _color_end);

    if (result < 0) return result;
    int32_t code = resp.getStatusCode();
    if (code == 0 || code==200)
    {
        return 0;
    }
    return 1;
}


struct ContentTypeItem
{
    char  szExt[16];
    char  szContentType[64];
};

static ContentTypeItem s_gContentTypeItem[] =
{
    {".*",       "application/octet-stream"},
    {".tif",     "image/tiff"},
    {".001",     "application/x-001"},
    {".301",     "application/x-301"},
    {".323",     "text/h323"},
    {".906",     "application/x-906"},
    {".907",     "drawing/907"},
    {".a11",     "application/x-a11"},
    {".acp",     "audio/x-mei-aac"},
    {".ai",      "application/postscript"},
    {".aif",     "audio/aiff"},
    {".aifc",    "audio/aiff"},
    {".aiff",    "audio/aiff"},
    {".anv",     "application/x-anv"},
    {".asa",     "text/asa"},
    {".asf",     "video/x-ms-asf"},
    {".asp",     "text/asp"},
    {".asx",     "video/x-ms-asf"},
    {".au",      "audio/basic"},
    {".avi",     "video/avi"},
    {".awf",     "application/vnd.adobe.workflow"},
    {".biz",     "text/xml"},
    {".bmp",     "application/x-bmp"},
    {".bot",     "application/x-bot"},
    {".c4t",     "application/x-c4t"},
    {".c90",     "application/x-c90"},
    {".cal",     "application/x-cals"},
    {".cat",     "application/vnd.ms-pki.seccat"},
    {".cdf",     "application/x-netcdf"},
    {".cdr",     "application/x-cdr"},
    {".cel",     "application/x-cel"},
    {".cer",     "application/x-x509-ca-cert"},
    {".cg4",     "application/x-g4"},
    {".cgm",     "application/x-cgm"},
    {".cit",     "application/x-cit"},
    {".class",   "java/*"},
    {".cml",     "text/xml"},
    {".cmp",     "application/x-cmp"},
    {".cmx",     "application/x-cmx"},
    {".cot",     "application/x-cot"},
    {".crl",     "application/pkix-crl"},
    {".crt",     "application/x-x509-ca-cert"},
    {".csi",     "application/x-csi"},
    {".css",     "text/css"},
    {".cut",     "application/x-cut"},
    {".dbf",     "application/x-dbf"},
    {".dbm",     "application/x-dbm"},
    {".dbx",     "application/x-dbx"},
    {".dcd",     "text/xml"},
    {".dcx",     "application/x-dcx"},
    {".der",     "application/x-x509-ca-cert"},
    {".dgn",     "application/x-dgn"},
    {".dib",     "application/x-dib"},
    {".dll",     "application/x-msdownload"},
    {".doc",     "application/msword"},
    {".dot",     "application/msword"},
    {".drw",     "application/x-drw"},
    {".dtd",     "text/xml"},
    {".dwf",     "Model/vnd.dwf"},
    {".dwf",     "application/x-dwf"},
    {".dwg",     "application/x-dwg"},
    {".dxb",     "application/x-dxb"},
    {".dxf",     "application/x-dxf"},
    {".edn",     "application/vnd.adobe.edn"},
    {".emf",     "application/x-emf"},
    {".eml",     "message/rfc822"},
    {".ent",     "text/xml"},
    {".epi",     "application/x-epi"},
    {".eps",     "application/x-ps"},
    {".eps",     "application/postscript"},
    {".etd",     "application/x-ebx"},
    {".exe",     "application/x-msdownload"},
    {".fax",     "image/fax"},
    {".fdf",     "application/vnd.fdf"},
    {".fif",     "application/fractals"},
    {".fo",      "text/xml"},
    {".frm",     "application/x-frm"},
    {".g4",      "application/x-g4"},
    {".gbr",     "application/x-gbr"},
    {".",        "application/x-"},
    {".gif",     "image/gif"},
    {".gl2",     "application/x-gl2"},
    {".gp4",     "application/x-gp4"},
    {".hgl",     "application/x-hgl"},
    {".hmr",     "application/x-hmr"},
    {".hpg",     "application/x-hpgl"},
    {".hpl",     "application/x-hpl"},
    {".hqx",     "application/mac-binhex40"},
    {".hrf",     "application/x-hrf"},
    {".hta",     "application/hta"},
    {".htc",     "text/x-component"},
    {".htm",     "text/html"},
    {".html",    "text/html"},
    {".htt",     "text/webviewhtml"},
    {".htx",     "text/html"},
    {".icb",     "application/x-icb"},
    {".ico",     "image/x-icon"},
    {".ico",     "application/x-ico"},
    {".iff",     "application/x-iff"},
    {".ig4",     "application/x-g4"},
    {".igs",     "application/x-igs"},
    {".iii",     "application/x-iphone"},
    {".img",     "application/x-img"},
    {".ins",     "application/x-internet-signup"},
    {".isp",     "application/x-internet-signup"},
    {".IVF",     "video/x-ivf"},
    {".java",    "java/*"},
    {".jfif",    "image/jpeg"},
    {".jpe",     "image/jpeg"},
    {".jpe",     "application/x-jpe"},
    {".jpeg",    "image/jpeg"},
    {".jpg",     "image/jpeg"},
    {".jpg",     "application/x-jpg"},
    {".js",      "application/x-javascript"},
    {".jsp",     "text/html"},
    {".la1",     "audio/x-liquid-file"},
    {".lar",     "application/x-laplayer-reg"},
    {".latex",   "application/x-latex"},
    {".lavs",    "audio/x-liquid-secure"},
    {".lbm",     "application/x-lbm"},
    {".lmsff",   "audio/x-la-lms"},
    {".ls",      "application/x-javascript"},
    {".ltr",     "application/x-ltr"},
    {".m1v",     "video/x-mpeg"},
    {".m2v",     "video/x-mpeg"},
    {".m3u",     "audio/mpegurl"},
    {".m4e",     "video/mpeg4"},
    {".mac",     "application/x-mac"},
    {".man",     "application/x-troff-man"},
    {".math",    "text/xml"},
    {".mdb",     "application/msaccess"},
    {".mdb",     "application/x-mdb"},
    {".mfp",     "application/x-shockwave-flash"},
    {".mht",     "message/rfc822"},
    {".mhtml",   "message/rfc822"},
    {".mi",      "application/x-mi"},
    {".mid",     "audio/mid"},
    {".midi",    "audio/mid"},
    {".mil",     "application/x-mil"},
    {".mml",     "text/xml"},
    {".mnd",     "audio/x-musicnet-download"},
    {".mns",     "audio/x-musicnet-stream"},
    {".mocha",   "application/x-javascript"},
    {".movie",   "video/x-sgi-movie"},
    {".mp1",     "audio/mp1"},
    {".mp2",     "audio/mp2"},
    {".mp2v",    "video/mpeg"},
    {".mp3",     "audio/mp3"},
    {".mp4",     "video/mpeg4"},
    {".mpa",     "video/x-mpg"},
    {".mpd",     "application/vnd.ms-project"},
    {".mpe",     "video/x-mpeg"},
    {".mpeg",    "video/mpg"},
    {".mpg",     "video/mpg"},
    {".mpga",    "audio/rn-mpeg"},
    {".mpp",     "application/vnd.ms-project"},
    {".mps",     "video/x-mpeg"},
    {".mpt",     "application/vnd.ms-project"},
    {".mpv",     "video/mpg"},
    {".mpv2",    "video/mpeg"},
    {".mpw",     "application/vnd.ms-project"},
    {".mpx",     "application/vnd.ms-project"},
    {".mtx",     "text/xml"},
    {".mxp",     "application/x-mmxp"},
    {".net",     "image/pnetvue"},
    {".nrf",     "application/x-nrf"},
    {".nws",     "message/rfc822"},
    {".odc",     "text/x-ms-odc"},
    {".out",     "application/x-out"},
    {".p10",     "application/pkcs10"},
    {".p12",     "application/x-pkcs12"},
    {".p7b",     "application/x-pkcs7-certificates"},
    {".p7c",     "application/pkcs7-mime"},
    {".p7m",     "application/pkcs7-mime"},
    {".p7r",     "application/x-pkcs7-certreqresp"},
    {".p7s",     "application/pkcs7-signature"},
    {".pc5",     "application/x-pc5"},
    {".pci",     "application/x-pci"},
    {".pcl",     "application/x-pcl"},
    {".pcx",     "application/x-pcx"},
    {".pdf",     "application/pdf"},
    {".pdx",     "application/vnd.adobe.pdx"},
    {".pfx",     "application/x-pkcs12"},
    {".pgl",     "application/x-pgl"},
    {".pic",     "application/x-pic"},
    {".pko",     "application/vnd.ms-pki.pko"},
    {".pl",      "application/x-perl"},
    {".plg",     "text/html"},
    {".pls",     "audio/scpls"},
    {".plt",     "application/x-plt"},
    {".png",     "image/png"},
    {".png",     "application/x-png"},
    {".pot",     "application/vnd.ms-powerpoint"},
    {".ppa",     "application/vnd.ms-powerpoint"},
    {".ppm",     "application/x-ppm"},
    {".pps",     "application/vnd.ms-powerpoint"},
    {".ppt",     "application/vnd.ms-powerpoint"},
    {".ppt",     "application/x-ppt"},
    {".pr",      "application/x-pr"},
    {".prf",     "application/pics-rules"},
    {".prn",     "application/x-prn"},
    {".prt",     "application/x-prt"},
    {".ps",      "application/x-ps"},
    {".ps",      "application/postscript"},
    {".ptn",     "application/x-ptn"},
    {".pwz",     "application/vnd.ms-powerpoint"},
    {".r3t",     "text/vnd.rn-realtext3d"},
    {".ra",      "audio/vnd.rn-realaudio"},
    {".ram",     "audio/x-pn-realaudio"},
    {".ras",     "application/x-ras"},
    {".rat",     "application/rat-file"},
    {".rdf",     "text/xml"},
    {".rec",     "application/vnd.rn-recording"},
    {".red",     "application/x-red"},
    {".rgb",     "application/x-rgb"},
    {".rjs",     "application/vnd.rn-realsystem-rjs"},
    {".rjt",     "application/vnd.rn-realsystem-rjt"},
    {".rlc",     "application/x-rlc"},
    {".rle",     "application/x-rle"},
    {".rm",      "application/vnd.rn-realmedia"},
    {".rmf",     "application/vnd.adobe.rmf"},
    {".rmi",     "audio/mid"},
    {".rmj",     "application/vnd.rn-realsystem-rmj"},
    {".rmm",     "audio/x-pn-realaudio"},
    {".rmp",     "application/vnd.rn-rn_music_package"},
    {".rms",     "application/vnd.rn-realmedia-secure"},
    {".rmvb",    "application/vnd.rn-realmedia-vbr"},
    {".rmx",     "application/vnd.rn-realsystem-rmx"},
    {".rnx",     "application/vnd.rn-realplayer"},
    {".rp",      "image/vnd.rn-realpix"},
    {".rpm",     "audio/x-pn-realaudio-plugin"},
    {".rsml",    "application/vnd.rn-rsml"},
    {".rt",      "text/vnd.rn-realtext"},
    {".rtf",     "application/msword"},
    {".rtf",     "application/x-rtf"},
    {".rv",      "video/vnd.rn-realvideo"},
    {".sam",     "application/x-sam"},
    {".sat",     "application/x-sat"},
    {".sdp",     "application/sdp"},
    {".sdw",     "application/x-sdw"},
    {".sit",     "application/x-stuffit"},
    {".slb",     "application/x-slb"},
    {".sld",     "application/x-sld"},
    {".slk",     "drawing/x-slk"},
    {".smi",     "application/smil"},
    {".smil",    "application/smil"},
    {".smk",     "application/x-smk"},
    {".snd",     "audio/basic"},
    {".sol",     "text/plain"},
    {".sor",     "text/plain"},
    {".spc",     "application/x-pkcs7-certificates"},
    {".spl",     "application/futuresplash"},
    {".spp",     "text/xml"},
    {".ssm",     "application/streamingmedia"},
    {".sst",     "application/vnd.ms-pki.certstore"},
    {".stl",     "application/vnd.ms-pki.stl"},
    {".stm",     "text/html"},
    {".sty",     "application/x-sty"},
    {".svg",     "text/xml"},
    {".swf",     "application/x-shockwave-flash"},
    {".tdf",     "application/x-tdf"},
    {".tg4",     "application/x-tg4"},
    {".tga",     "application/x-tga"},
    {".tif",     "image/tiff"},
    {".tif",     "application/x-tif"},
    {".tiff",    "image/tiff"},
    {".tld",     "text/xml"},
    {".top",     "drawing/x-top"},
    {".torrent", "application/x-bittorrent"},
    {".tsd",     "text/xml"},
    {".txt",     "text/plain"},
    {".uin",     "application/x-icq"},
    {".uls",     "text/iuls"},
    {".vcf",     "text/x-vcard"},
    {".vda",     "application/x-vda"},
    {".vdx",     "application/vnd.visio"},
    {".vml",     "text/xml"},
    {".vpg",     "application/x-vpeg005"},
    {".vsd",     "application/vnd.visio"},
    {".vsd",     "application/x-vsd"},
    {".vss",     "application/vnd.visio"},
    {".vst",     "application/vnd.visio"},
    {".vst",     "application/x-vst"},
    {".vsw",     "application/vnd.visio"},
    {".vsx",     "application/vnd.visio"},
    {".vtx",     "application/vnd.visio"},
    {".vxml",    "text/xml"},
    {".wav",     "audio/wav"},
    {".wax",     "audio/x-ms-wax"},
    {".wb1",     "application/x-wb1"},
    {".wb2",     "application/x-wb2"},
    {".wb3",     "application/x-wb3"},
    {".wbmp",    "image/vnd.wap.wbmp"},
    {".wiz",     "application/msword"},
    {".wk3",     "application/x-wk3"},
    {".wk4",     "application/x-wk4"},
    {".wkq",     "application/x-wkq"},
    {".wks",     "application/x-wks"},
    {".wm",      "video/x-ms-wm"},
    {".wma",     "audio/x-ms-wma"},
    {".wmd",     "application/x-ms-wmd"},
    {".wmf",     "application/x-wmf"},
    {".wml",     "text/vnd.wap.wml"},
    {".wmv",     "video/x-ms-wmv"},
    {".wmx",     "video/x-ms-wmx"},
    {".wmz",     "application/x-ms-wmz"},
    {".wp6",     "application/x-wp6"},
    {".wpd",     "application/x-wpd"},
    {".wpg",     "application/x-wpg"},
    {".wpl",     "application/vnd.ms-wpl"},
    {".wq1",     "application/x-wq1"},
    {".wr1",     "application/x-wr1"},
    {".wri",     "application/x-wri"},
    {".wrk",     "application/x-wrk"},
    {".ws",      "application/x-ws"},
    {".ws2",     "application/x-ws"},
    {".wsc",     "text/scriptlet"},
    {".wsdl",    "text/xml"},
    {".wvx",     "video/x-ms-wvx"},
    {".xdp",     "application/vnd.adobe.xdp"},
    {".xdr",     "text/xml"},
    {".xfd",     "application/vnd.adobe.xfd"},
    {".xfdf",    "application/vnd.adobe.xfdf"},
    {".xhtml",   "text/html"},
    {".xls",     "application/vnd.ms-excel"},
    {".xls",     "application/x-xls"},
    {".xlw",     "application/x-xlw"},
    {".xml",     "text/xml"},
    {".xpl",     "audio/scpls"},
    {".xq",      "text/xml"},
    {".xql",     "text/xml"},
    {".xquery",  "text/xml"},
    {".xsd",     "text/xml"},
    {".xsl",     "text/xml"},
    {".xslt",    "text/xml"},
    {".xwd",     "application/x-xwd"},
    {".x_b",     "application/x-x_b"},
    {".sis",     "application/vnd.symbian.install"},
    {".sisx",    "application/vnd.symbian.install"},
    {".x_t",     "application/x-x_t"},
    {".ipa",     "application/vnd.iphone"},
    {".apk",     "application/vnd.android.package-archive"},
    {".xap",     "application/x-silverlight-app"}
};

bool GetContentType(const CMyString &strFile, CMyString &strContentType)
{
    size_t pos = strFile.find_last_of('.');
    if (pos == std::string::npos)
    {
        return false;
    }
    CMyString strExt = strFile.substr(pos);
    strExt.MakeLower();
    for(int i=0; i<(int32_t)(sizeof(s_gContentTypeItem)/sizeof(s_gContentTypeItem[0])); ++i)
    {
        if (strExt == s_gContentTypeItem[i].szExt)
        {
            strContentType = s_gContentTypeItem[i].szContentType;
            return true;
        }
    }

    // 查找其他非标准的
    return false;
};

