#include <iostream>

#include "include/int.hpp" 
#include "include/dump.hpp"

#define SCOPE Scope scope{__PRETTY_FUNCTION__}

#define ARG(name_, type_, jumptype_) do {   Dumper::GetDumper()->AddJump(jumptype_, name_.GetNodeID(), \
                                            Dumper::GetDumper()->AddNode(type_, #name_)); } while(0)

#define VAR(name_, type_) do{   Dumper::GetDumper()->GetNode().type = type_; \
                                Dumper::GetDumper()->GetNode().name = #name_; } while(0)

Int foo(Int par_a)
{
    SCOPE;
    ARG(par_a, Node::NodeType::val, Jump::JumpType::copy);
    if(par_a.GetValue() == 0)
        return {-1};

    return foo({par_a.GetValue()-1});
}

int main()
{
    {
        SCOPE;
        Int a{4}; VAR(a, Node::NodeType::val);
        Int b = foo(a); VAR(b, Node::NodeType::val);
    }
    Dumper::GetDumper()->Dump();
    return 0;
}
