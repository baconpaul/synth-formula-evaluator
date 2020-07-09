#include "SynthFormulaEvaluator.h"

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <tao/pegtl/contrib/tracer.hpp>

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

    struct rhs;
    
    struct snumber : seq< opt< one< '-' > >, plus<digit>, opt< seq< string< '.' >, plus<digit> > > > {};
    struct svariable : identifier {};

    struct sfunctionname : identifier {};
    
    struct plus : pad< one< '+' >, space > {};
    struct minus : pad< one< '-' >, space > {};
    struct multiply : pad< one< '*' >, space > {};
    struct divide : pad< one< '/' >, space > {};

    struct open_bracket : seq< one< '(' >, star< space > > {};
    struct close_bracket : seq< star< space >, one< ')' > > {};

    struct in_parens : if_must< open_bracket, rhs, close_bracket > {};
    struct function_call : seq< sfunctionname, open_bracket, rhs, close_bracket > {};
    
    struct value : sor< function_call, in_parens, snumber, svariable > {};
    struct product : list_must< value, sor< multiply, divide > > {};
    struct sum : list_must< product, sor< plus, minus > > {};
    
    struct rhs : sor<sum> {};

    struct standalone_rhs : rhs {};
    
    struct let_literal : string< 'l', 'e', 't' > {};
    struct out_literal : string< 'o', 'u', 't' > {};

    struct let_identifier : identifier {};
    struct out_identifier : identifier {};

    template< typename L, typename I >
    struct target_with_identifier : seq< L, opt<space>, one<'('>, opt<space>, I, opt<space>, one<')'> > {};
        
    struct let_target : target_with_identifier< let_literal, let_identifier > {};
    struct out_target : target_with_identifier< out_literal, out_identifier > {};

    struct lhs : sor< out_target, let_target > {};
    struct assignment : seq< lhs, opt<space>, one<'='>, opt<space>, rhs > {};
    struct assignment_list : list_tail< assignment, one< ';' >, space > {};

    // struct anything : sor< assignment_list, comment > {};
    struct anything : sor< assignment_list, standalone_rhs, comment > {};
    struct grammar : until< eof, anything > {};

    template< typename Rule >
    using selector = tao::pegtl::parse_tree::selector<
        Rule,
        tao::pegtl::parse_tree::fold_one::on<
            value, product, sum
            
            >,
        tao::pegtl::parse_tree::store_content::on<
            snumber,
            svariable,
            sfunctionname,

            plus, minus, multiply, divide,

            in_parens, function_call,

            standalone_rhs,

            let_identifier, out_identifier,

            assignment,
            assignment_list
            
            > >;


}

bool Parser::checkGrammar() const
{
    pegtl::analyze< sfe_combinators::grammar >();
    return true;
}
    
    
std::unique_ptr<ParseTree::Node> fromTao( const tao::pegtl::parse_tree::node &i  )
{
    static std::unordered_map<std::type_index, ParseTree::NodeTypes> idmap;
    // WARNING: threads here I guess
    if( idmap.size() == 0 )
    {
        idmap[std::type_index(typeid(void))] = ParseTree::NodeTypes::ROOT;
        idmap[std::type_index(typeid(sfe_combinators::standalone_rhs))] = ParseTree::NodeTypes::STANDALONE_RHS;
        idmap[std::type_index(typeid(sfe_combinators::snumber))] = ParseTree::NodeTypes::NUMBER;

        idmap[std::type_index(typeid(sfe_combinators::sum))] = ParseTree::NodeTypes::SUM;
        idmap[std::type_index(typeid(sfe_combinators::product))] = ParseTree::NodeTypes::PRODUCT;
        
        idmap[std::type_index(typeid(sfe_combinators::plus))] = ParseTree::NodeTypes::PLUS;
        idmap[std::type_index(typeid(sfe_combinators::minus))] = ParseTree::NodeTypes::MINUS;
        idmap[std::type_index(typeid(sfe_combinators::multiply))] = ParseTree::NodeTypes::MULTIPLY;
        idmap[std::type_index(typeid(sfe_combinators::divide))] = ParseTree::NodeTypes::DIVIDE;
    }
    auto r = std::make_unique<ParseTree::Node>();
    r->typeName = i.name();
    if( i.has_content() )
        r->contents = i.content();

    if( idmap.find(i.id) != idmap.end() )
        r->type = idmap[i.id];
    
    for( auto &c : i.children )
    {
        r->children.push_back( fromTao( *c ) );
    }
    return r;
}
    

std::unique_ptr<ParseTree> Parser::parse(const std::string &formula) {
    //pegtl::string_input<> tin(formula, "Provided Formula");
    //pegtl::parse< sfe_combinators::grammar, pegtl::nothing, pegtl::tracer >( tin );
    
    pegtl::string_input<> in(formula, "Provided Formula");
    auto root = tao::pegtl::parse_tree::parse< sfe_combinators::grammar, sfe_combinators::selector >( in );
    if( root )
    {
        auto pt = std::make_unique<ParseTree>();
        pt->root = fromTao( *root );
        return pt;
    }
    else
    {
        return nullptr;
    }
}

void Parser::parseTreeToStdout( const ParseTree &result ) const {
    std::function<void(const ParseTree::Node &n, const std::string &pfx )> di =
        [&di](const ParseTree::Node &n, const std::string &pfx ) {
            std::cout << pfx << " " << n.typeName << " / " << n.type << " ct='" << n.contents << "'\n";
            for( auto &c : n.children )
            {
                di( *c, pfx + "--|" );
            }
        };
    di( *(result.root), "|" );
}
    
}
