#include "SynthFormulaEvaluator.h"

namespace SynthFormulaEvaluator {

struct Evaluator::Impl
{
    Impl(const ParseTree &tree) {};
    Evaluator::result_t evaluate( const Evaluator::environment_t &e ) const {
        Evaluator::result_t r;
        r["ROOT"] = 17324;
        return r;
    }
        
};

Evaluator::Evaluator( const ParseTree &tree ) : impl( new Impl(tree) )
{
}
    
Evaluator::~Evaluator()
{
    delete impl;
}

const Evaluator::result_t Evaluator::evaluate( const Evaluator::environment_t &e) const {
    return impl->evaluate( e );
}
}
