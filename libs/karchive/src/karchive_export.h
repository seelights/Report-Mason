/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2000-2008 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARCHIVE_EXPORT_H
#define KARCHIVE_EXPORT_H

// Simple export macro for KArchive
#ifndef KARCHIVE_EXPORT
# define KARCHIVE_EXPORT
#endif

// Version macros
#define KARCHIVE_VERSION_MAJOR 6
#define KARCHIVE_VERSION_MINOR 0
#define KARCHIVE_VERSION_PATCH 0

// Deprecation macros
#define KARCHIVE_ENABLE_DEPRECATED_SINCE(major, minor) 1
#define KARCHIVE_DISABLE_DEPRECATED_BEFORE_AND_AT(major, minor) 0
#define KARCHIVE_BUILD_DEPRECATED_SINCE(major, minor) 1

// Deprecated macro
#define KARCHIVE_DEPRECATED_VERSION(major, minor, text) Q_DECL_DEPRECATED

#endif
