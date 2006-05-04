#include "item.ih"

void Item::showKernel(Item const &item)
{
    msg() << indent << item.d_production->lhs()->name() << " -> " << spool;

    for_each(item.d_production->begin(), 
             item.d_production->begin() + item.d_dot, 
             &Symbol::displaySymbol);

    msg() << " . " << spool;

    for_each(item.d_production->begin() + item.d_dot, 
             item.d_production->end(), 
             &Symbol::displaySymbol);

    msg() << "  Rule " << item.d_production->nr() << ", dot " <<
                item.d_dot << spool;
}
