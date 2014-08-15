##rem $Id$

#ifndef CATALOGF_H
#define CATALOGF_H
// Interface file
// MUST be included in your code.
// If you are using FlexCat, you only need to include the header
// file generated by it.
// The locale.library is opened whenever is needed ...

#include <exec/types.h>

extern struct LocaleBase* LocaleBase;
// data-type that will be used to define the strings from the .cd file ...
// so, you need (if you want hard code) a file with a set of structures
// of this type, containing an identificator ID, and the string itself.
// this way, you could use this class without FlexCat and Locale.library,
// although i do not think WHY should you do that :)
struct CatMessage {
                    LONG   ID;
                    STRPTR textstring;
                  };

class CatalogF
{
public:
    CatalogF( const STRPTR   catalogFileName,
              const STRPTR   builtInLanguage ="english", // default
              const LONG     versionNumber =0 ,          // match any
              const STRPTR   languageName =0,            // use user defined
              struct Locale* loc =0 );                   // user defined
    const STRPTR GetStr(const CatMessage& mess) const;
    ~CatalogF();
private:
    struct Catalog* thecatalog;
    static unsigned counter;    // counter of opened objects
//    static struct LocaleBase* LocaleBase; // we do not bother if other
                                          // opened the library before
};
#endif  // CATALOGF_H
