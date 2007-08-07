#ifndef _INCLUDED_LOOKAHEADSET_
#define _INCLUDED_LOOKAHEADSET_

#include "../firstset/firstset.h"
#include "../terminal/terminal.h"

class LookaheadSet: public FirstSet
{                   // Elements const ptrs are known to be Terminal const ptrs
    public:
        enum EndStatus
        {
            e_withoutEOF,
            e_withEOF,
        };

    private:        
        EndStatus d_EOF;                // end-marker in the lookahead set

    public:
        LookaheadSet(EndStatus eof = e_withoutEOF);
        LookaheadSet(FirstSet const &firstSet);
        LookaheadSet(LookaheadSet const &other);

        LookaheadSet &operator-=(LookaheadSet const &other);
        LookaheadSet &operator+=(LookaheadSet const &other);
        LookaheadSet &operator+=(FirstSet const &other);

        LookaheadSet &operator-=(Symbol const *symbol);

                        // true if *this contains other, 
                        // i.e., true if other has NO new elements
        bool operator>=(LookaheadSet const &other) const;

                        //  true if `symbol' is found in *this
        bool operator>=(Symbol const *symbol) const;
        bool operator<(LookaheadSet const &other) const;
        bool operator==(LookaheadSet const &other) const;

        std::ostream &insert(std::ostream &out) const;

        LookaheadSet intersection(LookaheadSet const &other) const;

        bool hasEOF() const;
        void rmEOF();
        bool empty() const;
        size_t fullSize() const;

    private:
        LookaheadSet(Element const **begin, Element const **end);
};

inline LookaheadSet::LookaheadSet(Element const **begin, Element const **end)
:
    FirstSet(begin, end)
{}

inline bool LookaheadSet::operator>=(Symbol const *symbol) const
{
    return find(symbol) != end();
}

inline bool LookaheadSet::operator<(LookaheadSet const &other) const
{
    return not (*this >= other);
}

inline bool LookaheadSet::hasEOF() const
{
    return d_EOF == e_withEOF;
}        

inline void LookaheadSet::rmEOF()
{
    d_EOF = e_withoutEOF;
}        

inline bool LookaheadSet::empty() const
{
    return d_EOF == e_withoutEOF && FirstSet::empty();
}

inline size_t LookaheadSet::fullSize() const
{
    return size() + (d_EOF == e_withEOF);
}
        
                                // remove when insert() is virtual
std::ostream &operator<<(std::ostream &out, LookaheadSet const &LookaheadSet);

#endif


