#include "SynthFormulaEvaluator.h"
#include "catch2.hpp"

TEST_CASE( "Evaluate Constants", "[eval]" )
{
    SECTION( "Integers" )
    {
        auto tp = []( const std::string &s, float f ) {
                      auto parser = SynthFormulaEvaluator::Parser();
                      auto tree   = parser.parse( s );

                      parser.parseTreeToStdout( *tree );
                      
                      auto evalu  = SynthFormulaEvaluator::Evaluator( *tree );
                      auto env    = SynthFormulaEvaluator::Evaluator::environment_t( );
                      auto res    = evalu.evaluate( env );

                      REQUIRE( res["ROOT"] == f );
                  };

        tp( "13.2", 13.2f );
        tp( "2+2", 4.0 );
        tp( "8*3", 24.0 );
    }
}
