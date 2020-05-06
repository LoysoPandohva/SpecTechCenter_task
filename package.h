#ifndef PACKAGE_H
#define PACKAGE_H
#include <QString>
#include <QByteArray>

class Package {
private:
    struct Message{
        float x;
        float y;
        float z;
        float w;
    };

    bool isCorrectMessage;
    Message mes;

    void addFloat(QByteArray &package, float num);
    float getFloat(QByteArray &package);

public:
    Package();
    Package(const QString &packageText);

    QByteArray serializeMessage();
    QByteArray deSerializeMessage(QByteArray &package);

    void setIsCorrectMessage(bool value);
    bool getIsCorrectMessage();
};

#endif // PACKAGE_H
