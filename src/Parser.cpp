#include "SynthFormulaEvaluator.h"

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>

#include <iostream>
#include <iomanip>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

namespace SynthFormulaEvaluator
{

/*
** This is purposefully NOT in the header file
*/
namespace sfe_combinators
{
    using namespace tao::pegtl;

    /*
    ** Rules
    */
    struct comment : if_must< one< '#' >, until< eolf > > {};

    struct grammar : must< comment, eof > {};

    /*
    ** State
    */
    struct state
    {
        state() {
            pt = std::make_shared<ParseTree>();
        }
        std::shared_ptr<ParseTree> pt;
    };
    
    /*
    ** Actions 
    */
    template< typename Rule > struct action : nothing<Rule> {};

    template<>
    struct action< comment >
    {
        template< typename Input >
        static void apply( const Input& in, state& s ) {
            std::cout << "Parsed a comment" << std::endl;
        }
    };
}

bool Parser::checkGrammar() const
{
    pegtl::analyze< sfe_combinators::grammar >();
    return true;
}
    
std::shared_ptr<ParseTree> Parser::parse(const std::string &formula) const {
    pegtl::string_input<> in(formula, "Provided Formula");
    sfe_combinators::state s;
    
    pegtl::parse< sfe_combinators::grammar, sfe_combinators::action >( in, s );
    return s.pt;
}
}
