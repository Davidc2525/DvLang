
#include <iostream>
#include "../types.h"
#include "../process/strings_sets.h"
#include "../process/process.h"
typedef tuple<string,tuple<string,int>> element;

using namespace std;
namespace PROGRAM
{

class SymbInfo{
  public:
  string lib_name;
  string symb_name;
  int argc;
};

class LibInfo{
  public: 
  int count=0;
  SymbInfo** libs;
};

class DataProgram
{
  public:
    int main;
    int size;
    int globals;
    intdv *inst;
    STRING_SET::Entry ** entrys;
    PROCESS::LibLoad * libload;
};

/*** descomentar para protobuf read file
DataProgram* readProgram(string fileName);
void writeProgram(string, int,int, int *);
*/

DataProgram* readProgramJson(string fileName);
void writeProgramJson(string, int,int,int, intdv *,STRING_SET::Strings * string_set,LibInfo*);
 
} // namespace PROGRAM
