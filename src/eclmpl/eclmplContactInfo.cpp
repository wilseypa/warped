#include "eclmplContactInfo.h"

eclmplContactInfo::eclmplContactInfo() : providedById(0), intendedForId(0) {
} // End of Default Constructor

eclmplContactInfo::eclmplContactInfo(const unsigned int& provBy,
                                     const int& intFor,
                                     const std::vector<std::string> contArgs) :
                                     providedById(provBy),
                                     intendedForId(intFor),
                                     contactArgs(contArgs) {
} // End of Initialization Constructor

eclmplContactInfo::~eclmplContactInfo() {
} // End of Destructor

unsigned int eclmplContactInfo::providedBy() const {
    return providedById;
} // End of providedBy().

void eclmplContactInfo::setProvidedBy(const unsigned int& provBy) {
    providedById = provBy;
} // End of setProvidedBy(...).

unsigned int eclmplContactInfo::getIntendedFor() const {
    return intendedForId;
} // End of getIntendedFor().

void eclmplContactInfo::setIntendedFor(const unsigned int& intFor) {
    intendedForId = intFor;
} // End of setIntendedFor(...).

const std::vector<std::string> eclmplContactInfo::contactArguments() const {
    return contactArgs;
} // End of contactArguments().

void eclmplContactInfo::setContactArguments(const std::vector<std::string>& contArgs) {
    contactArgs = contArgs;
} // End of setContactArguments(...).

unsigned int eclmplContactInfo::nrOfContactArguments() const {
    return contactArgs.size();
} // End of nrOfContactArguments().

std::ostream& operator<< (std::ostream& os, const eclmplContactInfo& info) {
    os << "ContactInfo--Provided by: " << info.providedById
       << ", Intended for: " << info.intendedForId
       << ", Contact args:";
    for (unsigned int i = 0; i < info.contactArgs.size(); i++) {
        os << " " << info.contactArgs[i];
    }
    return os;
}
