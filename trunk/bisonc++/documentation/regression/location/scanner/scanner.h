#ifndef _SCANNER_H_
#define _SCANNER_H_

// #include <fstream>
// #include <string>


#if ! defined(_SKIP_YYFLEXLEXER_) && ! defined(_SYSINC_FLEXLEXER_H_)
#include <FlexLexer.h>
#define _SYSINC_FLEXLEXER_H_
#endif

// Use the next or a comparable enum if bison's tokens AREN'T used.
/*
    enum ScannerToken
    {
        DUMMY_256_VALUE = 256,      // do not use this one, but add new tokens
            // without assignment below, in a comma separated list. This will
            // give the first real token the value 257, allowing you to return
            // plain characters as tokens.
    
    };                                            
*/

#include "../parser/Parserbase.h"
    
class Scanner: public yyFlexLexer
{
    Parser::LTYPE__ *d_loc;
    Parser::STYPE__ *d_val;

    public:
        Scanner(Parser::LTYPE__ *loc, Parser::STYPE__ *val)
        :
            d_loc(loc),
            d_val(val)
        {}
        int yylex();
};

#endif


