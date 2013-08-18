#ifndef ECLMPL_CONTACT_INFO_H
#define ECLMPL_CONTACT_INFO_H

#include "eclmplCommonInclude.h"

class eclmplContactInfo {
public:
  eclmplContactInfo() : providedById(0), intendedForId(0) {}
  eclmplContactInfo(const unsigned int &provBy, const int &intFor, const vector<string> contArgs) :
    providedById(provBy), intendedForId(intFor), contactArgs(contArgs) {}
  ~eclmplContactInfo() {};

  inline const unsigned int providedBy() const { return providedById; }
  inline void setProvidedBy(const unsigned int &provBy) { providedById = provBy; }
  inline const unsigned int intendedFor() const { return intendedForId; }
  inline void setIntendedFor(const unsigned int &intFor) { intendedForId = intFor; }
  inline const vector<string> contactArguments() const { return contactArgs; }
  inline void setContactArguments(const vector<string> &contArgs) { contactArgs = contArgs; }
  inline unsigned int nrOfContactArguments() const { return contactArgs.size(); }

  friend std::ostream& operator<< (std::ostream& os, const eclmplContactInfo& info) {
    os << "ContactInfo--Provided by: " << info.providedById
       << ", Intended for: " << info.intendedForId
       << ", Contact args:";
    for (unsigned int i = 0; i < info.contactArgs.size(); i++) {
      os << " " << info.contactArgs[i];
    }
    return os;
  }
private:
  unsigned int providedById;
  unsigned int intendedForId;
  vector<string> contactArgs;
};

#endif
