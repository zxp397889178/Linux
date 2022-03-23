一、在config.xml 中增加配置
    <google sdk="GOOGLE" desc="谷歌支付">
        <notify uri="api/chaofan/callback" validIp="0.0.0.0" />
        <certified script="./test1.py" function="certify" keyfile="./py/client_secret.json"/>
        <package name="com.firegame.rummy" />
    </google>

二、安装Python环境
    wget https://www.python.org/ftp/python/3.9.2/Python-3.9.2.tgz
    yum -y install openssl-devel
    编辑 Module/Setup 文件，指定对应的ssl的 include 和 lib 路径
    ./configure --prefix=/opt/Python3
    编辑Makefile文件,增加 -fPIC 选项
    make && make install


三、安装google pay环境
    ## https://github.com/googleapis/google-api-python-client

    pip install --upgrade pip
    /opt/Python3/bin/python3.9 -m pip install --upgrade pip
    pip3 install virtualenv
    virtualenv <your-env>
    source <your-env>/bin/activate
    <your-env>/bin/pip install google-api-python-client

四、通讯协议
    1、充值请求，与其他协议一致

    2、充值到账确认

    URI:api/google/callback
    REQ: json
    {
    "gameCode":"",
    "uin":"",
    "fee":""
    "orderNo":"P12343332",
    "productId":"G-123-456-789",
    "token":"1232132321312",
    "sign":"12321312321"
    }

五、google的返回协议说明
    ## https://developers.google.com/android-publisher/api-ref/rest/v3/purchases.products?hl=zh-cn
    purchaseState: 0-订购,1-取消,2-等待
    consumptionState: 0-待消耗，1-已消耗
    orderId: 谷歌的订单ID
    acknowledgementState：0-待确认,1-已确认





