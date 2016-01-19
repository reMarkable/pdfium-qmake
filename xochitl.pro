TEMPLATE = subdirs
SUBDIRS = app pdfium

app.subdir = app
app.depends = pdfium

pdfium.subdir = pdfium

contains(QT_ARCH, arm) {
    SUBDIRS += qsgepaper
    app.depends += qsgepaper
    qsgepaper.subdir = qsgepaper
}
