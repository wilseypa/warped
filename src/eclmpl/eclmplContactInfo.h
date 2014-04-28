#ifndef ECLMPL_CONTACT_INFO_H
#define ECLMPL_CONTACT_INFO_H

#include "eclmplCommonInclude.h"

class eclmplContactInfo {
public:
    /**@name Public Class Methods of eclmplContactInfo. */
    //@{
    
    /// Default Constructor
    eclmplContactInfo();
    
    /// Initialization Constructor
    eclmplContactInfo(const unsigned int& provBy, const int& intFor, const std::vector<std::string> contArgs);
    
    /// Destructor
    ~eclmplContactInfo();

    unsigned int providedBy() const;
    void setProvidedBy(const unsigned int& provBy);
    unsigned int getIntendedFor() const;
    void setIntendedFor(const unsigned int& intFor);
    const std::vector<std::string> contactArguments() const;
    void setContactArguments(const std::vector<std::string>& contArgs);
    unsigned int nrOfContactArguments() const;

    friend std::ostream& operator<< (std::ostream& os, const eclmplContactInfo& info);
    
    //@} // End of Public Class Methods of eclmplContactInfo.
private:
    /**@name Private Class Attributes of eclmplContactInfo. */
    //@{
    
    unsigned int providedById;
    unsigned int intendedForId;
    std::vector<std::string> contactArgs;
    
    //@} // End of Private Class Attributes of eclmplContactInfo.
};

/** The slaveStartupInfo struct.

    This structure is used for storing information necessary to start-up a slave.
*/
struct slaveStartupInfo {
    /**@name Public Struct Attributes of slaveStartupInfo. */
    //@{

    /// Configuration table entry for a particular slave.
    std::vector<std::string> configTableEntry;

    /// Information that the slave needs to contact the master.
    eclmplContactInfo masterContactInfo;

//@} // End of Public Struct Attributes of slaveStartupInfo.
};

#endif
