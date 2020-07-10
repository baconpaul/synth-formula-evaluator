#include "SynthFormulaEvaluator.h"
#include "catch2.hpp"

#include <chrono>
#include <cmath>

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
                      auto state  = SynthFormulaEvaluator::Evaluator::state_t();
                      auto res    = evalu.evaluate( env, state );

                      REQUIRE( res == f );
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
                      auto state  = SynthFormulaEvaluator::Evaluator::state_t();

                      env["x"] = x;
                      auto res    = evalu.evaluate( env, state );

                      REQUIRE( res == f );
                  };

        tp( "x", 1.0f, 1.0f );
        tp( "x+1", 1.f, 2.f );
    }

    SECTION( "Functions" )
    {
        auto tp = []( const std::string &s, float x, float f ) {
                      INFO( "Evaluating " << s << " " << x << " expexting " << f );
                      auto parser = SynthFormulaEvaluator::Parser();
                      auto tree   = parser.parse( s );

                      // parser.parseTreeToStdout( *tree );
                      
                      auto evalu  = SynthFormulaEvaluator::Evaluator( *tree );
                      auto env    = SynthFormulaEvaluator::Evaluator::environment_t( );
                      auto state  = SynthFormulaEvaluator::Evaluator::state_t();
                      env["x"] = x;
                      auto res    = evalu.evaluate( env, state );

                      REQUIRE( res == Approx(f).margin(1e-5) );
                  };
        tp( "max( x, 0.5 )", 0.2, 0.5 );
        tp( "sin(x)", 0, 0 );
        tp( "sin(x)", 3.14159265/2.0, 1.0 );
        tp( "sin(4 * x)", 3.14159265/2.0, 0.0 );

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

                      // evalu.evaluationGraphToStdout();
                      
                      auto env    = SynthFormulaEvaluator::Evaluator::environment_t( );
                      auto state  = SynthFormulaEvaluator::Evaluator::state_t();
                      float dontopt = 0;
                      auto start = std::chrono::high_resolution_clock::now();
                      int sr = 48000;
                      int se = 5;
                      float q;
                      for( int i=0; i<sr*se; ++i )
                      {
                          auto res = evalu.evaluate( env, state );
                          q += res;
                          if( i == 0 )
                          {
                              REQUIRE( res == f );
                          }
                      }
                      auto end = std::chrono::high_resolution_clock::now();
                      auto dur = end - start;
                      auto durms =  std::chrono::duration_cast<std::chrono::milliseconds>(dur);
                      std::cout << "Duration is " << durms.count() << " ms or " << durms.count() * 100.f / 1000.f / se / sr << " %" << std::endl;

                      //REQUIRE( res == f );
                  };
         tp( "1", 1 );
         tp( "2+3", 5 );
         tp( "2+3+4+5+6+7+8+9+10+11+12+13+14+15+16", 135 );
    }
}
