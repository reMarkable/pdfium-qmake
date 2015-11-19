#ifndef QSGEPAPERPLUGIN_H
#define QSGEPAPERPLUGIN_H

#include <QtQuick/private/qsgcontextplugin_p.h>

class Q_DECL_EXPORT QsgEpaperPlugin : public QSGContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSGContextFactoryInterface" FILE "qsgepaper.json")

public:
    QsgEpaperPlugin();

    // QFactoryInterface interface
public:
    QStringList keys() const override;

    // QSGContextFactoryInterface interface
public:
    QSGContext *create(const QString &key) const override;
    QQuickTextureFactory *createTextureFactoryFromImage(const QImage &image) override;
    QSGRenderLoop *createWindowManager() override;
};

#endif // QSGEPAPERPLUGIN_H
