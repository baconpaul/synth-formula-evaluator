// -*-c++-*-

#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <iostream>

namespace SynthFormulaEvaluator {

struct ParseTree
{
    enum NodeTypes {
    };

    struct Node {
        NodeTypes type;
        std::string typeName;
        std::string contents;
        std::vector<std::unique_ptr<Node>> children;
    };

    std::unique_ptr<Node> root;
};
struct Parser
{
    bool checkGrammar() const;
    std::unique_ptr<ParseTree> parse( const std::string &formula );
    void parseTreeToStdout( const ParseTree &result ) const;
};
    
}
