TEMPLATE = subdirs
SUBDIRS = app qsgepaper

app.subdir = app
app.depends = qsgepaper
qsgepaper.subdir = qsgepaper
