#pragma once

#include <iostream>

#include "dump.hpp"


class Int
{
    private:
        int value;
        int nodeID;

    public:
        Int() = delete;

        Int(int x):
            value(x),
            nodeID(Dumper::GetDumper()->AddNode(Node::NodeType::temp))
            {};

        Int(const Int& x):
            value(x.value),
            nodeID(Dumper::GetDumper()->AddNode(Node::NodeType::temp))
            {
                Dumper::GetDumper()->AddJump(Jump::JumpType::copy, x.GetNodeID(), nodeID);
            };

        Int(Int&& x):
            value(x.value),
            nodeID(Dumper::GetDumper()->AddNode(Node::NodeType::temp))
            {
                Dumper::GetDumper()->AddJump(Jump::JumpType::move, x.GetNodeID(), nodeID);
                x.value = 0;
            }

        Int& operator=(const Int& x)
            {
                Dumper::GetDumper()->AddJump(Jump::JumpType::copy, x.GetNodeID(), nodeID);

                if(this == &x)
                    return *this;

                value = x.value;
                return *this;
            }

        Int& operator=(Int&& x)
            {
                Dumper::GetDumper()->AddJump(Jump::JumpType::move, x.GetNodeID(), nodeID);

                if(this == &x)
                    return *this;

                std::swap(value, x.value);
                return *this; 
            } 

        ~Int()
            {};

        int GetValue() const
            {
                return value;
            }

        int GetNodeID() const
            {
                return nodeID;
            }
};
