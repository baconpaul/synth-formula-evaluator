#include "SynthFormulaEvaluator.h"
#include "catch2.hpp"

#include <chrono>

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
        tp( "2+3+4", 9.0 );
        tp( "2+4*2+3+4*3*2", 37.0 );
    }

    SECTION( "Single Variable" )
    {
        auto tp = []( const std::string &s, float x, float f ) {
                      INFO( "Evaluating " << s << " " << x << " expexting " << f );
                      auto parser = SynthFormulaEvaluator::Parser();
                      auto tree   = parser.parse( s );

                      //parser.parseTreeToStdout( *tree );
                      
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

TEST_CASE( "Performance", "[eval]" )
{
    SECTION( "Math" )
    {
         auto tp = []( const std::string &s, float f ) {
                      INFO( "Evaluating " << s << " expexting " << f );
                      auto parser = SynthFormulaEvaluator::Parser();
                      auto tree   = parser.parse( s );

                      //parser.parseTreeToStdout( *tree );
                      
                      auto evalu  = SynthFormulaEvaluator::Evaluator( *tree );
                      auto env    = SynthFormulaEvaluator::Evaluator::environment_t( );
                      float dontopt = 0;
                      auto start = std::chrono::high_resolution_clock::now();
                      int sr = 48000;
                      int se = 5;
                      float q;
                      for( int i=0; i<sr*se; ++i )
                      {
                          auto res = evalu.evaluate( env );
                          q += res["ROOT"];
                          if( i == 0 )
                          {
                              REQUIRE( res["ROOT"] == f );
                          }
                      }
                      auto end = std::chrono::high_resolution_clock::now();
                      auto dur = end - start;
                      auto durms =  std::chrono::duration_cast<std::chrono::milliseconds>(dur);
                      std::cout << "Duration is " << durms.count() << " ms or " << durms.count() * 100.f / 1000.f / se / sr << " %" << std::endl;

                      //REQUIRE( res["ROOT"] == f );
                  };
         tp( "1", 1 );
         tp( "2+3", 5 );
         tp( "2+3+4+5+6+7+8+9+10+11+12+13+14+15+16", 135 );
    }
}
