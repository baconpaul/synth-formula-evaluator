#include "SynthFormulaEvaluator.h"

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

namespace SynthFormulaEvaluator
{

/*
** This is purposefully NOT in the header file
*/
namespace sfe_combinators
{
    using namespace tao::pegtl;
    
    struct comment : if_must< one< '#' >, until< eolf > > {};

    struct grammar : must< comment, eof > {};

    template< typename Rule > struct action {};
}

bool Parser::checkGrammar() const
{
    pegtl::analyze< sfe_combinators::grammar >();
    return true;
}
    
std::shared_ptr<ParseTree> Parser::parse(const std::string &formula) const {

    return nullptr;
}
}
