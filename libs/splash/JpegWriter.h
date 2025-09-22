//========================================================================
//
// JpegWriter.h
//
//========================================================================

#ifndef JPEGWRITER_H
#define JPEGWRITER_H

class JpegWriter
{
public:
    enum Format
    {
        RGB,
        GRAY
    };

    JpegWriter(Format format);
    ~JpegWriter();

    void setProgressive(bool progressive);
    void setOptimize(bool optimize);
    void setQuality(int quality);

private:
    Format format;
    bool progressive;
    bool optimize;
    int quality;
};

#endif
