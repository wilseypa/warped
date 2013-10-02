#ifndef EVENT_ID_H
#define EVENT_ID_H


#include <iostream>

class EventId {
    friend class Event;
    friend class DefaultEvent;
    friend class NegativeEvent;
    friend class NegativeEventMessage;
    friend class StragglerEvent;

    friend std::ostream& operator<<(std::ostream& os, const EventId& toPrint);

public:
    /**
       Constructor used in constructing vectors of EventIds.
    */
    EventId(const EventId& initFrom) : val(initFrom.val) {}
    virtual ~EventId() {}

    /**
       This is also required for containers of EventIds.
    */
    EventId& operator=(const EventId& initFrom) { val = initFrom.val; return *this; }

    /**
       This is really the only function of EventId that should normally be
       used by applications.
    */
    bool operator==(const EventId& compareTo) const {
        return val == compareTo.val;
    }
    bool operator!=(const EventId& compareTo) const {
        return val != compareTo.val;
    }
    bool operator< (const EventId& compareTo) const {
        return val < compareTo.val;
    }
    bool operator> (const EventId& compareTo) const {
        return val > compareTo.val;
    }
    bool operator<= (const EventId& compareTo) const {
        return val <= compareTo.val;
    }
    bool operator>= (const EventId& compareTo) const {
        return val >= compareTo.val;
    }

    unsigned int getEventNum() {
        return val;
    }

private:
    EventId(const unsigned int initVal) : val(initVal) {}

    unsigned int val;
};

inline std::ostream&
operator<<(std::ostream& os, const EventId& toPrint) {
    os << toPrint.val;
    return os;
}


#endif
