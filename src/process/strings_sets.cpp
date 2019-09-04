
#include "strings_sets.h"

namespace STRING_SET
{
Strings::Strings()
{
    ref = 0;
    entrys = (Entry **)malloc(1000 * sizeof(Entry));
}
void Strings::set_entrys(Entry ** ens){
    entrys = ens;
}
int Strings::put(int iref, Entry *entry)
{
    entrys[iref] = entry;
    return iref;
}

int Strings::put(string str)
{
    int c = ref++;
    string s = str;
    Entry *e = new Entry(s, s.length());
    entrys[c] = e;

    return c;
}

Entry Strings::get(int iref)
{
    return *entrys[iref];
}

Entry Strings::operator[](int iref)
{
    return get(iref);
}

int Strings::set(int iref, string str)
{
    Entry *e = entrys[iref];
    e->content = str;
    e->len = str.length();
    return iref;
}

int Strings::concat(int iref, int iref2)
{
    string nv = get(iref).content.append(get(iref2).content);
    set(iref, nv);
    return iref;
}
} // namespace STRING_SET