//========================================================================
//
// TiffWriter.h
//
//========================================================================

#ifndef TIFFWRITER_H
#define TIFFWRITER_H

class TiffWriter
{
public:
    enum Format
    {
        RGB,
        RGBA,
        GRAY,
        CMYK,
        LAB
    };

    TiffWriter(Format format);
    ~TiffWriter();

    void setCompressionString(const char* compression);

private:
    Format format;
    const char* compression;
};

#endif
