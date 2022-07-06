Chinese version of the README -> please [click here](./README_CN.md)

1. DobotLink is an intermediate service layer that communicates between hardware devices and the host computer (DobotLab or other development environments). All external software controls Dobot's hardware devices through this service layer, including magician, magician lite, and magician Go. DobotLink not only includes API dynamic library, but also supports device firmware update, device verification and other functions.

2. DobotLink is developed using Qt5, and the related modules using Qt are as follows:
   `core` `serialport` `network` `websockets` `gui` `widgets` `multimedia` `multimediawidgets` `concurrent`

3. DobotLink uses third-party open source libraries[libsmb2](https://github.com/sahlberg/libsmb2), and add an interface `int smb2_isconnect_share(struct smb2_context *smb2)`. For details, please refer to the source code.

4. The source code of DobotLink follows the LGPL agreement