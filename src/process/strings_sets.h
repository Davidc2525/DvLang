
#ifndef STRINT_SET_H
#define STRINT_SET_H
#include <iostream>

using namespace std;

namespace STRING_SET
{
class Entry
{
  public:
    Entry(string str, int len){
        this->content = str;
        this->len = len;
    };
    string content;
    int len;
};

class Strings
{
  private:
    int ref;
    Entry **entrys;

  public:
    Strings();
    ~Strings();
    int length(){return ref;}
    void set_entrys(Entry ** ens);
    Entry get(int iref);
    Entry operator[](int iref);
    int set(int iref, string);

    /*inicializar*/
    int put(int iref, Entry *entry);

    int put(string str);
    int concat(int iref, int iref2);
};
} // namespace STRING_SET

#endif
