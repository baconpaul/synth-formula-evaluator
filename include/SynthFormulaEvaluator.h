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
    // I really wanted to make this a unique pointer but can't because I get a template instantiation error when
    // used from outside the library on macos. So make this no-copy instead
    Impl *impl;

};
}
