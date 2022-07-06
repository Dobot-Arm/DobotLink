/**
 ***********************************************************************************************************************
 *
 * @author  ZhangRan
 * @date    2021/11/19
 * @version 1.0.0
 *
 * <h2><center>&copy; COPYRIGHT 2020 </center></h2>
 *
 ***********************************************************************************************************************
 */

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include <log/log.h>
#include <QDateTime>

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                                  MACROS
 *----------------------------------------------------------------------------------------------------------------------
 */

#define COLOR_NORMAL "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                                LOCAL VARIABLES
 *----------------------------------------------------------------------------------------------------------------------
 */

LogLevel log_level;
static const char* log_level_str[] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG" };
//static const char* log_color[] = { COLOR_RED, COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_NORMAL, COLOR_NORMAL };

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                                  IMPLEMENT
 *----------------------------------------------------------------------------------------------------------------------
 */

void setLogLevel(LogLevel level)
{
    log_level = level;
}

LogLevel getLogLevel()
{
    return log_level;
}

void logger(LogLevel level, const char* file, uint32_t line, const char* func, const char* format, ...)
{
    if (level > log_level)
        return;

    char log_buf[300];

    const char* file_name = strrchr(file, '\\');
    file_name = file_name != nullptr ? file_name + 1 : file;

    va_list va_arg;
    va_start(va_arg, format);
    vsnprintf(log_buf, sizeof(log_buf), format, va_arg);
    va_end(va_arg);
    log_buf[sizeof(log_buf) - 1] = 0;

    char time_str[50] = "";
    /*
    time_t now = time(nullptr);
    strftime(time_str, sizeof(time_str), "%F %H:%M:%S", localtime(&now));
    char ms_str[15];

    timespec ts{};
    timespec_get(&ts, TIME_UTC);

    sprintf(ms_str, "% 0.3f", (ts.tv_nsec) / 1000000.0);
    strcat(time_str, ms_str);

    QTime time = QTime::currentTime();
    */
    QString strNowTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    strcpy(time_str, strNowTime.toStdString().c_str());
//    printf("%s", log_color[static_cast<int>(level)]);
    printf("%s %-5s - [%-25s : %4d] %s\n", time_str, log_level_str[static_cast<int>(level)], file_name,
           static_cast<int>(line), log_buf);
//    printf("%s\n", COLOR_NORMAL);
}
