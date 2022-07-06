#include "DError.h"

QString DError::getErrorMessage(int code)
{
#if 0
    if (code == ERROR_PARSE) {
        return "语法解析错误,服务端接收到无效的json。该错误发送于服务器尝试解析json文本";
    } else if (code == ERROR_INVALID_REQUEST) {
        return "找不到方法,该方法不存在或无效";
    } else if (code == ERROR_METHOD_NOT_FOUND) {
        return "无效的参数,无效的方法参数";
    } else if (code == Invalid_Params) {
        return "内部错误,JSON-RPC内部错误";
    } else if (code == ERROR_INTERNAL) {
        return "服务端错误,预留用于自定义的服务器错误";
    }
#endif

    if (code == ERROR_INVALID_PORTNAME) {
        return "no portname, please specify an available portname.";
    }
    if (code == ERROR_INVALID_DEVICE) {
        return "invalid device, please check your port.";
    }
    if (code == ERROR_INVALID_COMMAND) {
        return "invalid command api, for more information in help document.";
    }
    if (code == ERROR_INVALID_PARAMS) {
        return "invalid params, for more information in help document.";
    }

    //![SerialPortError]
    if (code == ERROR_DEVICE_NOT_FOUND || code == ERROR_SERIALPORT_DEVICE_NOT_FOUND) {
        return "An error occurred while attempting to open an non-existing device.";
    } else if (code == ERROR_SERIALPORT_PERMISSION) {
        return "An error occurred while attempting to open an already opened device by another process " \
               "or a user not having enough permission and credentials to open.";
    } else if (code == ERROR_SERIALPORT_OPEN) {
        return "An error occurred while attempting to open an already opened device in this object.";
    } else if (code == ERROR_SERIALPORT_PARITY) {
        return "Parity error detected by the hardware while reading data. This value is obsolete. " \
               "We strongly advise against using it in new code.";
    } else if (code == ERROR_SERIALPORT_FRAMING) {
        return "Framing error detected by the hardware while reading data. This value is obsolete. " \
               "We strongly advise against using it in new code.";
    } else if (code == ERROR_SERIALPORT_BREAK_CONDITION) {
        return "Break condition detected by the hardware on the input line. This value is obsolete. " \
               "We strongly advise against using it in new code.";
    } else if (code == ERROR_SERIALPORT_WRITE) {
        return "An I/O error occurred while writing the data.";
    } else if (code == ERROR_SERIALPORT_READ) {
        return "An I/O error occurred while reading the data.";
    } else if (code == ERROR_SERIALPORT_RESOURCE) {
        return "An I/O error occurred when a resource becomes unavailable, e.g. " \
               "when the device is unexpectedly removed from the system.";
    } else if (code == ERROR_SERIALPORT_UNSUPPORTED_OPERATION) {
        return "The requested device operation is not supported or prohibited by the running operating system.";
    } else if (code == ERROR_SERIALPORT_UNKNOW) {
        return "An unidentified error occurred.";
    } else if (code == ERROR_SERIALPORT_TIMEOUT) {
        return "A timeout error occurred.";
    } else if (code == ERROR_SERIALPORT_NOT_OPENE) {
        return "his error occurs when an operation is executed that can only be successfully performed if the device is open.";
    } else if (code == ERROR_INDUSTRY_DECODE_BASE64) {
        return "DecodeBase64 File error.";
    } else if (code == ERROR_INDUSTRY_JSONOBJECT) {
        return "JsonObject is empty or invalid format";
    } else if (code == ERROR_INDUSTRY_MISSKEY) {
        return "Miss the params keyword";
    } else if (code == ERROR_INDUSTRY_API) {
        return "Don't find this function, please check your spelling";
    } else if (code == ERROR_INDUSTRY_NOSAMBA) {
        return "Samba is close, please open samba serve";
    }

    if (code == ERROR_UDP_BIND_FAILED) {
        return "binding failed";
    } else if (code == ERROR_INVALIED_FIRMWARE) {
        return "invalied firmware";
    }

    if (code == ERROR_INDUSTRY_FILE_NOT_EXIST) {
        return "File not exist";
    } else if (code == ERROR_INDUSTRY_FILE_CAN_NOT_OPEN) {
        return "Can not open file";
    } else if (code == ERROR_INDUSTRY_FILE_CAN_NOT_WRITE) {
        return "Can not write file";
    } else if (code == ERROR_INDUSTRY_FILE_CAN_NOT_COPY) {
        return "Can not copy file";
    } else if (code == ERROR_INDUSTRY_FILE_HAD_OPENED) {
        return "File had opened";
    } else if (code == ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE) {
        return "Can not create the folder";
    } else if (code == ERROR_INDUSTRY_FOLDER_ALREADY_EXIST) {
        return "Folder already exist";
    } else if (code == ERROR_INDUSTRY_FOLDER_NOT_EXIST) {
        return "Folder not exist";
    } else if (code == ERROR_INDUSTRY_FOLDER_HAD_OPENED) {
        return "Can not open folder";
    } else if (code == ERROR_INDUSTRY_FOLDER_CAN_NOT_RENAME) {
        return "Can not rename folder";
    } else if (code == ERROR_INDUSTRY_FOLDER_CAN_NOT_ENTER) {
        return "Can not enter folder";
    } else if (code == ERROR_INDUSTRY_FOLDER_CAN_NOT_REMOVE) {
        return "Can not remove folder";
    } else if (code == ERROR_INDUSTRY_FILE_TIMEOUT) {
        return "File or Folder operation timeout";
    }

    if (code == ERROR_INDUSTRY_SAMBA_FAILED) {
        return "Fail to start up samba serve.";
    } else if (code == ERROR_INDUSTRY_FIREWALL_FAILED) {
        return "Fail to modify firewall rule.";
    } else if (code == ERROR_INDUSTRY_NOT_CONNECTED) {
        return "Device has not connected.";
    } else if (code == ERROR_INDUSTRY_HAS_CONNECTED) {
        return "Device has connected.";
    } else if (code == ERROR_INDUSTRY_CONNECT_TIMEOUT) {
        return "Device connect timeout.";
    }

    if (code == ERROR_INVALID_REQUEST) {
        return "invalid request.";
    } else if (code == ERROR_INVALID_PLUGIN) {
        return "invalid plugin.";
    } else if (code == ERROR_INVALID_METHOD) {
        return "invalid method.";
    } else if (code == ERROR_DL_API_BUSY) {
        return "Dobotlink api is busy.";
    }

    if (code == ERROR_DEVICE_LOST_CONNECTION) {
        return "device lost its connection.";
    } else if (code == ERROR_DEVICE_DOWNLOAD_FAILD) {
        return "script download failed.";
    } else if (code == ERROR_DEVICE_DISCONNECT_FALID) {
        return "device is not connected.";
    } else if (code == ERROR_COMMUNICATION_TIMEOUT) {
        return "communication timeout.";
    } else if (code == ERROR_COMMUNICATION_BUFFER_FULL) {
        return "command buffer full.";
    }

    if (code == ERROR_DEVICE_ACTION_TIMEOUT) {
        return "action timeout.";
    } else if (code == ERROR_DEVICE_ACTION_CANCELED) {
        return "action canceled.";
    }

    if (code == ERROR_DEBUGGERLITE_RUNNING) {
        return "debugger is running, please stop it first.";
    } else if (code == ERROR_DEBUGGERLITE_START_FAILED) {
        return "debugger start failed.";
    } else if (code == ERROR_DEBUGGERLITE_PROCESS_INVALID) {
        return "invalid debugger process.";
    } else if (code == ERROR_DEBUGGERLITE_INVALID_DPID) {
        return "invalid debugger dpid.";
    } else if (code == ERROR_DEBUGGERLITE_WRITE_FAILD) {
        return "write to debugger process faild.";
    } else if (code == ERROR_DEBUGGERLITE_SETUP_MODULE_FAILED){
        return "Setup module failed.";
    } else if (code == ERROR_DEBUGGERLITE_INVALID_PARAMS){
        return "Invalid params.";
    } else if (code == ERROR_DEBUGGERLITE_PREPARE_FAILED){
        return "Prepare failed.";
    } else if (code == ERROR_DEBUGGERLITE_NO_PERMISSION){
        return "No permission.";
    } else if (code == ERROR_DEBUGGERLITE_FILE_CANNOT_OPEN){
        return "File can not open.";
    } else if (code == ERROR_DEBUGGERLITE_FILE_CANNOT_WRITE){
        return "File can not write.";
    }

    if (code == ERROR_MOBDEBUG_START_FAILED) {
        return "modebug process start failed.";
    } else if (code == ERROR_MOBDEBUG_CLIENT_TIMEOUT) {
        return "modebug waitting client timeout.";
    } else if (code == ERROR_MOBDEBUG_TCP_BIND_FAILED) {
        return "mobdeug tcp bind failed.";
    } else if (code == ERROR_MOBDEBUG_UDP_BIND_FAILED) {
        return "mobdeug udp bind failed.";
    } else if (code == ERROR_MOBDEBUG_PROCESS_CRASHED) {
        return "mobdeug process crashed.";
    } else if (code == ERROR_MOBDEBUG_STOP_FAILED) {
        return "mobdeug process stop timeout";
    } else if (code == ERROR_MOBDEBUG_PROCESS_NOT_RUNNING) {
        return "mobdeug process is not running";
    }

    if (code == ERROR_PROTOCOLFRAME_NOCONNECT){
        return "No connection !!! please connect serialport first !!!";
    } else if (code == ERROR_PROTOCOLFRAME_NOGENERAL) {
        return "No this GO general api !!!";
    } else if (code == ERROR_PROTOCOLFRAME_NOGO) {
        return "no this MagicianGo api !!!";
    } else if (code == ERROR_PROTOCOLFRAME_NOCAMERA) {
        return "no this MagicianGo Camera api !!!";
    } else if (code == ERROR_PROTOCOLFRAME_NOBOX) {
        return "no this MagicBox api !!!";
    } else if (code == ERROR_PROTOCOLFRAME_TIMEOUT) {
        return "the cmd receive timeout ! maybe  connection interrupted !";
    }

    if (code > ERROR_HTTP_CURL && code < ERROR_HTTP_QT) {
        return QString("http curl error(%1).").arg(code - ERROR_HTTP_CURL);
    }

    if (code == ERROR_HTTP_QT_TIMEOUT) {
        return "http communicate timeout!";
    } else if (code > ERROR_HTTP_QT && code < ERROR_HTTP_QT + 1000) {
        return QString("http qt error(%1).").arg(code - ERROR_HTTP_QT);
    }


    return "NUKNOWN ERROR";
}
