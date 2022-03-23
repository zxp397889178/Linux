redis中键值的使用

一、充值数据
key: paycenter_order_订单号#data ，其中订单号是用户本次的订单号，以P开头
data: 
    {"code":      0,
     "message":   "充值说明"
     "uin":       "",              # 用户唯一ID
     "gameCode":  "",              # 游服代码
     "orderNo":   "",              # 订单号
     "billNo":    "",              # 游服产生的订单号
     "fee":       0,               # 充值金额，以分计算
     "realFee":   0,               # 实际充值金额，以分计算
     "productId": "",              # 游戏传过来的产品ID，仅作记录
     "status":    0,               # 订单状态
     "payId":     0,               # t_payorder表中的主键ID
     "channelId": 0,               # t_sdkchannel表中的主键ID
     "billpoint": 0,               # t_billpoing表中的主键ID
     "method":    "",              # 支付方式
    }

二、订单相关数据
key: paycenter_order_订单号#id
value: 订单ID

key: paycenter_order_订单号#channelId
value: 订单channelId



返回给前端的数据
{
    "code":0,
    "message":"",
    "data":
    {
        "traceNo": "",      # 支付中心订单号
        "url": "",          # 返回给前端的URL或者数据串
        "sdk": "",          # 前端调用的SDK，用于指示是否要用Native调用
        "channel":"",       # 支付渠道
        "method":""         # 支付方式
    }
}
