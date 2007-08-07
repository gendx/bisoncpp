#include "generator.ih"

Generator::Generator(Rules const &rules, Parser const &parser)
:
    d_arg(Arg::instance()),
    d_rules(rules),
    d_parser(parser),
    d_baseclassScope(d_parser.className() + "Base::"),
    d_nameSpace(d_parser.nameSpace()),
    d_debug(d_parser.debugFlag()),
    d_writer(d_baseclassScope, rules)
{}