TEMPLATE = subdirs
SUBDIRS = app

app.subdir = app

contains(QT_ARCH, arm) {
    SUBDIRS += qsgepaper
    app.depends = qsgepaper
    qsgepaper.subdir = qsgepaper
}
