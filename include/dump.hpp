#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>

using std::string;
using std::ofstream;
using std::vector;
using std::cout;
using std::endl;

#ifdef DEBUG
#define $$ fprintf(stderr, "%d\n", __LINE__);
#else
#define $$
#endif

class Dumper;

struct Node
{
    public:

        enum NodeType
        {
            temp,
            val,
            def,
        };

        string name;
        NodeType type;

        Node(NodeType type_, string name_ = ""):
            name(name_), type(type_) {};
};

struct Jump
{
    public:

        enum JumpType
        {
            copy,
            move,
            def,
        };

        JumpType type;
        int from;
        int to;

        Jump(JumpType type_, int from_, int to_):
            type(type_), from(from_), to(to_) {};
};


struct Call
{
    public:

        vector<int> nodesIDs;
        vector<int> subcallsIDs;
        string ident;

        Call(string ident_):
            ident(ident_) {}
};


class Dumper
{
    private:

        ofstream fout;

        vector<Node> nodes;
        vector<Jump> jumps;
        vector<Call> calls;
        vector<int> callVector;

        Dumper(const string& file)
            {$$
                fout.open(file);
                calls.push_back({"MyDump"});
                callVector.push_back(0);
            }

        void MakeIndent(int change)
            {$$
                static int nIndents = 0;
                nIndents += change;
                fout << endl;
                for(int i = 0; i < nIndents; i++)
                    fout << "\t";
            }

    public:

        Dumper() = delete;

        Dumper(const Dumper& ) = delete;

        Dumper& operator=(const Dumper& ) = delete;

        Dumper(Dumper&& ) = delete;

        Dumper& operator=(Dumper&& ) = delete;

        ~Dumper()
            {$$
                fout.close();
            }

        static Dumper* GetDumper()
            {$$
                static std::unique_ptr<Dumper> dumper = nullptr;;

                if(!dumper)
                    dumper.reset(new Dumper("test.dot"));

                return dumper.get();
            }

        Node& GetNode()
            {$$
                return nodes.back();
            }

        void StartCall(const string& ident)
            {$$
                callVector.push_back(AddCall(ident));$$
            }

        void EndCall()
            {$$
                callVector.pop_back();
            }

        int AddCall(const string& ident)
            {$$
                int childCallID = calls.size();
                int parentCallID = callVector.back();

                calls.push_back({ident});
                calls[parentCallID].subcallsIDs.push_back(childCallID);
                return childCallID;
            }

        int AddNode(Node::NodeType type, const string& ident = "")
            {$$
                int nodeID = nodes.size();
                if(ident == "")
                {
                    nodes.push_back({type, "temp" + std::to_string(nodeID)});
                }
                else
                {
                    nodes.push_back({type, ident});
                }

                calls[callVector.back()].nodesIDs.push_back(nodeID);
                return nodeID;
            }

        int AddJump(Jump::JumpType type, int from, int to)
            {$$
                jumps.push_back({type, from, to});
                return jumps.size() - 1;
            }

        void DumpNode(int id)
            {$$
                const Node& node = nodes[id];
                fout << "\"%" << id << "\"[label=\"" << node.name.c_str() << "\", ";
                switch (node.type)
                {
                case Node::NodeType::temp:
                    fout << "color = \"red\"]";
                    break;
                case Node::NodeType::val:
                    fout << "color = \"blue\"]";
                    break;
                case Node::NodeType::def:
                default:
                    fout << "color = \"grey\"]";
                    break;
                }
                MakeIndent(0);
            }

        void DumpJump(int id)
            {
                const Jump& jump = jumps[id];
                fout << "\"%" << jump.from << "\" -> \"%" << jump.to << "\"";
                switch (jump.type)
                {
                    case Jump::JumpType::copy:
                        fout << "[label=\"copy\", color=green]";
                        break;
                    
                    case Jump::JumpType::move:
                        fout << "[label=\"move\", color=blue]";
                        break;

                    case Jump::JumpType::def:
                    default:
                        fout << "[style=\"dashed\"]";
                        break;
                }
                MakeIndent(0);
            }

        void DumpCallData(const Call& call)
            {$$
                for(unsigned long i = 0; i < call.nodesIDs.size(); i++)
                    DumpNode(call.nodesIDs[i]);

                for(unsigned long i = 0; i < call.subcallsIDs.size(); i++)
                    DumpCall(call.subcallsIDs[i]);
            }

        void DumpCall(int callID)
            {$$
                const Call& call = calls[callID];
                fout << "subgraph cluster_" << callID;
                MakeIndent(0);
                fout << "{";
                MakeIndent(1);$$
                fout << "style = \"rounded\"label=\"" << call.ident.c_str() << "\"";$$
                MakeIndent(0);
                DumpCallData(call);$$
                MakeIndent(-1);$$
                fout << "}";
                MakeIndent(0);
            }

        void Dump()
            {$$
                const Call& call = calls[0];
                fout << "digraph \"" << call.ident.c_str() << "\"";
                MakeIndent(0);
                fout << "{";
                MakeIndent(1);
                fout << "fontname=\"Courier New\"";
                MakeIndent(0);
                fout << "edge [fontname=\"Courier New\"]";
                MakeIndent(0);
                fout << R"(node [penwidth = 2, shape = box, fillcolor = white, style = "rounded, filled", fontname = "Courier"])";
                MakeIndent(0);
                DumpCallData(call);
                MakeIndent(0);
                for(unsigned long i = 0; i < jumps.size(); i++)
                    DumpJump(i);
                MakeIndent(-1);
                fout << "}";
            }
};


struct Scope
{
    public:

        Scope(const string& ident)
            {$$
                Dumper::GetDumper()->StartCall(ident);
            }

        ~Scope()
            {$$
                Dumper::GetDumper()->EndCall();
            }
};
