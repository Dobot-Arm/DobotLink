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

#pragma once

#include <cstdint>

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                                   PROTOTYPE
 *----------------------------------------------------------------------------------------------------------------------
 */

/**
 * 日志输出级别
 */
enum class LogLevel
{
    LOG_FATAL = 0, /**< 严重错误信息 */
    LOG_ERROR,     /**< 错误信息 */
    LOG_WARN,      /**< 警告信息 */
    LOG_INFO,      /**< 一般信息 */
    LOG_DEBUG      /**< 调试信息 */
};

/**
 * 日志回调函数
 */
typedef void (*LogHandler)(LogLevel level, const char* file, uint32_t line, const char* func, const char* msg);

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                                     PROTOTYPE
 *----------------------------------------------------------------------------------------------------------------------
 */

/**
 * 设置日志级别
 */
void setLogLevel(LogLevel level);

/**
 * 获取日志级别
 */
LogLevel getLogLevel();

/**
 * 日志输出
 */
void logger(LogLevel level, const char* file, uint32_t line, const char* func, const char* format_str, ...);

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                                     MACRO
 *----------------------------------------------------------------------------------------------------------------------
 */

/**
 * 记录调试信息
 * @param format 与 printf 函数的用法类似
 */
#define DOBOT_DEBUG(format, ...) logger(LogLevel::LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define DOBOT_INFO(format, ...) logger(LogLevel::LOG_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define DOBOT_WARN(format, ...) logger(LogLevel::LOG_WARN, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define DOBOT_ERR(format, ...) logger(LogLevel::LOG_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define DOBOT_FATAL(format, ...) logger(LogLevel::LOG_FATAL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
