#ifndef CONFIG_H
#define CONFIG_H

const int QT_EVENT_PROCESS_TIME = 5; // 每次处理QT事件的时间
const int QT_EVENT_PROCESS_INTERVAL = 100; // 每次处理QT事件的间隔
const int WEB_SERIALPORT_READ_INTERVAL = 20; // 调用WEB端串口读方法的间隔
const uint8_t EMSERIALPORT_ERROR_BUFFER_SIZE = 100; // 异常buffer大小
const uint8_t EMSERIALPORT_SEARCH_BUFFER_SIZE = 20; // 搜索buffer大小
const int64_t EMSERIALPORT_READ_RAW_BUFFER_ZISE = 4096; // 读buffer大小

#endif // CONFIG_H