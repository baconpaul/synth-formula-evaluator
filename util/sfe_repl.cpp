#include "SynthFormulaEvaluator.h"
extern "C" {
#include "linenoise.h"
}

int main( int argc, char **argv )
{
    const char* line;
    linenoiseHistorySetMaxLen( 500 );
    std::string prompt = "sfe> ";
    while((line = linenoise(prompt.c_str())) != nullptr) {
        linenoiseHistoryAdd(line);

        auto parser = SynthFormulaEvaluator::Parser();
        auto tree   = parser.parse(line);
        if( tree )
        {
            parser.parseTreeToStdout(*tree);

            try
            {
                auto eval = SynthFormulaEvaluator::Evaluator( *tree );
                auto env  = SynthFormulaEvaluator::Evaluator::environment_t();
                auto state = SynthFormulaEvaluator::Evaluator::state_t();
                
                auto r = 1.f * rand() / RAND_MAX;
                std::cout << "Creating environment where x = " << r << std::endl;
                env["x"] = r;
                auto root = eval.evaluate(env, state);
                
                std::cout << "\nRESULT=" << root << std::endl;
            }
            catch( const std::exception &e )
            {
                std::cout << "EVAL ERROR\n" << std::endl;
            }
        }
        else
        {
            std::cout << "PARSE ERROR" << std::endl;
        }
        linenoiseFree((void*)line);
    }
}
