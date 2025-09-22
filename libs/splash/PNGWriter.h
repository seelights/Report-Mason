//========================================================================
//
// PNGWriter.h
//
//========================================================================

#ifndef PNGWRITER_H
#define PNGWRITER_H

class PNGWriter
{
public:
    enum Format
    {
        RGB,
        RGBA,
        GRAY
    };

    PNGWriter(Format format);
    ~PNGWriter();

private:
    Format format;
};

#endif
