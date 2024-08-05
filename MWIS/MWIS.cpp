#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

using namespace std;

const int INF = 1e9;

// Hash function for vector<int>
struct VectorHash {
    size_t operator()(const vector<int>& v) const {
        size_t seed = v.size();
        for (auto& i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

// Function to check if a subset is an independent set in a given bag
bool isIndependentSet(const set<int>& subset, const unordered_map<int, unordered_set<int>>& adj) {
    for (int v : subset) {
        for (int u : subset) {
            if (v != u && adj.at(v).count(u)) {
                return false;
            }
        }
    }
    return true;
}

// Read graph from CSV file
void readGraph(const string& filename, unordered_map<int, unordered_set<int>>& adj) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int u, v;
        char comma;
        if (iss >> u >> comma >> v) {
            adj[u].insert(v);
            adj[v].insert(u); // For undirected graph
        }
    }
    cout << "Graph read successfully." << endl;
}

// Read tree decomposition from CSV file
void readTreeDecomposition(const string& filename, unordered_map<int, vector<int>>& bags, unordered_map<int, vector<int>>& parentBag) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int node;
        char comma;
        string bagStr;
        if (getline(iss, bagStr, ',')) {
            node = stoi(bagStr);
            vector<int> bag;
            while (getline(iss, bagStr, ' ')) {
                if (!bagStr.empty()) {
                    bag.push_back(stoi(bagStr));
                }
            }
            bags[node] = bag;
        }
    }
    cout << "Tree decomposition read successfully." << endl;
}

// Read vertex weights from CSV file
void readVertexWeights(const string& filename, unordered_map<int, int>& weights) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int vertex, weight;
        char comma;
        if (iss >> vertex >> comma >> weight) {
            weights[vertex] = weight;
        }
    }
    cout << "Vertex weights read successfully." << endl;
}

// Dynamic programming function to solve the maximum weighted independent set
int dp(int u, const vector<int>& bag, const unordered_map<int, unordered_set<int>>& adj, 
       unordered_map<int, unordered_map<vector<int>, int, VectorHash>>& memo, unordered_map<int, vector<int>>& parentBag, const unordered_map<int, vector<int>>& bags, const unordered_map<int, int>& weights) {

    // Convert vector to a sorted vector for memoization
    vector<int> sortedBag = bag;
    sort(sortedBag.begin(), sortedBag.end());
    
    if (memo[u].find(sortedBag) != memo[u].end()) {
        return memo[u][sortedBag];
    }

    int maxWeight = 0;

    // Iterate over all subsets of the bag
    int n = bag.size();
    int numSubsets = 1 << n;

    for (int mask = 0; mask < numSubsets; ++mask) {
        set<int> subset;
        int weightSum = 0;
        bool valid = true;

        // Build the subset and compute its weight
        for (int i = 0; i < n; ++i) {
            if (mask & (1 << i)) {
                subset.insert(bag[i]);
                weightSum += weights.at(bag[i]); // Use actual weight from weights map
            }
        }

        if (isIndependentSet(subset, adj)) {
            int currentWeight = weightSum;

            // Add weight of the independent set and check children
            for (int v : subset) {
                if (parentBag.find(v) != parentBag.end()) {
                    for (int child : parentBag[v]) {
                        currentWeight += dp(child, bags.at(child), adj, memo, parentBag, bags, weights);
                    }
                }
            }

            maxWeight = max(maxWeight, currentWeight);
        }
    }

    memo[u][sortedBag] = maxWeight;
    return maxWeight;
}

int main() {
    unordered_map<int, unordered_set<int>> adj;
    unordered_map<int, vector<int>> bags;
    unordered_map<int, vector<int>> parentBag;
    unordered_map<int, int> weights;

    // Read graph, tree decomposition, and vertex weights
    readGraph("graph.csv", adj);
    readTreeDecomposition("tree_decomposition.csv", bags, parentBag);
    readVertexWeights("vertex_weights.csv", weights);

    unordered_map<int, unordered_map<vector<int>, int, VectorHash>> memo;

    // Assuming root is 0; 
    int root = 0;
    int result = dp(root, bags[root], adj, memo, parentBag, bags, weights);
    cout << "Maximum Weight Independent Set: " << result << endl;

    return 0;
}
