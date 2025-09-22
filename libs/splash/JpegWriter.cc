//========================================================================
//
// JpegWriter.cc
//
//========================================================================

#include "JpegWriter.h"

JpegWriter::JpegWriter(Format format)
{
    this->format = format;
}

JpegWriter::~JpegWriter()
{
}

void JpegWriter::setProgressive(bool progressive)
{
    this->progressive = progressive;
}

void JpegWriter::setOptimize(bool optimize)
{
    this->optimize = optimize;
}

void JpegWriter::setQuality(int quality)
{
    this->quality = quality;
}
