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

        IN_PARENS,

        
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
    void parseTreeToStdout( const ParseTree &result ) const;
};

struct Evaluator
{
    Evaluator( const ParseTree &tree );
    ~Evaluator();

    Evaluator( const Evaluator &) = delete;
    Evaluator& operator=( const Evaluator &) = delete;
    
    typedef std::string symbol_t;
    typedef float value_t;
    typedef std::unordered_map<symbol_t, value_t> environment_t;
    typedef std::unordered_map<symbol_t, value_t> result_t;

    // TODO register functions
    
    const result_t evaluate( const environment_t & ) const;

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
