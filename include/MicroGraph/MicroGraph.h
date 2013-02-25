#ifndef MICRO_GRAPH_H
#define MICRO_GRAPH_H

#include <map>
#include <string>
#include <vector>
#include <fstream>

template <class tVL, class tVP, class tEP>
class MicroGraph {

public:
    typedef tVL tVertex;
    typedef tVP tVertexProperty;
    typedef tEP tEdgeProperty;

    typedef typename std::map<tVertex, tVertexProperty> tVertices;
    typedef typename tVertices::iterator tVertexIterator;

    typedef typename std::multimap<tVertex, tVertex> tEdges;
    typedef typename tEdges::iterator tEdgeIterator;

    typedef typename std::pair<tVertex, tVertex> tEdge;
    typedef typename std::map<tEdge, tEdgeProperty> tEdgeProperties;

private:
    tVertices vertices;
    tEdges edges;

    tEdgeProperties edgeProperties;

public:
    tVertexIterator vertex_begin() { return vertices.begin(); }
    tVertexIterator vertex_end()   { return vertices.end();   }

    tEdgeIterator   edge_begin()   { return edges.begin();    }
    tEdgeIterator   edge_end()     { return edges.end();      }

    std::pair<tVertexIterator, bool> addVertex(tVertex v) {
        // Add vertex
        return vertices.insert(std::make_pair(v, tVertexProperty()));
    }

    // Add a copy of a vertex
    std::pair<tVertexIterator, bool> addVertex(tVertexIterator it) {
        return vertices.insert(std::make_pair(it->first, it->second));
    }

    void removeVertex(tVertexIterator it) {

        // Remove all edges
        for (auto e = edges.begin(); e != edges.end(); ++e) {
            if (e->first == it->first || e->second == it->first) {
                auto edge = std::make_pair(e->first, e->second);
                edgeProperties.erase(edge);
                edges.erase(e);
            }
        }

        // Remove the vertex
        vertices.erase(it);
    }

    tEdgeIterator addEdge(tVertexIterator v1, tVertexIterator v2) {
        tEdge edge(v1->first, v2->first);

        // Only a single edge between same vertices is allowed
        //auto it = edges.find(edge.first);
        for (auto it = edges.lower_bound(edge.first); it != edges.upper_bound(edge.first); ++it) {
            if (it->second == edge.second) {
                return it;
            }
        }

        return edges.insert(std::pair<tVertex, tVertex>(v1->first, v2->first));

    }

    std::pair<tEdgeIterator, tEdgeIterator> outEdges(tVertex v) {
        return std::make_pair(
            edges.lower_bound(v), edges.upper_bound(v)
        );
    }

    std::pair<tEdgeIterator, tEdgeIterator> outEdges(tVertexIterator it) {
        return outEdges(it->first);
    }

    unsigned int outDegree(tVertex v) {
        auto out_edges = outEdges(v);
        return std::distance(out_edges.first, out_edges.second);
    }

    unsigned int outDegree(tVertexIterator it) { return outDegree(it->first); }

    std::vector<tEdgeIterator> inEdges(tVertex v) {

        std::vector<tEdgeIterator> in_edges;

        for (auto e = edges.begin(); e != edges.end(); ++e) {
            if (e->second == v) {
                in_edges.push_back(e);
            }
        }

        return in_edges;
    }

    std::vector<tEdgeIterator> inEdges(tVertexIterator it) {
        return inEdges(it->first);
    }

    unsigned int inDegree(tVertex v) { return inEdges(v).size(); }
    unsigned int inDegree(tVertexIterator it) { return inDegree(it->first); }

    // Get the target vertex of an edge
    tVertexIterator target(tEdgeIterator it) { return vertices.find(it->second); }
    tVertexIterator source(tEdgeIterator it) { return vertices.find(it->first);  }

    tVertexIterator operator[](tVertex v) {
        return vertices.find(v);
    }

    //tEdgeProperty& operator[](tVertexIterator it) { return edgeProperties[it->first]; }
    tEdgeProperty& operator[](tEdgeIterator it) {
        tEdge edge(it->first, it->second);
        return edgeProperties[edge];
    }

    tEdgeProperty& operator[](tEdge e) {
        return edgeProperties[e];
    }

};

template <class G>
class MicroGraphWriter {

private:
    G g;
    std::string file;

public:
    MicroGraphWriter(std::string _f, G &_g) {
        g = _g;
        file = _f;
    }

    void writeGraphToFile() {

        std::ofstream f(file);

        // Generate local vertex IDs
        std::map<typename G::tVertex, unsigned long> vertexIDs;
        unsigned long ID = 0;
        for (typename G::tVertexIterator it = g.vertex_begin(); it != g.vertex_end(); ++it) {
            vertexIDs.insert(std::make_pair(it->first, ID));
            ID++;
        }

        f << "digraph {\n";

        // Write graph header
        f << "    rankdir=LR;\n"
          //<< "    splines=ortho;\n"
          //<< "    compound=false;\n"
          //<< "    ranksep=equally;\n"
          << "    node [shape=rectangle style=\"filled\" color=\"gray80\" fillcolor=\"gray80\"];\n\n";

        // Write all vertices
        for (typename G::tVertexIterator it = g.vertex_begin(); it != g.vertex_end(); ++it) {
            auto vertex_id = vertexIDs.find(it->first)->second;
            auto vertex_properties = it->second;

            f << "    " << vertex_id << " [label=\"" << vertex_properties.name << "\"]\n";
        }

        f << "\n";

        // Write all edges
        for (typename G::tEdgeIterator it = g.edge_begin(); it != g.edge_end(); ++it) {

            auto vertex_id1 = vertexIDs.find(it->first)->second;
            auto vertex_id2 = vertexIDs.find(it->second)->second;
            auto edge_properties = g[typename G::tEdge(it->first, it->second)];

            f << "    " << vertex_id1 << " -> " << vertex_id2
                        << " [label=\"" << edge_properties.name << "\"";

            if (edge_properties.ctrl) {
                f << " style=\"dashed\"";
            }

            f << "]\n";

        }

        f << "}\n";

        f.close();

    }
};

#endif /* MICRO_GRAPH_H */
