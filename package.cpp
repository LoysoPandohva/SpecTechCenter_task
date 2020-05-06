#include "package.h"
#include <array>
#include <cstring>
#include <QMessageBox>

Package::Package(): isCorrectMessage(true) {}

Package::Package(const QString &message): isCorrectMessage(true) {
    std::array<QString,4> numbers;
    int idx = 0;
    for(int i = 0; i < message.size(); i++){
        if(message[i] == '\n'){
            idx++;
            i++;
            if(idx > 3 || i >= message.size()){
                isCorrectMessage = false;
                break;
            }
        }
        numbers[idx].push_back(message[i]);
    }

    if(isCorrectMessage){
        mes.x = numbers[0].toFloat(&isCorrectMessage);
    }
    if(isCorrectMessage){
        mes.y = numbers[1].toFloat(&isCorrectMessage);
    }
    if(isCorrectMessage){
        mes.z = numbers[2].toFloat(&isCorrectMessage);
    }
    if(isCorrectMessage){
        mes.w = numbers[3].toFloat(&isCorrectMessage);
    }

    if(!isCorrectMessage) {
        QMessageBox::critical(0,"ERROR", "Entered data is incorrect");
    }
}

QByteArray Package::serializeMessage() {
    static QByteArray package;
    package.clear();

    if(isCorrectMessage){
        char prefix = 0x40;
        package.push_back(prefix);

        addFloat(package, mes.x);
        addFloat(package, mes.y);
        addFloat(package, mes.z);
        addFloat(package, mes.w);
    }

    return package;
}

QByteArray Package::deSerializeMessage(QByteArray &_package) {
    QByteArray package;
    package.swap(_package);

    if(package.size() == 17 && package.front() == (char)0x40){ // 1+4+4+4+4 = 17 bytes in package
        isCorrectMessage = true;
    }
    else {
        isCorrectMessage = false;
        QMessageBox::critical(0,"ERROR", "Received data is incorrect");
    }

    static std::string message;
    message.clear();

    if(isCorrectMessage){
        mes.w = getFloat(package);
        mes.z = getFloat(package);
        mes.y = getFloat(package);
        mes.x = getFloat(package);

        message = std::to_string(mes.x) + '\n' + std::to_string(mes.y)
                + '\n' + std::to_string(mes.z) + '\n' + std::to_string(mes.w);
    }

    return message.data();
}

void Package::addFloat(QByteArray &package, float num) {
    uint8_t num_c[4];
    std::memcpy(num_c, &num, sizeof(num));

    package.push_back(num_c[0]);
    package.push_back(num_c[1]);
    package.push_back(num_c[2]);
    package.push_back(num_c[3]);
}
float Package::getFloat(QByteArray &package) {
    uint8_t num_c[4];

    num_c[3] = package.back();
    package.remove(package.size() - 1, 1);
    num_c[2] = package.back();
    package.remove(package.size() - 1, 1);
    num_c[1] = package.back();
    package.remove(package.size() - 1, 1);
    num_c[0] = package.back();
    package.remove(package.size() - 1, 1);

    float num;
    std::memcpy(&num, num_c, sizeof(num));

    return num;
}
