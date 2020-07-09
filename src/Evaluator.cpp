#include "SynthFormulaEvaluator.h"

namespace SynthFormulaEvaluator {

struct Evaluator::Impl
{
    // FIXME - can I do this without a virtual function?
    struct Op
    {
        ParseTree::NodeTypes type;
        Op( ) : type(ParseTree::UNKNOWN) { }
        virtual ~Op() = default;
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) = 0;
        std::vector<std::unique_ptr<Op>> children;
    };

    struct Error : public Op
    {
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

    struct ApplyOp : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            float a = children[0]->eval(e,r);
            for( int i=1; i<children.size(); i+= 2 )
            {
                float b = children[i+1]->eval(e,r);
                BinOp *o = static_cast<BinOp*>(children[i].get());
                switch(o->type)
                {
                case ParseTree::PLUS:
                    a+= b;
                    break;
                case ParseTree::MINUS:
                    a-=b;
                    break;
                case ParseTree::DIVIDE:
                    a/=b;
                    break;
                case ParseTree::MULTIPLY:
                    a*=b;
                    break;
                default:
                    break;
                }
            }
            return a;
        }

    };
    
    struct Sum : public ApplyOp
    {
    };

    struct Product : public ApplyOp
    {
    };

    struct Variable : public Op {
        std::string name;
        Variable( const std::string &nm ) : Op(), name(nm) {}
        virtual float eval( const Evaluator::environment_t &e, Evaluator::result_t &r ) override {
            return e.at(name);
        }
    };
    
    struct BinOp : public Op
    {
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
        case ParseTree::IN_PARENS:
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
        case ParseTree::VARIABLE:
            op = std::make_unique<Variable>(n.contents);
            break;
        case ParseTree::PLUS:
        case ParseTree::MINUS:
        case ParseTree::MULTIPLY:
        case ParseTree::DIVIDE:
            op = std::make_unique<BinOp>();
            break;
        case ParseTree::UNKNOWN:
        default:
            op = std::make_unique<Error>();
            break;
        }
        op->type = n.type;
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
