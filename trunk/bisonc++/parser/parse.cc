// $insert class.ih
#include "parser.ih"

// The FIRST element of SR arrays shown below uses `d_type', defining the
// state's type, and `d_lastIdx' containing the last element's index. If
// d_lastIdx contains the REQ_TOKEN bitflag (see below) then the state needs
// a token: if in this state d_token__ is _UNDETERMINED_, nextToken() will be
// called

// The LAST element of SR arrays uses `d_token' containing the last retrieved
// token to speed up the (linear) seach.  Except for the first element of SR
// arrays, the field `d_action' is used to determine what to do next. If
// positive, it represents the next state (used with SHIFT); if zero, it
// indicates `ACCEPT', if negative, -d_action represents the number of the
// rule to reduce to.

// `lookup()' tries to find d_token__ in the current SR array. If it fails, and
// there is no default reduction UNEXPECTED_TOKEN__ is thrown, which is then
// caught by the error-recovery function.

// The error-recovery function will pop elements off the stack until a state
// having bit flag ERR_ITEM is found. This state has a transition on _error_
// which is applied. In this _error_ state, while the current token is not a
// proper continuation, new tokens are obtained by nextToken(). If such a
// token is found, error recovery is successful and the token is
// handled according to the error state's SR table and parsing continues.

// A state flagged with the DEF_RED flag will perform a default
// reduction if no other continuations are available for the current token.

// The ACCEPT STATE never shows a default reduction: when it is reached the
// parser returns ACCEPT(). During the grammar
// analysis phase a default reduction may have been defined, but it is
// removed during the state-definition phase.

// So:
//      s_x[] = 
//      {
//                  [_field_1_]         [_field_2_]
//
// First element:   {state-type,        idx of last element},
// Other elements:  {required token,    action to perform},
//                                      ( < 0: reduce, 
//                                          0: ACCEPT,
//                                        > 0: next state)
// Last element:    {set to d_token__,    action to perform}
//      }

// When the --thread-safe option is specified, all static data are defined as
// const. If --thread-safe is not provided, the state-tables are not defined
// as const, since the lookup() function below will modify them


namespace // anonymous
{
    char const author[] = "Frank B. Brokken (f.b.brokken@rug.nl)";

    enum 
    {
        STACK_EXPANSION = 5     // size to expand the state-stack with when
                                // full
    };

    enum ReservedTokens
    {
        PARSE_ACCEPT     = 0,   // `ACCEPT' TRANSITION
        _UNDETERMINED_   = -2,
        _EOF_            = -1,
        _error_          = 256
    };
    enum StateType       // modify statetype/data.cc when this enum changes
    {
        NORMAL,
        ERR_ITEM,
        REQ_TOKEN,
        ERR_REQ,    // ERR_ITEM | REQ_TOKEN
        DEF_RED,    // state having default reduction
        ERR_DEF,    // ERR_ITEM | DEF_RED
        REQ_DEF,    // REQ_TOKEN | DEF_RED
        ERR_REQ_DEF // ERR_ITEM | REQ_TOKEN | DEF_RED
    };    
    struct PI__     // Production Info
    {
        size_t d_nonTerm; // identification number of this production's
                            // non-terminal 
        size_t d_size;    // number of elements in this production 
    };

    struct SR__     // Shift Reduce info, see its description above
    {
        union
        {
            int _field_1_;      // initializer, allowing initializations 
                                // of the SR s_[] arrays
            int d_type;
            int d_token;
        };
        union
        {
            int _field_2_;

            int d_lastIdx;          // if negative, the state uses SHIFT
            int d_action;           // may be negative (reduce), 
                                    // postive (shift), or 0 (accept)
            size_t d_errorState;    // used with Error states
        };
    };

    // $insert staticdata
    
// Productions Info Records:
PI__ const s_productionInfo[] = 
{
     {0, 0}, // not used: reduction values are negative
     {299, 4}, // 1: input ->  directives two_percents rules optTwo_percents
     {301, 1}, // 2: two_percents ->  TWO_PERCENTS
     {304, 1}, // 3: baseclass_header_t ->  BASECLASS_HEADER
     {305, 1}, // 4: baseclass_preinclude_t ->  BASECLASS_PREINCLUDE
     {306, 1}, // 5: class_header_t ->  CLASS_HEADER
     {307, 1}, // 6: class_name_t ->  CLASS_NAME
     {308, 1}, // 7: expect_t ->  EXPECT
     {309, 1}, // 8: filenames_t ->  FILENAMES
     {310, 1}, // 9: implementation_header_t ->  IMPLEMENTATION_HEADER
     {311, 1}, // 10: locationstruct_t ->  LOCATIONSTRUCT
     {312, 1}, // 11: ltype_t ->  LTYPE
     {313, 1}, // 12: namespace_t ->  NAMESPACE
     {314, 1}, // 13: parsefun_source_t ->  PARSEFUN_SOURCE
     {315, 1}, // 14: required_t ->  REQUIRED
     {316, 1}, // 15: scanner_include_t ->  SCANNER_INCLUDE
     {317, 1}, // 16: start_t ->  START
     {318, 1}, // 17: stype_t ->  STYPE
     {319, 0}, // 18: symbol_m ->  <empty>
     {320, 0}, // 19: token_m ->  <empty>
     {321, 0}, // 20: typesymbol_m ->  <empty>
     {322, 1}, // 21: type_t ->  TYPE
     {323, 1}, // 22: union_t ->  UNION
     {324, 2}, // 23: left_p ->  LEFT typesymbol_m
     {325, 2}, // 24: nonassoc_p ->  NONASSOC typesymbol_m
     {326, 2}, // 25: right_p ->  RIGHT typesymbol_m
     {327, 2}, // 26: token_p ->  TOKEN typesymbol_m
     {328, 1}, // 27: optComma ->  ','
     {328, 0}, // 28: optComma ->  <empty>
     {329, 1}, // 29: optNumber ->  NUMBER
     {329, 0}, // 30: optNumber ->  <empty>
     {330, 1}, // 31: optSemiCol ->  ';'
     {330, 0}, // 32: optSemiCol ->  <empty>
     {331, 2}, // 33: optTypename ->  typename_a token_m
     {331, 1}, // 34: optTypename ->  token_m
     {303, 1}, // 35: optTwo_percents ->  TWO_PERCENTS
     {303, 0}, // 36: optTwo_percents ->  <empty>
     {333, 1}, // 37: d_pxstring ->  PSTRING
     {333, 1}, // 38: d_pxstring ->  STRING
     {333, 1}, // 39: d_pxstring ->  XSTRING
     {334, 0}, // 40: d_incrementPrecedence ->  <empty>
     {335, 0}, // 41: d_pushPrecedence ->  <empty>
     {300, 2}, // 42: directives ->  directives directive
     {300, 0}, // 43: directives ->  <empty>
     {336, 1}, // 44: directive ->  directiveAlt
     {337, 2}, // 45: directiveAlt ->  baseclass_header_t d_pxstring
     {337, 2}, // 46: directiveAlt ->  baseclass_preinclude_t d_pxstring
     {337, 2}, // 47: directiveAlt ->  class_header_t d_pxstring
     {337, 2}, // 48: directiveAlt ->  class_name_t IDENTIFIER
     {337, 1}, // 49: directiveAlt ->  DEBUGFLAG
     {337, 1}, // 50: directiveAlt ->  ERROR_VERBOSE
     {337, 2}, // 51: directiveAlt ->  expect_t NUMBER
     {337, 2}, // 52: directiveAlt ->  filenames_t d_pxstring
     {337, 2}, // 53: directiveAlt ->  implementation_header_t d_pxstring
     {337, 4}, // 54: directiveAlt ->  left_p d_incrementPrecedence optTypename symbols
     {337, 1}, // 55: directiveAlt ->  LINES
     {337, 3}, // 56: directiveAlt ->  locationstruct_t BLOCK optSemiCol
     {337, 1}, // 57: directiveAlt ->  LSP_NEEDED
     {337, 2}, // 58: directiveAlt ->  ltype_t XSTRING
     {337, 2}, // 59: directiveAlt ->  namespace_t IDENTIFIER
     {337, 1}, // 60: directiveAlt ->  NEG_DOLLAR
     {337, 4}, // 61: directiveAlt ->  nonassoc_p d_incrementPrecedence optTypename symbols
     {337, 2}, // 62: directiveAlt ->  parsefun_source_t d_pxstring
     {337, 2}, // 63: directiveAlt ->  required_t NUMBER
     {337, 4}, // 64: directiveAlt ->  right_p d_incrementPrecedence optTypename symbols
     {337, 2}, // 65: directiveAlt ->  scanner_include_t d_pxstring
     {337, 2}, // 66: directiveAlt ->  start_t IDENTIFIER
     {337, 2}, // 67: directiveAlt ->  stype_t XSTRING
     {337, 4}, // 68: directiveAlt ->  token_p optTypename d_pushPrecedence symbols
     {337, 3}, // 69: directiveAlt ->  type_t typename_a symbols
     {337, 3}, // 70: directiveAlt ->  union_t BLOCK optSemiCol
     {337, 1}, // 71: directiveAlt ->  _error_
     {338, 3}, // 72: symbols ->  symbol_m symbolList optSemiCol
     {339, 3}, // 73: symbolList ->  symbolList optComma symbol
     {339, 1}, // 74: symbolList ->  symbol
     {340, 1}, // 75: symbol ->  QUOTE
     {340, 2}, // 76: symbol ->  identifier_a optNumber
     {302, 2}, // 77: rules ->  rules rule
     {302, 0}, // 78: rules ->  <empty>
     {341, 3}, // 79: rule ->  ruleName productionList ';'
     {342, 2}, // 80: ruleName ->  identifier_a ':'
     {344, 1}, // 81: p_precSpec ->  IDENTIFIER
     {344, 1}, // 82: p_precSpec ->  QUOTE
     {343, 3}, // 83: productionList ->  productionList productionSeparator production
     {343, 1}, // 84: productionList ->  production
     {345, 1}, // 85: productionSeparator ->  '|'
     {346, 1}, // 86: production ->  productionElements
     {346, 0}, // 87: production ->  <empty>
     {347, 2}, // 88: productionElements ->  productionElements productionElement
     {347, 1}, // 89: productionElements ->  productionElement
     {348, 1}, // 90: productionElement ->  QUOTE
     {348, 1}, // 91: productionElement ->  IDENTIFIER
     {348, 1}, // 92: productionElement ->  BLOCK
     {348, 2}, // 93: productionElement ->  PREC p_precSpec
     {349, 1}, // 94: identifier_a ->  IDENTIFIER
     {332, 3}, // 95: typename_a ->  '<' identifier_a '>'
     {350, 1}, // 96: input_$ ->  input
};

// State info and SR__ transitions for each state.


SR__ s_0[] =
{
    { { DEF_RED}, {   3} },              
    { {     299}, {   1} }, // input     
    { {     300}, {   2} }, // directives
    { {       0}, { -43} },              

};

SR__ s_1[] =
{
    { { REQ_TOKEN}, {            2} }, 
    { {     _EOF_}, { PARSE_ACCEPT} }, 
    { {         0}, {            0} }, 

};

SR__ s_2[] =
{
    { { ERR_REQ}, { 53} },                           
    { {     301}, {  3} }, // two_percents           
    { {     336}, {  4} }, // directive              
    { {     288}, {  5} }, // TWO_PERCENTS           
    { {     337}, {  6} }, // directiveAlt           
    { {     304}, {  7} }, // baseclass_header_t     
    { {     305}, {  8} }, // baseclass_preinclude_t 
    { {     306}, {  9} }, // class_header_t         
    { {     307}, { 10} }, // class_name_t           
    { {     262}, { 11} }, // DEBUGFLAG              
    { {     263}, { 12} }, // ERROR_VERBOSE          
    { {     308}, { 13} }, // expect_t               
    { {     309}, { 14} }, // filenames_t            
    { {     310}, { 15} }, // implementation_header_t
    { {     324}, { 16} }, // left_p                 
    { {     269}, { 17} }, // LINES                  
    { {     311}, { 18} }, // locationstruct_t       
    { {     271}, { 19} }, // LSP_NEEDED             
    { {     312}, { 20} }, // ltype_t                
    { {     313}, { 21} }, // namespace_t            
    { {     274}, { 22} }, // NEG_DOLLAR             
    { {     325}, { 23} }, // nonassoc_p             
    { {     314}, { 24} }, // parsefun_source_t      
    { {     315}, { 25} }, // required_t             
    { {     326}, { 26} }, // right_p                
    { {     316}, { 27} }, // scanner_include_t      
    { {     317}, { 28} }, // start_t                
    { {     318}, { 29} }, // stype_t                
    { {     327}, { 30} }, // token_p                
    { {     322}, { 31} }, // type_t                 
    { {     323}, { 32} }, // union_t                
    { { _error_}, { 33} }, // _error_                
    { {     257}, { 34} }, // BASECLASS_HEADER       
    { {     258}, { 35} }, // BASECLASS_PREINCLUDE   
    { {     260}, { 36} }, // CLASS_HEADER           
    { {     261}, { 37} }, // CLASS_NAME             
    { {     264}, { 38} }, // EXPECT                 
    { {     265}, { 39} }, // FILENAMES              
    { {     267}, { 40} }, // IMPLEMENTATION_HEADER  
    { {     268}, { 41} }, // LEFT                   
    { {     270}, { 42} }, // LOCATIONSTRUCT         
    { {     272}, { 43} }, // LTYPE                  
    { {     273}, { 44} }, // NAMESPACE              
    { {     275}, { 45} }, // NONASSOC               
    { {     277}, { 46} }, // PARSEFUN_SOURCE        
    { {     281}, { 47} }, // REQUIRED               
    { {     282}, { 48} }, // RIGHT                  
    { {     283}, { 49} }, // SCANNER_INCLUDE        
    { {     284}, { 50} }, // START                  
    { {     286}, { 51} }, // STYPE                  
    { {     287}, { 52} }, // TOKEN                  
    { {     289}, { 53} }, // TYPE                   
    { {     290}, { 54} }, // UNION                  
    { {       0}, {  0} },                           

};

SR__ s_3[] =
{
    { { DEF_RED}, {   2} },         
    { {     302}, {  55} }, // rules
    { {       0}, { -78} },         

};

SR__ s_4[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -42} }, 

};

SR__ s_5[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -2} }, 

};

SR__ s_6[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -44} }, 

};

SR__ s_7[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 56} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_8[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 60} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_9[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 61} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_10[] =
{
    { { REQ_TOKEN}, {  2} },              
    { {       266}, { 62} }, // IDENTIFIER
    { {         0}, {  0} },              

};

SR__ s_11[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -49} }, 

};

SR__ s_12[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -50} }, 

};

SR__ s_13[] =
{
    { { REQ_TOKEN}, {  2} },          
    { {       276}, { 63} }, // NUMBER
    { {         0}, {  0} },          

};

SR__ s_14[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 64} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_15[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 65} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_16[] =
{
    { { DEF_RED}, {   2} },                         
    { {     334}, {  66} }, // d_incrementPrecedence
    { {       0}, { -40} },                         

};

SR__ s_17[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -55} }, 

};

SR__ s_18[] =
{
    { { REQ_TOKEN}, {  2} },         
    { {       259}, { 67} }, // BLOCK
    { {         0}, {  0} },         

};

SR__ s_19[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -57} }, 

};

SR__ s_20[] =
{
    { { REQ_TOKEN}, {  2} },           
    { {       291}, { 68} }, // XSTRING
    { {         0}, {  0} },           

};

SR__ s_21[] =
{
    { { REQ_TOKEN}, {  2} },              
    { {       266}, { 69} }, // IDENTIFIER
    { {         0}, {  0} },              

};

SR__ s_22[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -60} }, 

};

SR__ s_23[] =
{
    { { DEF_RED}, {   2} },                         
    { {     334}, {  70} }, // d_incrementPrecedence
    { {       0}, { -40} },                         

};

SR__ s_24[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 71} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_25[] =
{
    { { REQ_TOKEN}, {  2} },          
    { {       276}, { 72} }, // NUMBER
    { {         0}, {  0} },          

};

SR__ s_26[] =
{
    { { DEF_RED}, {   2} },                         
    { {     334}, {  73} }, // d_incrementPrecedence
    { {       0}, { -40} },                         

};

SR__ s_27[] =
{
    { { REQ_TOKEN}, {  5} },              
    { {       333}, { 74} }, // d_pxstring
    { {       279}, { 57} }, // PSTRING   
    { {       285}, { 58} }, // STRING    
    { {       291}, { 59} }, // XSTRING   
    { {         0}, {  0} },              

};

SR__ s_28[] =
{
    { { REQ_TOKEN}, {  2} },              
    { {       266}, { 75} }, // IDENTIFIER
    { {         0}, {  0} },              

};

SR__ s_29[] =
{
    { { REQ_TOKEN}, {  2} },           
    { {       291}, { 76} }, // XSTRING
    { {         0}, {  0} },           

};

SR__ s_30[] =
{
    { { REQ_DEF}, {   5} },               
    { {     331}, {  77} }, // optTypename
    { {     332}, {  78} }, // typename_a 
    { {     320}, {  79} }, // token_m    
    { {      60}, {  80} }, // '<'        
    { {       0}, { -19} },               

};

SR__ s_31[] =
{
    { { REQ_TOKEN}, {  3} },              
    { {       332}, { 81} }, // typename_a
    { {        60}, { 80} }, // '<'       
    { {         0}, {  0} },              

};

SR__ s_32[] =
{
    { { REQ_TOKEN}, {  2} },         
    { {       259}, { 82} }, // BLOCK
    { {         0}, {  0} },         

};

SR__ s_33[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -71} }, 

};

SR__ s_34[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -3} }, 

};

SR__ s_35[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -4} }, 

};

SR__ s_36[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -5} }, 

};

SR__ s_37[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -6} }, 

};

SR__ s_38[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -7} }, 

};

SR__ s_39[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -8} }, 

};

SR__ s_40[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -9} }, 

};

SR__ s_41[] =
{
    { { DEF_RED}, {   2} },                
    { {     321}, {  83} }, // typesymbol_m
    { {       0}, { -20} },                

};

SR__ s_42[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -10} }, 

};

SR__ s_43[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -11} }, 

};

SR__ s_44[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -12} }, 

};

SR__ s_45[] =
{
    { { DEF_RED}, {   2} },                
    { {     321}, {  84} }, // typesymbol_m
    { {       0}, { -20} },                

};

SR__ s_46[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -13} }, 

};

SR__ s_47[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -14} }, 

};

SR__ s_48[] =
{
    { { DEF_RED}, {   2} },                
    { {     321}, {  85} }, // typesymbol_m
    { {       0}, { -20} },                

};

SR__ s_49[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -15} }, 

};

SR__ s_50[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -16} }, 

};

SR__ s_51[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -17} }, 

};

SR__ s_52[] =
{
    { { DEF_RED}, {   2} },                
    { {     321}, {  86} }, // typesymbol_m
    { {       0}, { -20} },                

};

SR__ s_53[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -21} }, 

};

SR__ s_54[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -22} }, 

};

SR__ s_55[] =
{
    { { REQ_DEF}, {   7} },                   
    { {     303}, {  87} }, // optTwo_percents
    { {     341}, {  88} }, // rule           
    { {     288}, {  89} }, // TWO_PERCENTS   
    { {     342}, {  90} }, // ruleName       
    { {     349}, {  91} }, // identifier_a   
    { {     266}, {  92} }, // IDENTIFIER     
    { {       0}, { -36} },                   

};

SR__ s_56[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -45} }, 

};

SR__ s_57[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -37} }, 

};

SR__ s_58[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -38} }, 

};

SR__ s_59[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -39} }, 

};

SR__ s_60[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -46} }, 

};

SR__ s_61[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -47} }, 

};

SR__ s_62[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -48} }, 

};

SR__ s_63[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -51} }, 

};

SR__ s_64[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -52} }, 

};

SR__ s_65[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -53} }, 

};

SR__ s_66[] =
{
    { { REQ_DEF}, {   5} },               
    { {     331}, {  93} }, // optTypename
    { {     332}, {  78} }, // typename_a 
    { {     320}, {  79} }, // token_m    
    { {      60}, {  80} }, // '<'        
    { {       0}, { -19} },               

};

SR__ s_67[] =
{
    { { REQ_DEF}, {   3} },              
    { {     330}, {  94} }, // optSemiCol
    { {      59}, {  95} }, // ';'       
    { {       0}, { -32} },              

};

SR__ s_68[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -58} }, 

};

SR__ s_69[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -59} }, 

};

SR__ s_70[] =
{
    { { REQ_DEF}, {   5} },               
    { {     331}, {  96} }, // optTypename
    { {     332}, {  78} }, // typename_a 
    { {     320}, {  79} }, // token_m    
    { {      60}, {  80} }, // '<'        
    { {       0}, { -19} },               

};

SR__ s_71[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -62} }, 

};

SR__ s_72[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -63} }, 

};

SR__ s_73[] =
{
    { { REQ_DEF}, {   5} },               
    { {     331}, {  97} }, // optTypename
    { {     332}, {  78} }, // typename_a 
    { {     320}, {  79} }, // token_m    
    { {      60}, {  80} }, // '<'        
    { {       0}, { -19} },               

};

SR__ s_74[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -65} }, 

};

SR__ s_75[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -66} }, 

};

SR__ s_76[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -67} }, 

};

SR__ s_77[] =
{
    { { DEF_RED}, {   2} },                    
    { {     335}, {  98} }, // d_pushPrecedence
    { {       0}, { -41} },                    

};

SR__ s_78[] =
{
    { { DEF_RED}, {   2} },           
    { {     320}, {  99} }, // token_m
    { {       0}, { -19} },           

};

SR__ s_79[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -34} }, 

};

SR__ s_80[] =
{
    { { REQ_TOKEN}, {   3} },                
    { {       349}, { 100} }, // identifier_a
    { {       266}, {  92} }, // IDENTIFIER  
    { {         0}, {   0} },                

};

SR__ s_81[] =
{
    { { DEF_RED}, {   3} },            
    { {     338}, { 101} }, // symbols 
    { {     319}, { 102} }, // symbol_m
    { {       0}, { -18} },            

};

SR__ s_82[] =
{
    { { REQ_DEF}, {   3} },              
    { {     330}, { 103} }, // optSemiCol
    { {      59}, {  95} }, // ';'       
    { {       0}, { -32} },              

};

SR__ s_83[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -23} }, 

};

SR__ s_84[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -24} }, 

};

SR__ s_85[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -25} }, 

};

SR__ s_86[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -26} }, 

};

SR__ s_87[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -1} }, 

};

SR__ s_88[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -77} }, 

};

SR__ s_89[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -35} }, 

};

SR__ s_90[] =
{
    { { REQ_DEF}, {   9} },                      
    { {     343}, { 104} }, // productionList    
    { {     346}, { 105} }, // production        
    { {     347}, { 106} }, // productionElements
    { {     348}, { 107} }, // productionElement 
    { {     280}, { 108} }, // QUOTE             
    { {     266}, { 109} }, // IDENTIFIER        
    { {     259}, { 110} }, // BLOCK             
    { {     278}, { 111} }, // PREC              
    { {       0}, { -87} },                      

};

SR__ s_91[] =
{
    { { REQ_TOKEN}, {   2} },       
    { {        58}, { 112} }, // ':'
    { {         0}, {   0} },       

};

SR__ s_92[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -94} }, 

};

SR__ s_93[] =
{
    { { DEF_RED}, {   3} },            
    { {     338}, { 113} }, // symbols 
    { {     319}, { 102} }, // symbol_m
    { {       0}, { -18} },            

};

SR__ s_94[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -56} }, 

};

SR__ s_95[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -31} }, 

};

SR__ s_96[] =
{
    { { DEF_RED}, {   3} },            
    { {     338}, { 114} }, // symbols 
    { {     319}, { 102} }, // symbol_m
    { {       0}, { -18} },            

};

SR__ s_97[] =
{
    { { DEF_RED}, {   3} },            
    { {     338}, { 115} }, // symbols 
    { {     319}, { 102} }, // symbol_m
    { {       0}, { -18} },            

};

SR__ s_98[] =
{
    { { DEF_RED}, {   3} },            
    { {     338}, { 116} }, // symbols 
    { {     319}, { 102} }, // symbol_m
    { {       0}, { -18} },            

};

SR__ s_99[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -33} }, 

};

SR__ s_100[] =
{
    { { REQ_TOKEN}, {   2} },       
    { {        62}, { 117} }, // '>'
    { {         0}, {   0} },       

};

SR__ s_101[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -69} }, 

};

SR__ s_102[] =
{
    { { REQ_TOKEN}, {   6} },                
    { {       339}, { 118} }, // symbolList  
    { {       340}, { 119} }, // symbol      
    { {       280}, { 120} }, // QUOTE       
    { {       349}, { 121} }, // identifier_a
    { {       266}, {  92} }, // IDENTIFIER  
    { {         0}, {   0} },                

};

SR__ s_103[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -70} }, 

};

SR__ s_104[] =
{
    { { REQ_TOKEN}, {   4} },                       
    { {        59}, { 122} }, // ';'                
    { {       345}, { 123} }, // productionSeparator
    { {       124}, { 124} }, // '|'                
    { {         0}, {   0} },                       

};

SR__ s_105[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -84} }, 

};

SR__ s_106[] =
{
    { { REQ_DEF}, {   6} },                     
    { {     348}, { 125} }, // productionElement
    { {     280}, { 108} }, // QUOTE            
    { {     266}, { 109} }, // IDENTIFIER       
    { {     259}, { 110} }, // BLOCK            
    { {     278}, { 111} }, // PREC             
    { {       0}, { -86} },                     

};

SR__ s_107[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -89} }, 

};

SR__ s_108[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -90} }, 

};

SR__ s_109[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -91} }, 

};

SR__ s_110[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -92} }, 

};

SR__ s_111[] =
{
    { { REQ_TOKEN}, {   4} },              
    { {       344}, { 126} }, // p_precSpec
    { {       266}, { 127} }, // IDENTIFIER
    { {       280}, { 128} }, // QUOTE     
    { {         0}, {   0} },              

};

SR__ s_112[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -80} }, 

};

SR__ s_113[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -54} }, 

};

SR__ s_114[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -61} }, 

};

SR__ s_115[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -64} }, 

};

SR__ s_116[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -68} }, 

};

SR__ s_117[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -95} }, 

};

SR__ s_118[] =
{
    { { REQ_DEF}, {  33} },                         
    { {     330}, { 129} }, // optSemiCol           
    { {     328}, { 130} }, // optComma             
    { {      59}, {  95} }, // ';'                  
    { {      44}, { 131} }, // ','                  
    { { _error_}, { -32} }, // _error_              
    { {     257}, { -32} }, // BASECLASS_HEADER     
    { {     258}, { -32} }, // BASECLASS_PREINCLUDE 
    { {     260}, { -32} }, // CLASS_HEADER         
    { {     261}, { -32} }, // CLASS_NAME           
    { {     262}, { -32} }, // DEBUGFLAG            
    { {     263}, { -32} }, // ERROR_VERBOSE        
    { {     272}, { -32} }, // LTYPE                
    { {     264}, { -32} }, // EXPECT               
    { {     265}, { -32} }, // FILENAMES            
    { {     267}, { -32} }, // IMPLEMENTATION_HEADER
    { {     268}, { -32} }, // LEFT                 
    { {     269}, { -32} }, // LINES                
    { {     270}, { -32} }, // LOCATIONSTRUCT       
    { {     271}, { -32} }, // LSP_NEEDED           
    { {     273}, { -32} }, // NAMESPACE            
    { {     274}, { -32} }, // NEG_DOLLAR           
    { {     275}, { -32} }, // NONASSOC             
    { {     277}, { -32} }, // PARSEFUN_SOURCE      
    { {     281}, { -32} }, // REQUIRED             
    { {     282}, { -32} }, // RIGHT                
    { {     283}, { -32} }, // SCANNER_INCLUDE      
    { {     284}, { -32} }, // START                
    { {     286}, { -32} }, // STYPE                
    { {     287}, { -32} }, // TOKEN                
    { {     288}, { -32} }, // TWO_PERCENTS         
    { {     289}, { -32} }, // TYPE                 
    { {     290}, { -32} }, // UNION                
    { {       0}, { -28} },                         

};

SR__ s_119[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -74} }, 

};

SR__ s_120[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -75} }, 

};

SR__ s_121[] =
{
    { { REQ_DEF}, {   3} },             
    { {     329}, { 132} }, // optNumber
    { {     276}, { 133} }, // NUMBER   
    { {       0}, { -30} },             

};

SR__ s_122[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -79} }, 

};

SR__ s_123[] =
{
    { { REQ_DEF}, {   8} },                      
    { {     346}, { 134} }, // production        
    { {     347}, { 106} }, // productionElements
    { {     348}, { 107} }, // productionElement 
    { {     280}, { 108} }, // QUOTE             
    { {     266}, { 109} }, // IDENTIFIER        
    { {     259}, { 110} }, // BLOCK             
    { {     278}, { 111} }, // PREC              
    { {       0}, { -87} },                      

};

SR__ s_124[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -85} }, 

};

SR__ s_125[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -88} }, 

};

SR__ s_126[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -93} }, 

};

SR__ s_127[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -81} }, 

};

SR__ s_128[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -82} }, 

};

SR__ s_129[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -72} }, 

};

SR__ s_130[] =
{
    { { REQ_TOKEN}, {   5} },                
    { {       340}, { 135} }, // symbol      
    { {       280}, { 120} }, // QUOTE       
    { {       349}, { 121} }, // identifier_a
    { {       266}, {  92} }, // IDENTIFIER  
    { {         0}, {   0} },                

};

SR__ s_131[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -27} }, 

};

SR__ s_132[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -76} }, 

};

SR__ s_133[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -29} }, 

};

SR__ s_134[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -83} }, 

};

SR__ s_135[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -73} }, 

};


// State array:
SR__ *s_state[] =
{
  s_0,  s_1,  s_2,  s_3,  s_4,  s_5,  s_6,  s_7,  s_8,  s_9,
  s_10,  s_11,  s_12,  s_13,  s_14,  s_15,  s_16,  s_17,  s_18,  s_19,
  s_20,  s_21,  s_22,  s_23,  s_24,  s_25,  s_26,  s_27,  s_28,  s_29,
  s_30,  s_31,  s_32,  s_33,  s_34,  s_35,  s_36,  s_37,  s_38,  s_39,
  s_40,  s_41,  s_42,  s_43,  s_44,  s_45,  s_46,  s_47,  s_48,  s_49,
  s_50,  s_51,  s_52,  s_53,  s_54,  s_55,  s_56,  s_57,  s_58,  s_59,
  s_60,  s_61,  s_62,  s_63,  s_64,  s_65,  s_66,  s_67,  s_68,  s_69,
  s_70,  s_71,  s_72,  s_73,  s_74,  s_75,  s_76,  s_77,  s_78,  s_79,
  s_80,  s_81,  s_82,  s_83,  s_84,  s_85,  s_86,  s_87,  s_88,  s_89,
  s_90,  s_91,  s_92,  s_93,  s_94,  s_95,  s_96,  s_97,  s_98,  s_99,
  s_100,  s_101,  s_102,  s_103,  s_104,  s_105,  s_106,  s_107,  s_108,  s_109,
  s_110,  s_111,  s_112,  s_113,  s_114,  s_115,  s_116,  s_117,  s_118,  s_119,
  s_120,  s_121,  s_122,  s_123,  s_124,  s_125,  s_126,  s_127,  s_128,  s_129,
  s_130,  s_131,  s_132,  s_133,  s_134,  s_135,
};

} // anonymous namespace ends



// If the parsing function call uses arguments, then provide an overloaded
// function.  The code below doesn't rely on parameters, so no arguments are
// required.  Furthermore, parse uses a function try block to allow us to do
// ACCEPT and ABORT from anywhere, even from within members called by actions,
// simply throwing the appropriate exceptions.

ParserBase::ParserBase()
:
    d_stackIdx__(-1),
    // $insert debuginit
    d_debug__(false),
    d_nErrors__(0),
    // $insert requiredtokens
    d_requiredTokens__(0),
    d_acceptedTokens__(d_requiredTokens__),
    d_token__(_UNDETERMINED_),
    d_nextToken__(_UNDETERMINED_)
{}


void ParserBase::clearin()
{
    d_token__ = d_nextToken__ = _UNDETERMINED_;
}

void ParserBase::push__(size_t state)
{
    if (static_cast<size_t>(d_stackIdx__ + 1) == d_stateStack__.size())
    {
        size_t newSize = d_stackIdx__ + STACK_EXPANSION;
        d_stateStack__.resize(newSize);
        d_valueStack__.resize(newSize);
    }
    ++d_stackIdx__;
    d_stateStack__[d_stackIdx__] = d_state__ = state;
    *(d_vsp__ = &d_valueStack__[d_stackIdx__]) = d_val__;
}

void ParserBase::popToken__()
{
    d_token__ = d_nextToken__;
    d_nextToken__ = _UNDETERMINED_;
}
     
void ParserBase::pushToken__(int token)
{
    d_nextToken__ = d_token__;
    d_token__ = token;
}
     
void ParserBase::pop__(size_t count)
{
    if (d_stackIdx__ < static_cast<int>(count))
    {
        ABORT();
    }

    d_stackIdx__ -= count;
    d_state__ = d_stateStack__[d_stackIdx__];
    d_vsp__ = &d_valueStack__[d_stackIdx__];
}

inline size_t ParserBase::top__() const
{
    return d_stateStack__[d_stackIdx__];
}

inline void ParserBase::checkEOF__() const
{
    if (d_token__ == _EOF_)
    {
        throw DEFAULT_RECOVERY_MODE__;
    }
}

inline void ParserBase::reduce__(PI__ const &pi)
{
    pushToken__(pi.d_nonTerm);
    pop__(pi.d_size);

}

void Parser::executeAction(int production)
{
    switch (production)
    {
        // $insert actioncases
        
        case 2:
        {
         expectRules();
         }
        break;

        case 3:
        {
         d_msg = "baseclass header name";
         }
        break;

        case 4:
        {
         d_msg = "baseclass pre-include name";
         }
        break;

        case 5:
        {
         d_msg = "class header name";
         }
        break;

        case 6:
        {
         d_msg = "class name";
         }
        break;

        case 7:
        {
         d_msg = "number (of conflicts)";
         }
        break;

        case 8:
        {
         d_msg = "generic name of files";
         }
        break;

        case 9:
        {
         d_msg = "implementation header name";
         }
        break;

        case 10:
        {
         d_msg = "Location struct definition";
         }
        break;

        case 11:
        {
         d_msg = "Location type specification";
         }
        break;

        case 12:
        {
         d_msg = "Namespace identifier";
         }
        break;

        case 13:
        {
         d_msg = "File name for the parse() member";
         }
        break;

        case 14:
        {
         d_msg = "Required number of tokens between errors";
         }
        break;

        case 15:
        {
         d_msg = "Path to the scanner header filename";
         }
        break;

        case 16:
        {
         d_msg = "Start rule" ;
         }
        break;

        case 17:
        {
         d_msg = "STYPE type name" ;
         }
        break;

        case 18:
        {
         d_msg = "identifier or character-constant";
         }
        break;

        case 19:
        {
         d_msg = "token name";
         }
        break;

        case 20:
        {
         d_msg = "opt. <type> identifier(s) or char constant(s)";
         }
        break;

        case 21:
        {
         d_msg = "type-name";
         d_typeDirective = true;
         }
        break;

        case 22:
        {
         d_msg = "Semantic value union definition";
         }
        break;

        case 23:
        {
         d_association = Terminal::LEFT;
         }
        break;

        case 24:
        {
         d_association = Terminal::NONASSOC;
         }
        break;

        case 25:
        {
         d_association = Terminal::RIGHT;
         }
        break;

        case 26:
        {
         d_association = Terminal::UNDEFINED;
         }
        break;

        case 29:
        {
         d_val__.logic = true;
         }
        break;

        case 30:
        {
         d_val__.logic = false;
         }
        break;

        case 34:
        {
         d_field.clear();
         }
        break;

        case 35:
        {
         lineMsg() << 
         "Ignoring all input beyond the second %% token" << warning;
         ACCEPT();
         }
        break;

        case 37:
        {
         d_val__.type = PSTRING;
         }
        break;

        case 38:
        {
         d_val__.type = STRING;
         }
        break;

        case 39:
        {
         d_val__.type = XSTRING;
         }
        break;

        case 40:
        {
         Terminal::incrementPrecedence();
         }
        break;

        case 41:
        {
         d_val__.type = Terminal::sPrecedence();
         Terminal::resetPrecedence();
         }
        break;

        case 44:
        {
         d_msg.erase();
         d_typeDirective = false;
         }
        break;

        case 45:
        {
         setBaseclassHeader(d_vsp__[0].type);
         }
        break;

        case 46:
        {
         setPreInclude();
         }
        break;

        case 47:
        {
         setClassHeader(d_vsp__[0].type);
         }
        break;

        case 48:
        {
         setClassName();
         }
        break;

        case 49:
        {
         setDebugFlag();
         }
        break;

        case 50:
        {
         setErrorVerbose();
         }
        break;

        case 51:
        {
         setExpectedConflicts();
         }
        break;

        case 52:
        {
         setGenericFilename(d_vsp__[0].type);
         }
        break;

        case 53:
        {
         setImplementationHeader(d_vsp__[0].type);
         }
        break;

        case 55:
        {
         setLines();
         }
        break;

        case 56:
        {
         setLocationDecl();
         }
        break;

        case 57:
        {
         setLspNeeded();
         }
        break;

        case 58:
        {
         setLtype();
         }
        break;

        case 59:
        {
         setNameSpace();
         }
        break;

        case 60:
        {
         setNegativeDollar();
         }
        break;

        case 62:
        {
         setParsefunSource(d_vsp__[0].type);
         }
        break;

        case 63:
        {
         setRequiredTokens();
         }
        break;

        case 65:
        {
         setScannerInclude();
         }
        break;

        case 66:
        {
         setStart();
         }
        break;

        case 67:
        {
         setStype();
         }
        break;

        case 68:
        {
         Terminal::set_sPrecedence(d_vsp__[-1].type);
         }
        break;

        case 70:
        {
         setUnionDecl();
         }
        break;

        case 75:
        {
         defineTerminal(d_scanner.canonicalQuote(), Symbol::CHAR_TERMINAL);
         }
        break;

        case 76:
        {
         defineTokenName(d_vsp__[-1].sp, d_vsp__[0].logic); 
         }
        break;

        case 80:
        {
         openRule(d_vsp__[-1].sp);
         }
        break;

        case 81:
        {
         d_val__.type = IDENTIFIER;
         }
        break;

        case 82:
        {
         d_val__.type = QUOTE;
         }
        break;

        case 85:
        {
         d_rules.addProduction();
         }
        break;

        case 86:
        {
         handleProductionElement(d_vsp__[0].tag);
         
         
         }
        break;

        case 87:
        {
         
         
         checkEmptyBlocktype();
         }
        break;

        case 88:
        {
         d_val__.tag = handleProductionElements(d_vsp__[-1].tag, d_vsp__[0].tag);
         
         
         }
        break;

        case 90:
        {
         d_val__.tag = new PTag(useTerminal());
         }
        break;

        case 91:
        {
         d_val__.tag = new PTag(useSymbol());
         }
        break;

        case 92:
        {
         d_val__.tag = new PTag(d_scanner.block());
         }
        break;

        case 93:
        {
         setPrecedence(d_vsp__[0].type);
         d_val__.tag = 0; 
         }
        break;

        case 94:
        {
         d_val__.sp = newYYText();
         }
        break;

        case 95:
        {
         d_field = *d_vsp__[-1].sp;
         delete d_vsp__[-1].sp;
         }
        break;

    }
}

// If d_token__ is _UNDETERMINED_ then if d_nextToken__ is _UNDETERMINED_ another
// token is obtained from lex(). Then d_nextToken__ is assigned to d_token__.
void Parser::nextToken()
{
    if (d_token__ != _UNDETERMINED_)        // no need for a token: got one
        return;                             // already

    if (d_nextToken__ != _UNDETERMINED_)
    {
        popToken__();                       // consume pending token
    }
    else
    {
        ++d_acceptedTokens__;               // accept another token (see
                                            // errorRecover())
        d_token__ = lex();
        if (d_token__ <= 0)
            d_token__ = _EOF_;
    }
    print();
}

// if the final transition is negative, then we should reduce by the rule
// given by its positive value. Note that the `recovery' parameter is only
// used with the --debug option
int Parser::lookup(bool recovery)
{
    // $insert threading
    SR__ *sr = s_state[d_state__];        // get the appropriate state-table
    int lastIdx = sr->d_lastIdx;        // sentinel-index in the SR__ array

    SR__ *lastElementPtr = sr + lastIdx;
    SR__ *elementPtr = sr + 1;            // start the search at s_xx[1]

    lastElementPtr->d_token = d_token__;// set search-token

    while (elementPtr->d_token != d_token__)
        ++elementPtr;

    if (elementPtr == lastElementPtr)   // reached the last element
    {
        if (elementPtr->d_action < 0)   // default reduction
        {
            return elementPtr->d_action;                
        }

        // No default reduction, so token not found, so error.
        throw UNEXPECTED_TOKEN__;
    }

    // not at the last element: inspect the nature of the action
    // (< 0: reduce, 0: ACCEPT, > 0: shift)

    int action = elementPtr->d_action;


    return action;
}

    // When an error has occurred, pop elements off the stack until the top
    // state has an error-item. If none is found, the default recovery
    // mode (which is to abort) is activated. 
    //
    // If EOF is encountered without being appropriate for the current state,
    // then the error recovery will fall back to the default recovery mode.
    // (i.e., parsing terminates)
void Parser::errorRecovery()
try
{
    if (d_acceptedTokens__ >= d_requiredTokens__)// only generate an error-
    {                                           // message if enough tokens 
        ++d_nErrors__;                          // were accepted. Otherwise
        error("Syntax error");                  // simply skip input

    }


    while (not (s_state[top__()][0].d_type & ERR_ITEM))
    {
        pop__();
    }

        // if on entry here token is already EOF then we've probably been 
        // here before: _error_ accepts EOF, but the state using
        // error nevertheless doesn't. In that case parsing terminates 
    checkEOF__();

    pushToken__(_error_);                     // specify _error_ as next token
    push__(lookup(true));                     // push the error state

    // In the error state, lookup a token with which we can proceed.
    // It may be a reduce, but normally a shift is indicated
    // If a token is seen which doesn't fit, the catch below will catch the
    // exception thrown by lookup()

    while (true)
    {
            // if on entry here token is already EOF then we've probably been 
            // here before: _error_ accepts EOF, but the state using
            // error nevertheless doesn't. In that case parsing terminates 
        checkEOF__();
        try
        {
            popToken__();
            nextToken();

            int action = lookup(true);
            popToken__();                   // token processed

            if (action > 0)                 // push a new state
            {
                push__(action);
            }
            else if (action < 0)
            {
                executeAction(-action);     // the error's action

                                            // next token is the rule's LHS
                reduce__(s_productionInfo[-action]); 
            }
            d_acceptedTokens__ = 0;         // reset accept count on error
            return;
        }
        catch (...)
        {
            if (d_token__ == _EOF_)
                continue;
                      
            popToken__();                   // token processed
        }
    }
}
catch (ErrorRecovery__)       // This is: DEFAULT_RECOVERY_MODE
{
    ABORT();
}

    // The parsing algorithm:
    // Initially, state 0 is pushed on the stack, and d_token__ as well as
    // d_nextToken__ are initialized to _UNDETERMINED_. 
    //
    // Then, in an eternal loop:
    //
    //  1. If a state does not have REQ_TOKEN no token is assigned to
    //     d_token__. If the state has REQ_TOKEN, nextToken() is called to
    //      determine d_nextToken__ and d_token__ is set to
    //     d_nextToken__. nextToken() will not call lex() unless d_nextToken__ is 
    //     _UNDETERMINED_. 
    //
    //  2. lookup() is called: 
    //     d_token__ is stored in the final element's d_token field of the
    //     state's SR_ array. 
    //
    //  3. The current token is looked up in the state's SR_ array
    //
    //  4. Depending on the result of the lookup() function the next state is
    //     shifted on the parser's stack, a reduction by some rule is applied,
    //     or the parsing function returns ACCEPT(). When a reduction is
    //     called for, any action that may have been defined for that
    //     reduction is executed.
    //
    //  5. An error occurs if d_token__ is not found, and the state has no
    //     default reduction. Error handling was described at the top of this
    //     file.

int Parser::parse()
try 
{
    push__(0);                              // initial state
    clearin();                              // clear the tokens.

    while (true)
    {
        try
        {
            if (s_state[d_state__]->d_type & REQ_TOKEN)
                nextToken();                // obtain next token


            int action = lookup(false);     // lookup d_token__ in d_state__

            if (action > 0)                 // SHIFT: push a new state
            {
                push__(action);
                popToken__();               // token processed
            }
            else if (action < 0)            // REDUCE: execute and pop.
            {
                executeAction(-action);
                                            // next token is the rule's LHS
                reduce__(s_productionInfo[-action]); 
            }
            else 
                ACCEPT();
        }
        catch (ErrorRecovery__)
        {
            errorRecovery();
        }
    }
}
catch (Return__ retValue)
{
    return retValue;
}
