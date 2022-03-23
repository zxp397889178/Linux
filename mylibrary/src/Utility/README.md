2021-08-30 增加告警的记录日志
*config.xml中增加以下配置
<main>
    <service>
        <alter appName="paycenter" appKey="1232ABedfa131$2s" url="http://127.0.0.1:8090/bot/SendMsg" ids="123456,11223" />
    </service>
</main>

*调用方法：
    1. CAlertLog::Instance().Init(CONFIG.getXml());
    2. 替换函数规则：
       LogWarnAlt   => LogWarn
       LogErrAlt    => LogErr
       LogFatalAlt  => LogErr

------------------------------------------------------------------------------

