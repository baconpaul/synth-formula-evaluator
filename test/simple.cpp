/*
** simple.cpp
**
** part of the synt-formula-evaluator
** copyright 2020 paul walker
**
** Released under the MIT license
*/

#include "catch2.hpp"
#include "SynthFormulaEvaluator.h"

using namespace SynthFormulaEvaluator;

TEST_CASE( "Show some Parse Trees" "[basics]" )
{
    SECTION( "Parse Trees" )
    {
        auto tc = { "", "# Hi There", "42", "let(a) = -1932.2", "out(b) = 17.23" };
        for( auto s : tc )
        {
            std::cout << "\nPARSING " << s << std::endl;
            auto p = Parser();
            p.parseTreeToStdout( s );
        }

    }
}

TEST_CASE( "Parse Simple Thigns", "[basics]" )
{
    SECTION( "Check Grammar" )
    {
        auto p = Parser();
        REQUIRE( p.checkGrammar() );
    }
    SECTION( "Empty String" )
    {
        auto p = Parser();
        auto r = p.parse( "" );
        REQUIRE( r.get() );
        std::cout << *r << std::endl;
    }

    SECTION( "Just a Coment" )
    {
        auto p = Parser();
        auto r = p.parse( "# Hey there kids" );
        REQUIRE( r.get() );
        std::cout << *r << std::endl;
    }

    SECTION( "Just a Number" )
    {
        auto p = Parser();
        auto r = p.parse( "-272.3" );
        REQUIRE( r.get() );
        std::cout << *r << std::endl;
    }

}
