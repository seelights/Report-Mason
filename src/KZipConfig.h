/*
 * @Author: seelights
 * @Date: 2025-09-14 21:30:00
 * @LastEditTime: 2025-09-14 21:30:00
 * @LastEditors: seelights
 * @Description: KZip配置文件
 * @FilePath: \ReportMason\include\KZipConfig.h
 * Copyright (c) 2025 by seelights@git.cn, All Rights Reserved.
 */

#pragma once

// KArchive版本定义
#define KARCHIVE_VERSION_MAJOR 5
#define KARCHIVE_VERSION_MINOR 0
#define KARCHIVE_VERSION_PATCH 0

// 启用ZIP支持
#define KARCHIVE_ENABLE_ZIP 1

// 启用压缩支持
#define KARCHIVE_ENABLE_COMPRESSION 1

// 文档格式支持
#define KARCHIVE_ENABLE_DOCX 1
#define KARCHIVE_ENABLE_PPTX 1

// 调试模式
#ifdef QT_DEBUG
    #define KARCHIVE_DEBUG 1
#else
    #define KARCHIVE_DEBUG 0
#endif

