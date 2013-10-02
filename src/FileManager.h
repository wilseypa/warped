#ifndef WARPED_FILE_MANAGER_H
#define WARPED_FILE_MANAGER_H


#include <fstream>
using std::fstream;
using std::ios;
#include <string>
using std::string;

namespace warped {

class FileManager {
public:
    static FileManager& instance();
    virtual fstream& open(const string& fileName, ios::openmode) = 0;

protected:
    FileManager();
    virtual ~FileManager() = 0;

private:
    static FileManager* initSingleton();
};

}
#endif
