#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>

class Settings : public QObject
{
    Q_OBJECT
public:
    enum Key {
        Rotation
    };
    Q_ENUM(Key)

    explicit Settings(QObject *parent = 0);

    Q_INVOKABLE void setValue(Key key, QVariant value);
    Q_INVOKABLE QVariant getValue(Key key, QVariant defaultValue);

    Q_INVOKABLE static int thumbnailWidth();
    Q_INVOKABLE static int thumbnailHeight();

signals:

public slots:
};

#endif // SETTINGS_H
