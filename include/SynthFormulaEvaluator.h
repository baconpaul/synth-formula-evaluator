// -*-c++-*-

#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace SynthFormulaEvaluator {

struct ParseTree
{
    struct ParseItem {
        enum Type {
            ROOT,
            COMMENT,
            NUMBER
        } type;
        std::string svalue;
        std::vector<std::shared_ptr<ParseItem>> children;
        std::weak_ptr<ParseItem> parent;
    };

    std::shared_ptr<ParseItem> rootItem;
    std::shared_ptr<ParseItem> currentItem;
    ParseTree() {
        rootItem = std::make_shared<ParseItem>();
        rootItem->type = ParseItem::Type::ROOT;

        currentItem = rootItem;
    }
    ~ParseTree() {
    }

    std::shared_ptr<ParseItem> addChild( const ParseItem::Type &type, const std::string &svalue ) {
        auto c = std::make_shared<ParseItem>();
        c->type = type;
        c->svalue = svalue;
        c->parent = currentItem;
        currentItem->children.push_back(c);
        return c;
    }

    void push( std::shared_ptr<ParseItem> p ) {
        currentItem = p;
    }

    void pop() {
        currentItem = currentItem->parent.lock();
    }

    friend std::ostream& operator<<(std::ostream &os, const ParseTree &pt );
};

struct Parser
{
    bool checkGrammar() const;
    std::shared_ptr<ParseTree> parse( const std::string &formula ) const;
};
    
}
