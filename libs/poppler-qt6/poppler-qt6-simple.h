/*
 * poppler-qt6-simple.h
 * Simplified Poppler Qt6 Header for ReportMason
 */

#ifndef POPPLER_QT6_SIMPLE_H
#define POPPLER_QT6_SIMPLE_H

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QIODevice>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <memory>
#include <vector>
#include <functional>

// Poppler namespace
namespace Poppler {
    class Document;
    class Page;
    class Annotation;
    class Link;
    class FormField;
    class FontInfo;
    class EmbeddedFile;
    class Movie;
    class Sound;
    class OutlineItem;
    class PageTransition;
    class OptContentModel;
}

// Forward declarations
using PopplerDocument = Poppler::Document;
using PopplerPage = Poppler::Page;
using PopplerAnnotation = Poppler::Annotation;
using PopplerLink = Poppler::Link;
using PopplerFormField = Poppler::FormField;
using PopplerFontInfo = Poppler::FontInfo;
using PopplerEmbeddedFile = Poppler::EmbeddedFile;
using PopplerMovie = Poppler::Movie;
using PopplerSound = Poppler::Sound;
using PopplerOutlineItem = Poppler::OutlineItem;
using PopplerPageTransition = Poppler::PageTransition;
using PopplerOptContentModel = Poppler::OptContentModel;

// Main Poppler Qt6 classes
namespace Poppler {

class Document : public QObject
{
public:
    Document();
    ~Document();
    
    enum DocumentType {
        PDF
    };

    enum Permission {
        AllowPrint = 0x0001,
        AllowModify = 0x0002,
        AllowCopy = 0x0004,
        AllowAddNotes = 0x0008,
        AllowFillForms = 0x0010,
        AllowAccessibility = 0x0020,
        AllowAssemble = 0x0040,
        AllowPrintHighRes = 0x0080
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

    static std::unique_ptr<Document> load(const QString &filePath);
    static std::unique_ptr<Document> load(QIODevice *device);
    
    int numPages() const;
    Page *page(int index) const;
    
    QString title() const;
    QString author() const;
    QString subject() const;
    QString keywords() const;
    QString creator() const;
    QString producer() const;
    QDateTime creationDate() const;
    QDateTime modDate() const;
    
    bool isLocked() const;
    bool unlock(const QString &password);
    
    Permissions permissions() const;
    
    DocumentType docType() const;
    
    QList<Page*> pages() const;
    
private:
    class Private;
    std::unique_ptr<Private> d;
};

class Page : public QObject
{
public:
    Page();
    ~Page();
    
    QSize pageSize() const;
    QSize pageSizeF() const;
    
    QImage renderToImage(double xRes = 72.0, double yRes = 72.0, 
                        int x = -1, int y = -1, int w = -1, int h = -1) const;
    QImage renderToImage(int x, int y, int w, int h, double xRes = 72.0, double yRes = 72.0) const;
    
    QString text(const QRect &rect = QRect()) const;
    enum SearchMode {
        CaseSensitive,
        CaseInsensitive
    };
    
    QList<QRectF> search(const QString &text, SearchMode caseSensitive = CaseSensitive) const;
    
    QList<Link*> links() const;
    QList<Annotation*> annotations() const;
    
    double duration() const;
    int label() const;
    
    QRectF boundingBox() const;
    QRectF boundingBox(const QString &text) const;
    
private:
    class Private;
    std::unique_ptr<Private> d;
    friend class Document;
};

// Simple implementation classes
class Document::Private
{
public:
    QString m_filePath;
    bool m_isLocked;
    int m_numPages;
    // Add other necessary members
};

class Page::Private
{
public:
    int m_pageNumber;
    QSize m_pageSize;
    // Add other necessary members
};

} // namespace Poppler

#endif // POPPLER_QT6_SIMPLE_H
