#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

// Graph representation using adjacency list
class Graph {
public:
    int V;
    vector<set<int>> adj;

    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].insert(v);
        adj[v].insert(u);
    }

    void printGraph() {
        for (int i = 0; i < V; ++i) {
            cout << i << ": ";
            for (int neighbor : adj[i]) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
    }
};

// Tree Decomposition
class TreeDecomposition {
public:
    vector<set<int>> bags;
    map<int, vector<int>> tree;

    void addBag(set<int> bag) {
        bags.push_back(bag);
        cout << "Added bag: ";
        for (int v : bag) {
            cout << v << " ";
        }
        cout << endl;
    }

    void addTreeEdge(int u, int v) {
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    // Function to compute the treewidth
    int computeTreewidth() {
        int treewidth = 0;
        for (const auto& bag : bags) {
            cout << "Bag content: ";
            for (int v : bag) {
                cout << v << " ";
            }
            cout << " | Size: " << bag.size() << endl;
            treewidth = max(treewidth, static_cast<int>(bag.size()));
        }
        cout << "Final computed treewidth (size of largest bag - 1): " << (treewidth - 1) << endl;
        return treewidth - 1; // Treewidth is the size of the largest bag minus one
    }

    void print() {
        cout << "Tree Decomposition:" << endl;
        for (size_t i = 0; i < bags.size(); ++i) {
            cout << "Bag " << i << ": ";
            for (int v : bags[i]) {
                cout << v << " ";
            }
            cout << endl;
        }
        cout << "Tree edges:" << endl;
        for (auto &edge : tree) {
            for (int v : edge.second) {
                cout << edge.first << " - " << v << endl;
            }
        }
    }
};

// A naive function to create a simple tree decomposition (not optimal)
TreeDecomposition naiveTreeDecomposition(Graph &g) {
    TreeDecomposition td;

    for (int i = 0; i < g.V; ++i) {
        set<int> bag;
        bag.insert(i);
        for (int neighbor : g.adj[i]) {
            bag.insert(neighbor);
        }
        td.addBag(bag);
        if (i > 0) {
            td.addTreeEdge(i - 1, i);
        }
    }
    return td;
}

// Function to read the graph from a CSV file
Graph readGraphFromCSV(const string &filename) {
    ifstream file(filename);
    string line;
    set<int> vertices;
    vector<pair<int, int>> edges;

    while (getline(file, line)) {
        stringstream ss(line);
        string u_str, v_str;
        getline(ss, u_str, ',');
        getline(ss, v_str, ',');
        int u = stoi(u_str);
        int v = stoi(v_str);
        edges.emplace_back(u, v);
        vertices.insert(u);
        vertices.insert(v);
    }

    Graph g(vertices.size());
    for (const auto &edge : edges) {
        g.addEdge(edge.first, edge.second);
    }
    return g;
}

int main() {
    string filename = "graph.csv";
    Graph g = readGraphFromCSV(filename);

    cout << "Graph:" << endl;
    g.printGraph();

    TreeDecomposition td = naiveTreeDecomposition(g);
    td.print();

    int treewidth = td.computeTreewidth();
    cout << "Treewidth: " << treewidth << endl;

    return 0;
}
