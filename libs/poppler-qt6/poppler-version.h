/*
 * poppler-version.h
 * Poppler Version Header
 */

#ifndef POPPLER_VERSION_H
#define POPPLER_VERSION_H

#include <QString>

#define POPPLER_VERSION "24.08.0"
#define POPPLER_VERSION_MAJOR 24
#define POPPLER_VERSION_MINOR 8
#define POPPLER_VERSION_MICRO 0

namespace Poppler {
    class Version {
    public:
        static QString string();
        static unsigned int major();
        static unsigned int minor();
        static unsigned int micro();
    };
}

#endif // POPPLER_VERSION_H
