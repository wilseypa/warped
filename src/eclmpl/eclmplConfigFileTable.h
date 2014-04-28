#ifndef ECLMPL_CONFIG_FILE_TABLE_H
#define ECLMPL_CONFIG_FILE_TABLE_H

#include "eclmplCommonInclude.h"

/// The eclmplConfigFileTable class.
class eclmplConfigFileTable {
public:
    /**@name Public Class Methods of eclmplConfigFileTable. */
    //@{
    
    /// Default Constructor
    eclmplConfigFileTable();
    
    /// Destructor
    ~eclmplConfigFileTable();

    void addEntry(const std::vector<std::string>& entry);
    std::vector<std::string> getEntry(const unsigned int& entryNr) const;
    void addToEntry(const std::string& add, const unsigned int& entryNr);
    unsigned int getNumberOfEntries() const;
    
    void serialize(char* buf, unsigned int& bufSize) const;
    void deserialize(const char* const buf);
    
    friend std::ostream& operator<< (std::ostream& os, const eclmplConfigFileTable& in);
    
    //@} // End of Public Class Methods of eclmplConfigFileTable.
private:
    /**@name Private Class Attributes of eclmplConfigFileTable. */
    //@{
    
    std::vector<std::vector<std::string>> configTable;
    
    //@} // End of Private Class Attributes of eclmplConfigFileTable.
};

#endif
