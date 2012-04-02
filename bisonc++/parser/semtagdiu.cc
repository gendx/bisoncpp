#include "parser.ih"

Parser::SemTag Parser::semTagDIU(size_t nElements, int idx, 
                              string const &unionField) const
{
    if (idx > static_cast<int>(nElements))
        return UNTYPED;
        
    if (idx < 0)
        return TYPEIGNORED;

    return unionField.length() ?  TYPED : UNTYPED;
}

