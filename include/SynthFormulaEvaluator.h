// -*-c++-*-

#include <memory>
#include <string>

namespace SynthFormulaEvaluator {

struct ParseTree
{
};

struct Parser
{
    bool checkGrammar() const;
    std::shared_ptr<ParseTree> parse( const std::string &formula ) const;
};
    
}
