//========================================================================
//
// TiffWriter.cc
//
//========================================================================

#include "TiffWriter.h"

TiffWriter::TiffWriter(Format format)
{
    this->format = format;
}

TiffWriter::~TiffWriter()
{
}

void TiffWriter::setCompressionString(const char* compression)
{
    this->compression = compression;
}
