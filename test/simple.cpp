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

TEST_CASE( "Show some Parse Trees", "[basics]" )
{
    SECTION( "Parse Trees" )
    {
        auto tc = { "", "# Hi There", "42", "-1932.2", "let(a) = -1932.2", "out(b) = 17.23", "let(a) = 7;",
                    "q + 2", "4 - (12.3 + 4) * 2",
                    "sin( 2 * ( t + 4 ) )",
                    "let(a) = 2;  let(b) = 7 * 2;"
                    "let(a) = 2;  let(b) = 7 * 2; out(a) = b + a",
                    "max(0.2, 0.3)"
        };
        //auto tc = { "let(a) = 1; let(b) = 2;" };
        for( auto s : tc )
        {
            std::cout << "\nPARSING " << s << std::endl;
            auto p = Parser();
            auto r = p.parse(s);
            if( r )
                p.parseTreeToStdout( *r );
        }

    }


}

