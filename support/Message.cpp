//
// Created by Rasmus Munk on 11/11/2017.
//

#include "Message.h"

namespace ENU {

    Aseba::NamedValue Message::namedValue() const {
        return _namedValue;
    }


    Aseba::UserMessage::DataVector Message::dataVector() const {
        return _dataVector;
    }
}