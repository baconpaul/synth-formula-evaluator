#include "SynthFormulaEvaluator.h"
#include "catch2.hpp"

TEST_CASE( "Evaluate Constants", "[eval]" )
{
    SECTION( "Numerical Expressions" )
    {
        auto tp = []( const std::string &s, float f ) {
                      INFO( "Evaluating " << s << " expexting " << f );
                      auto parser = SynthFormulaEvaluator::Parser();
                      auto tree   = parser.parse( s );

                      //parser.parseTreeToStdout( *tree );
                      
                      auto evalu  = SynthFormulaEvaluator::Evaluator( *tree );
                      auto env    = SynthFormulaEvaluator::Evaluator::environment_t( );
                      auto res    = evalu.evaluate( env );

                      REQUIRE( res["ROOT"] == f );
                  };

        tp( "13.2", 13.2f );
        tp( "2+2", 4.0 );
        tp( "8*3", 24.0 );
        tp( "10/5", 2.0 );
        tp( "(8+2)*2", 20.0);
        tp( "-3", -3.0 );
    }

    SECTION( "Single Variable" )
    {
        auto tp = []( const std::string &s, float x, float f ) {
                      INFO( "Evaluating " << s << " " << x << " expexting " << f );
                      auto parser = SynthFormulaEvaluator::Parser();
                      auto tree   = parser.parse( s );

                      parser.parseTreeToStdout( *tree );
                      
                      auto evalu  = SynthFormulaEvaluator::Evaluator( *tree );
                      auto env    = SynthFormulaEvaluator::Evaluator::environment_t( );
                      env["x"] = x;
                      auto res    = evalu.evaluate( env );

                      REQUIRE( res["ROOT"] == f );
                  };

        tp( "x", 1.0f, 1.0f );
        tp( "x+1", 1.f, 2.f );
    }
}
