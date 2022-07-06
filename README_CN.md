English version of the README -> please [click here](./README.md)

1. DobotLink是硬件设备与上位机（DobotLab或者其他开发环境）通信的中间服务层，所有外界软件都通过该服务层控制Dobot的硬件设备，包含magician、magician lite、magician Go等。DobotLink不仅包含API动态库，还支持设备的固件更新、设备校验等功能。

2. DobotLink使用Qt5开发，使用了Qt的相关模块如下：
   `core` `serialport` `network` `websockets` `gui` `widgets` `multimedia` `multimediawidgets` `concurrent`

3. DobotLink使用了第三方开源库[libsmb2](https://github.com/sahlberg/libsmb2)，并增加一个接口`int smb2_isconnect_share(struct smb2_context *smb2)`，具体可以参考源码。

4. DobotLink的源码遵循LGPL协议