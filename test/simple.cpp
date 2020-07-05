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
    }
    
}
