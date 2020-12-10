// -*-c++-*-

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <unordered_map>

namespace SynthFormulaEvaluator {

struct ParseTree
{
    enum NodeTypes {
        UNKNOWN = -1,

        ROOT,

        STANDALONE_RHS,
        NUMBER,
        VARIABLE,

        SUM,
        PRODUCT,
        
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,

        UNARY_MINUS,
        IN_PARENS,

        FUNCTION_CALL,
        FUNCTION_NAME,

        ASSIGNMENT,
        ASSIGNMENT_LIST,

        LET_IDENTIFIER,
        OUT_IDENTIFIER,
        STATE_IDENTIFIER
    };

    struct Node {
        NodeTypes type = UNKNOWN;
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
    void parseTreeToStdout( const ParseTree &result ) const { parseTreeToStream( std::cout, result ); };
    void parseTreeToStream( std::ostream &os, const ParseTree &result ) const;
    void parseTreeToStreamAsDot( std::ostream &os, const ParseTree &result ) const;
};

struct Evaluator
{
    Evaluator( const ParseTree &tree );
    ~Evaluator();

    Evaluator( const Evaluator &) {};
    Evaluator& operator=( const Evaluator &) = delete;
    
    typedef std::string symbol_t;
    typedef float value_t;
    typedef std::unordered_map<symbol_t, value_t> environment_t;
    typedef std::unordered_map<symbol_t, value_t> state_t;

    void evaluationGraphToStdout() const { evaluationGraphToStream( std::cout ); }
    void evaluationGraphToStream( std::ostream &os ) const;
    void evaluationGraphToStreamAsDot( std::ostream &os ) const;

    // TODO register functions
    
    float evaluate( const environment_t &, state_t & ) const;

    struct Impl;
    struct ImplDeleter {
        /*
        ** We need this so the destructor isn't automatically constructed by the unique ptr
        ** since the header has an incomplete type and the default destructor will use sizeof
        */
        void operator()(Impl *i);
    };
    std::unique_ptr<Impl, ImplDeleter> impl;

};
}
