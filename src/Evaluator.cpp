#include "SynthFormulaEvaluator.h"

namespace SynthFormulaEvaluator {

struct Evaluator::Impl
{
    // FIXME - can I do this without a virtual function?
    struct Op
    {
        virtual ~Op() = default;
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) = 0;
        std::vector<std::unique_ptr<Op>> children;
    };

    struct Error : public Op
    {
        ParseTree::NodeTypes type;
        Error( ParseTree::NodeTypes t ) : type(t) { }
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) {
            std::cerr << "ERROR " << type << std::endl;
            return -123456;
        }
    };

    struct PassThru : public Op {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            // FIXME assert n.children == 1
            float res = 0;
            for( auto &n : children )
            {
                res = n->eval( e, r );
            }
            return res;
        }
    };
    
    struct Root : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            // Execute all the children; return the value of the last one. Populate the result along the way
            float res;
            for( auto &n : children )
            {
                res = n->eval( e, r );
            }
            return res;
        }
    };

    struct Number : public Op
    {
        float val;
        Number(const std::string &s) {
            // FIXME locales
            val = std::atof(s.c_str());
        }
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            return val;
        }
    };

    struct ApplyBinOp : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            float a = children[0]->eval(e,r);
            float b = children[2]->eval(e,r);
            BinOp *o = static_cast<BinOp*>(children[1].get());
            switch(o->type)
            {
            case ParseTree::PLUS:
                return a+b;
            case ParseTree::MINUS:
                return a-b;
            case ParseTree::DIVIDE:
                return a/b;
            case ParseTree::MULTIPLY:
                return a*b;
            default:
                return 0;
            }
        }

    };
    
    struct Sum : public ApplyBinOp
    {
    };

    struct Product : public ApplyBinOp
    {
    };
    
    struct BinOp : public Op
    {
        ParseTree::NodeTypes type;
        BinOp( const ParseTree::NodeTypes t ) : type(t) {}
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            std::cout << "ERROR - callin geval on binop" << std::endl;
            return 0;
        }
    };

    std::unique_ptr<Op> root;
    
    Impl(const ParseTree &tree) {
        root = buildRecursively(*(tree.root));
    };

    std::unique_ptr<Op> buildRecursively( const ParseTree::Node &n ) {
        std::unique_ptr<Op> op;
        switch( n.type )
        {
        case ParseTree::ROOT:
            op = std::make_unique<Root>();
            break;
        case ParseTree::STANDALONE_RHS:
            op = std::make_unique<PassThru>();
            break;
        case ParseTree::NUMBER:
            op = std::make_unique<Number>(n.contents);
            break;
        case ParseTree::SUM:
            op = std::make_unique<Sum>();
            break;
        case ParseTree::PRODUCT:
            op = std::make_unique<Product>();
            break;
        case ParseTree::PLUS:
        case ParseTree::MINUS:
        case ParseTree::MULTIPLY:
        case ParseTree::DIVIDE:
            op = std::make_unique<BinOp>(n.type);
            break;
        case ParseTree::UNKNOWN:
        default:
            op = std::make_unique<Error>(n.type);
            break;
        }
        for( auto &c : n.children )
        {
            op->children.push_back( buildRecursively(*c) );
        }
        return op;
    }
    
    Evaluator::result_t evaluate( const Evaluator::environment_t &e ) const {
        Evaluator::result_t r;
        r["ROOT"] = root->eval( e, r );
        return r;
    }
        
};

void Evaluator::ImplDeleter::operator()(Impl *d)
{
    delete d;
}
    
Evaluator::Evaluator( const ParseTree &tree ) 
{
    impl.reset( new Impl(tree) );
}

Evaluator::~Evaluator() {}
    
const Evaluator::result_t Evaluator::evaluate( const Evaluator::environment_t &e) const {
    return impl->evaluate( e );
}

    
}
