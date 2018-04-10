//
// Created by Rasmus Munk on 11/11/2017.
//

#ifndef DBUS_TEST_MESSAGE_H
#define DBUS_TEST_MESSAGE_H

#include <aseba/compiler/compiler.h>
#include <aseba/common/msg/msg.h>
#include <QMetaType>

namespace ENU
{
    class Message
    {
    public:
        Message() : _namedValue(Aseba::NamedValue(std::wstring{1}, 1)), _dataVector(Aseba::UserMessage::DataVector{}) {}
        Message(Aseba::NamedValue namedValue, Aseba::UserMessage::DataVector dataVector) : _namedValue(std::move(namedValue)),
                                                                                           _dataVector(std::move(dataVector)) {}
        //Message(const Message &message) : _namedValue(message.namedValue()), _dataVector(message.dataVector()) {}


        Aseba::NamedValue namedValue() const;
        Aseba::UserMessage::DataVector dataVector() const;

    private:
        Aseba::NamedValue _namedValue;
        Aseba::UserMessage::DataVector _dataVector;

    };
}
Q_DECLARE_METATYPE(ENU::Message);


#endif //DBUS_TEST_MESSAGE_H
