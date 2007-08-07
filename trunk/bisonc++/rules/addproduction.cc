#include "rules.ih"

void Rules::addProduction()
{
    d_currentProduction = new Production(d_currentRule);// create production

    d_production.push_back(d_currentProduction);        // put production in
                                                        // production  vector

    d_currentRule->addProduction(d_currentProduction);  // add production to 
                                                        // current rule.

    lineMsg() << "  Adding production rule " << 
                d_currentRule->nProductions() << 
                " (" << d_production.size() << " productions in total)" << 
                info;
}