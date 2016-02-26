#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QVector>

class PDFWorker;

class Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentPage WRITE setCurrentPage READ currentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    Q_PROPERTY(QString pageTemplate READ currentTemplate WRITE setCurrentTemplate NOTIFY templateChanged)
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)

public:
    explicit Document(QString path, QObject *parent = 0);
    virtual ~Document();

    QString getThumbnailPath(int index) const {
        return m_path + '-' + QString::number(index) + ".thumbnail.jpg";
    }

    void addOpenCount();
    int openCount() { return m_openCount; }

    static bool createDocument(QString defaultTemplate, QString path);

public slots:
    void setCurrentPage(int newPage);

    int pageCount() { return m_pageCount; }

    int currentPage() const { return m_currentPage; }
    QString path() const { return m_path; }

    QString getStoredPagePath(int index) const {
        return m_path + '-' + QString::number(index) + ".cached.png";
    }

    void deletePages(QList<int> pagesToRemove);
    void addPage();

    QString currentTemplate();
    void setCurrentTemplate(QString newTemplate);

    QStringList availableTemplates() { return QStringList() << "Sketch" << "Lined" << "Squared"; }

    QString getThumbnail(int index);
    QString title();

signals:
    void currentPageChanged(int newPage);
    void pageCountChanged();
    void templateChanged();
    void openCountChanged();

    void thumbnailUpdated(int page);
    void missingThumbnailRequested(int page);

    void pagesDeleted(QList<int> pages);

private slots:
    friend class PDFWorker;
    void setPageCount(int pageCount);

private:
    QString m_path;
    int m_currentPage;
    int m_pageCount;
    int m_openCount;
    QVector<QString> m_templates;
};

#endif // DOCUMENT_H
