#include "SynthFormulaEvaluator.h"

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>

#include <iostream>
#include <iomanip>
#include <unordered_map>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

namespace SynthFormulaEvaluator
{

/*
** This is purposefully NOT in the header file
*/
namespace sfe_combinators
{
    using namespace tao::pegtl;


    struct comment : if_must< one<'#'>, until<eolf>> {};
    struct sfe_number : seq< opt< one< '+', '-' > >, plus<digit>, opt< seq< string< '.' >, plus<digit> > > > {};
    struct anything : sor< comment, sfe_number, plus<space> > {};
        
    struct grammar : until< eof, anything > {};
    
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
            s.pt->addChild( ParseTree::ParseItem::Type::COMMENT, in.string() );
        }
    };

    template<>
    struct action< sfe_number >
    {
        template< typename Input >
        static void apply( const Input& in, state& s ) {
            s.pt->addChild( ParseTree::ParseItem::Type::NUMBER, in.string() );
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

std::ostream& operator<<( std::ostream &os, const ParseTree &pt )
{
    os << "Parse Tree\n";

    auto em = std::unordered_map<ParseTree::ParseItem::Type, std::string>();
    em[ParseTree::ParseItem::ROOT] = "ROOT";
    em[ParseTree::ParseItem::COMMENT] = "COMMENT";
    em[ParseTree::ParseItem::NUMBER] = "NUMBER";
    
    std::function<void(const std::shared_ptr<ParseTree::ParseItem> &, const std::string &pfx)> di =
        [&di, &os, &em](const std::shared_ptr<ParseTree::ParseItem> &i, const std::string pfx ) {
            auto tv = std::string( "UNKNOWN" );
            if( em.find( i->type ) != em.end() )
            {
                tv = em[i->type];
            }
            os << pfx << " " << tv << " [" << i->type << "] => '" << i->svalue << "'\n";
            for( auto c : i->children )
            {
                di( c, pfx + "--|" );
            }
        };
    di( pt.rootItem, "|" );
    return os;
}
}
