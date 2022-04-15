# Homekit-Outlet

原生接入HomeKit

based on Arduino

This accessory contains tow builtin-leds, a button and a relay on ESP M4（esp8285）

Setup code: 根据MAC地址生成，串口打印处调试

连接引脚：
The Red Led is linked to GPIO 16
The Blue Led is linked to GPIO 12
The Button is linked to GPIO 13
The Relay is linked to GPIO 14


使用方法：
将固件烧入esp，上电，红灯闪烁进入配网模式，连接插座热点，按照提示完成配网。
串口会打印处连接代码，在家庭中添加即可。

点按按键会打开或关闭插座
长按按键恢复出厂设置，重新配网，连接。
