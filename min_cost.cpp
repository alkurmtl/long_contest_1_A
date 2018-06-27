#include <iostream>
#include <set>
#include <vector>
#include <iomanip>

const int infinity = 1e9;

struct Edge {
    int from, to, flow, capacity, cost, number;
};

int vertices, source, sink;
std::vector<Edge> edges;
std::vector<std::vector<int>> graph;
std::vector<int> potential;
std::vector<int> parent;
std::vector<char> used;

void add_edge(int from, int to, int capacity, int cost, int number) {
    edges.push_back({from, to, 0, capacity, cost, number});
    edges.push_back({to, from, 0, 0, -cost, number});
    graph[from].push_back(edges.size() - 2);
    graph[to].push_back(edges.size() - 1);
}

void recalculate_potentials() {
    std::set<std::pair<int, int>> distances;
    std::vector<int> new_potential(vertices, infinity);
    new_potential[source] = 0;
    parent.assign(vertices, -1);
    distances.insert({0, source});
    while (distances.size() > 0) {
        int vertex = distances.begin()->second;
        distances.erase(distances.begin());
        for (int edge_number : graph[vertex]) {
            Edge edge = edges[edge_number];
            if (edge.flow == edge.capacity) continue;
            int from = edge.from;
            int to = edge.to;
            if (new_potential[to] > new_potential[from] + edge.cost + (potential[from] - potential[to])) {
                distances.erase({new_potential[to], to});
                new_potential[to] = new_potential[from] + edge.cost + (potential[from] - potential[to]);
                distances.insert({new_potential[to], to});
                parent[to] = edge_number;
            }
        }
    }
    for (int i = 0; i < vertices; ++i) {
        potential[i] += new_potential[i];
        potential[i] = std::min(infinity, potential[i]);
    }
}

void init_potentials() {
    potential.assign(vertices, infinity);
    potential[source] = 0;
    parent.assign(vertices, -1);
    for (int phase = 0; phase < vertices; ++phase) {
        for (int edge_num = 0; edge_num < edges.size(); ++edge_num) {
            Edge edge = edges[edge_num];
            if (edge.flow == edge.capacity) continue;
            int from = edge.from;
            int to = edge.to;
            if (potential[to] > potential[from] + edge.cost) {
                potential[to] = potential[from] + edge.cost;
                parent[to] = edge_num;
            }
        }
    }
}

int add_unit_flow() {
    int current = sink;
    int cost = 0;
    while (current != source) {
        int edge_num = parent[current];
        ++edges[edge_num].flow;
        --edges[edge_num ^ 1].flow;
        cost += edges[edge_num].cost;
        current = edges[edge_num].from;
    }
    return cost;
}

std::pair<int, int> find_min_cost_flow(int _source, int _sink, int amount) {
    init_potentials();
    source = _source;
    sink = _sink;
    int flow = 0;
    int cost = 0;
    while (flow < amount && potential[sink] != infinity) {
        cost += add_unit_flow();
        ++flow;
        recalculate_potentials();
    }
    return {flow, cost};
}

void init_network(int _vertices) {
    vertices = _vertices;
    graph.assign(vertices, std::vector<int>());
}

std::vector<std::vector<int>> decomposition;
std::vector<int> path;

bool dfs(int current) {
    if (used[current]) return false;
    used[current] = true;
    if (current == sink) {
        decomposition.push_back(path);
        return true;
    }
    for (int edge_num : graph[current]) {
        Edge edge = edges[edge_num];
        if (edge.flow <= 0) continue;
        path.push_back(edge.number);
        if (dfs(edge.to)) {
            --edges[edge_num].flow;
            ++edges[edge_num ^ 1].flow;
            return true;
        }
        path.pop_back();
    }
    return false;
}

void decompose_flow() {
    do {
        used.assign(vertices, false);
        path.clear();
    } while (dfs(source));
}

using namespace std;

main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    int n, m, k;
    cin >> n >> m >> k;
    init_network(n);
    int u, v, c;
    for (int i = 0; i < m; ++i) {
        cin >> u >> v >> c;
        --u;
        --v;
        add_edge(u, v, 1, c, i);
        add_edge(v, u, 1, c, i);
    }
    pair<int, int> mfmc = find_min_cost_flow(0, n - 1, k);
    if (mfmc.first < k) {
        cout << -1;
        return 0;
    }
    cout << fixed << setprecision(16) << mfmc.second * 1.0 / k << '\n';
    decompose_flow();
    for (const auto& path : decomposition) {
        cout << path.size() << ' ';
        for (int edge_num : path) {
            cout << edge_num + 1 << ' ';
        }
        cout << '\n';
    }
}