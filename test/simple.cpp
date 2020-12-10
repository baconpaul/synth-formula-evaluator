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
     auto tc = { "", "# Hi There", "42", "-1932.2", "let(a) = -1932.2", "out(b) = 17.23", "let(a) = 7;",
                "q + 2", "4 - (12.3 + 4) * 2",
                "sin( 2 * ( t + 4 ) )",
                "let(a) = 2;  let(b) = 7 * 2;"
                "let(a) = 2;  let(b) = 7 * 2; out(a) = b + a",
                "# Hi\n",
                "13\n",
                 "# Hi\n13",
                 "# Hi\n13",

                 "# Hi\n\n13",
                 "# Hi\n\n13\n",
                 "\n# Hi\n\n13\n",
                 "13\n# Hi",
                 "13\n# Hi\n",
                 "\n",
                "\n\n",
                "\n13\n",
                 "\n13\n14",
                 "\n13\n14\n",
                 "\n\n13\n14\n",
                 "\n\n13\n14\n",
                 "\n13\n\n14\n",
                 "\n13\n14\n\n",
                 R"TC(
let(a) = 2;
let(b) = a + 1;

max(a,b)
                )TC"
                R"TC(
# This is multiline with comments

let(a) = 2;
let(b) = a + 1;

max(a,b)
                )TC"

    };
    int idx = 0;
    for( auto s : tc )
    {
        DYNAMIC_SECTION( "Parsing " << idx++ ) {
            INFO( "Input is: [" << s << "]" );
            auto p = Parser();
            auto r = p.parse(s);
            REQUIRE(r);
            if (r)
                p.parseTreeToStdout(*r);
        }
    }

}

