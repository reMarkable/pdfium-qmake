#include "qsgepaperplugin.h"
#include "epcontext.h"
#include "eptexture.h"
#include "eprenderloop.h"

QsgEpaperPlugin::QsgEpaperPlugin()
{
}


QStringList QsgEpaperPlugin::keys() const
{
    return QStringList() << "epaper";
}

QSGContext *QsgEpaperPlugin::create(const QString &key) const
{
    if (key != "epaper") {
        return nullptr;
    }

    return new EPContext;
}

QQuickTextureFactory *QsgEpaperPlugin::createTextureFactoryFromImage(const QImage &image)
{
    return new EPTextureFactory(image);
}

QSGRenderLoop *QsgEpaperPlugin::createWindowManager()
{
    return new EPRenderLoop;
}
