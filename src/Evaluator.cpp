#include "SynthFormulaEvaluator.h"

#include <cmath>
#include <algorithm>

namespace SynthFormulaEvaluator {

struct Evaluator::Impl
{
    // FIXME - can I do this without a virtual function?
    struct Op
    {
        ParseTree::NodeTypes type;
        Op( ) : type(ParseTree::UNKNOWN) { }
        virtual ~Op() = default;
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) = 0;
        std::vector<std::unique_ptr<Op>> children;

        bool isConstant() { return false; }

        virtual std::string toString() const {
            auto res = std::string( "Op " ) + std::to_string(type);
            return res;
        }
        
        virtual bool optimizeChildren() {
            auto haveAnother = false;
            for( auto &n : children )
                haveAnother |= n->optimizeChildren();
            return haveAnother;
        }
    };

    struct Error : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) {
            std::cerr << "ERROR " << type << std::endl;
            return -123456;
        }
    };

    struct PassThru : public Op {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            // FIXME assert n.children == 1
            float res = 0;
            for( auto &n : children )
            {
                res = n->eval( e, l, r );
            }
            return res;
        }
    };
    
    struct Root : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            // Execute all the children; return the value of the last one. Populate the result along the way
            float res;
            for( auto &n : children )
            {
                res = n->eval( e, l, r );
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
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            return val;
        }
        bool isConstant() { return true; }
    };

    struct ApplyOp : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            float a = children[0]->eval(e,l,r);
            for( int i=1; i<children.size(); i+= 2 )
            {
                float b = children[i+1]->eval(e,l,r);
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

    struct UnaryMinus : public Op {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            return -1.f * children[0]->eval(e,l,r);
        }
    };
    
    struct Variable : public Op {
        std::string name;
        Variable( const std::string &nm ) : Op(), name(nm) {}
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            if( l.find( name ) != l.end() )
                return l.at(name);
            return e.at(name);
        }
    };

    // Functions with args
    template< float f() >
    struct Function0 : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            return f();
        }
    };

    template< float f(const float x) >
    struct Function1 : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            auto x = children[1]->eval(e, l, r);
            return f(x);
        }
    };

    template< const float& f(const float &x) >
    struct Function1Ref : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            auto x = children[1]->eval(e, l, r);
            return f(x);
        }
    };

    template< float f(const float x, const float y) >
    struct Function2 : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            auto x = children[1]->eval(e, l, r);
            auto y = children[2]->eval(e, l, r);
            return f(x, y);
        }
    };

    template< const float& f(const float &x, const float &y) >
    struct Function2Ref : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            auto x = children[1]->eval(e, l, r);
            auto y = children[2]->eval(e, l, r);
            return f(x, y);
        }
    };

    struct BinOp : public Op
    {
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            std::cout << "ERROR - callin geval on binop" << std::endl;
            return 0;
        }
    };

    struct Let : public Op {
        std::string var;
        Let( const std::string &v ) : Op(), var(v) {}
        virtual float eval( const Evaluator::environment_t &e, Evaluator::environment_t &l, Evaluator::state_t &r ) override {
            auto c = children[1]->eval(e,l,r);
            l[var] = c;
            return 0;
        }
    };
    
    std::unique_ptr<Op> root;
    
    Impl(const ParseTree &tree) {
        registerStdLib();
        root = buildRecursively(*(tree.root));
        auto oc = root->optimizeChildren();
        // It may take multiple passes, bcause I'm sloppy
        for( int i=0; i<10 && oc; ++i )
        {
            oc = root->optimizeChildren();
        }
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
        case ParseTree::ASSIGNMENT_LIST:
            op = std::make_unique<PassThru>();
            break;
        case ParseTree::ASSIGNMENT:
        {
            switch( n.children[0]->type ) {
            case ParseTree::LET_IDENTIFIER:
                op = std::make_unique<Let>(n.children[0]->contents);
                break;
            default:
                op = std::make_unique<Error>();
                break;
            }
            break;
        }
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
        case ParseTree::UNARY_MINUS:
            op = std::make_unique<UnaryMinus>();
            break;
        case ParseTree::FUNCTION_CALL:
        {
            // TODO have this be dynamic later
            auto fn = n.children[0]->contents;
            op = std::make_unique<Error>();
            if( std_lib.find( fn ) != std_lib.end() )
            {
                std::cout << "Found stdlib for " << fn << std::endl;
                op = std_lib[fn]();
            }
            else
            {
                std::cout << "No stdlib for " << fn << " " << std_lib.size() << std::endl;
                
            }
            break;
        }
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

    std::unordered_map<std::string, std::function<std::unique_ptr<Op>()>> std_lib;

    template< float f(float) >
    void rs1( const std::string &s ) {
        std_lib[s] = [](){ return std::make_unique<Function1<f>>(); };
    };
    template< const float& f(const float&) >
    void rs1r( const std::string &s ) {
        std_lib[s] = [](){ return std::make_unique<Function1Ref<f>>(); };
    };
    template< float f(float,float) >
    void rs2( const std::string &s ) {
        std_lib[s] = [](){ return std::make_unique<Function2<f>>(); };
    };
    template< const float& f(const float&, const float&) >
    void rs2r( const std::string &s ) {
        std_lib[s] = [](){ return std::make_unique<Function2Ref<f>>(); };
    };
    
    void registerStdLib() {
        rs1<&std::sin>("sin");
        rs1<&std::cos>("cos");
        rs1<&std::tan>("tan");
        rs1<&std::fabs>("abs");
        rs1<&std::fabs>("fabs");
        rs1<&std::floor>("floor");
        rs1<&std::ceil>("ceil");
        rs1<&std::ceil>("exp");
        rs1<&std::ceil>("log");
        rs1<&std::ceil>("log2");

        rs2r<&std::max<float>>("max");
        rs2<&std::powf>("pow");

        std::cout << "Register StdLib results in " << std_lib.size() << std::endl;
    }
    
    float evaluate( const Evaluator::environment_t &e, Evaluator::state_t &s ) const {
        auto localEnv = Evaluator::environment_t();
        auto r = root->eval( e, localEnv, s );
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
    
float Evaluator::evaluate( const Evaluator::environment_t &e, Evaluator::state_t &s) const {
    return impl->evaluate( e, s );
}

void Evaluator::evaluationGraphToStream( std::ostream &os ) const
{
    std::function<void(const Evaluator::Impl::Op *n, const std::string &pfx )> di =
        [&di,&os](const Evaluator::Impl::Op *n, const std::string &pfx ) {
            os << pfx << " " << n->toString() << "\n";
            for( auto &c : n->children )
            {
                di( c.get(), pfx + "--|" );
            }
        };
    di( impl->root.get(), "|" );
}

}
