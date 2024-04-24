#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <memory>
#include <algorithm>
#include <unordered_set>

struct Transition {
    std::string from;
    std::string input;
    std::string to;

    Transition(std::string from, std::string input, std::string to) :
                from(std::move(from)), input(std::move(input)), to(std::move(to)) {}
};

struct FSA {
    std::string type;
    std::vector<std::string> states;
    std::vector<std::string> alphabet;
    std::string initial;
    std::vector<std::string> accepting;
    std::vector<Transition> transitions;


    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;

        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    bool checkState(const std::string& state) {
        for (const auto& c : state) {
            if (!(isalpha(c) || isdigit(c))) {
                return false;
            }
        }
        return true;
    }

    bool checkLetter(const std::string& letter) {
        for (const auto& c : letter) {
            if (!(isalpha(c) || isdigit(c) || c == '_')) {
                return false;
            }
        }
        return true;
    }

    bool containsState(const std::string& state) {
        return std::find(states.begin(), states.end(), state) != states.end();
    }

    bool containsLetter(const std::string& letter) {
        return std::find(alphabet.begin(), alphabet.end(), letter) != alphabet.end();
    }

    std::string initialization() {
        std::string input;
        while (std::cin >> input) {
            std::vector<std::string> tokens = split(input, '=');
            if (tokens.size() != 2) {
                return "E1: Input file is malformed";
            }
            std::string key = tokens[0], value = tokens[1];
            if (key == "type") {
                type = value.substr(1, value.size() - 2);
                if (!(type == "deterministic" || type == "non-deterministic")) {
                    return "E1: Input file is malformed";
                }
            } else if (key == "states") {
                std::string s = value.substr(1, value.size() - 2);
                states = split(s, ',');
                for (const auto& state : states) {
                    if (!checkState(state)) {
                        return "E1: Input file is malformed";
                    }
                }
            } else if (key == "alphabet") {
                std::string s = value.substr(1, value.size() - 2);
                alphabet = split(s, ',');
                for (const auto& letter : alphabet) {
                    if (!checkLetter(letter)) {
                        return "E1: Input file is malformed";
                    }
                }
            } else if (key == "initial") {
                initial = value.substr(1, value.size() - 2);
                if (!containsState(initial)) {
                    return "E2: Initial state is not defined";
                }
            } else if (key == "accepting") {
                std::string s = value.substr(1, value.size() - 2);
                accepting = split(s, ',');
                if (accepting.empty()) {
                    return "E3: Set of accepting states is empty";
                }
                for (const auto& state : accepting) {
                    if (!containsState(state)) {
                        return "E4: A state " + state + " is not in the set of states";
                    }
                }
            } else if (key == "transitions") {
                std::string s = value.substr(1, value.size() - 2);
                std::vector<std::string> intermediateTransitions = split(s, ',');
                for (const auto& intermediate : intermediateTransitions) {
                    std::vector<std::string> transitionTokens = split(intermediate, '>');
                    if (transitionTokens.size() != 3) {
                        return "E1: Input file is malformed";
                    }
                    std::string from = transitionTokens[0], delta = transitionTokens[1],
                                    to = transitionTokens[2];
                    if (!containsState(from)) {
                        return "E4: A state " + from + " is not in the set of states";
                    } else if (!containsState(to)) {
                        return "E4: A state " + to + " is not in the set of states";
                    } else if (!containsLetter(delta)) {
                        return "E5: A transition " + delta + " is not represented in the alphabet";
                    }
                    Transition transition(from, delta, to);
                    transitions.push_back(transition);
                }
            } else {
                return "E1: Input file is malformed";
            }
        }
        return "Success";
    }

    void dfs(const std::string& v, std::unordered_map<std::string, bool>& visited,
                const std::unordered_map<std::string, std::vector<std::string>>& graph) {
        visited[v] = true;
        for (const auto& u : graph.at(v)) {
            if (visited.count(u) && !visited[u]) {
                dfs(u, visited, graph);
            }
        }
    }

    bool checkDisjointness() {
        std::unordered_map<std::string, bool> visited;
        std::unordered_map<std::string, std::vector<std::string>> graph;
        for (const auto& transition : transitions) {
            graph[transition.from].push_back(transition.to);
            visited[transition.from] = false;
        }

        dfs(initial, visited, graph);

        for (const auto& v : visited) {
            if (!v.second) {
                return true;
            }
        }

        return false;
    }

    bool isNonDeterministic() {
        std::unordered_map<std::string, std::unordered_set<std::string>> transitionsData;

        for (const auto& transition : transitions) {
            const std::string& from = transition.from;
            const std::string& to = transition.to;

            if (from == to) {
                return true;
            }

            if (transitionsData[from].count(to)) {
                return true;
            }

            transitionsData[from].insert(to);
        }

        return false;
    }

    void kleenesAlgorithm() {
        std::vector<std::vector<std::string>> initR(
                states.size(),std::vector<std::string>(states.size(), ""));
        std::vector<std::vector<std::vector<std::string>>> R(
                states.size(), std::vector<std::vector<std::string>>(states.size(),
                        std::vector<std::string>(states.size(), "")));
        std::string answer = "";
        for (size_t i = 0; i < transitions.size(); ++i) {
            size_t from, to;
            for (size_t j = 0; j < states.size(); ++j) {
                if (states[j] == transitions[i].from) {
                    from = j;
                    break;
                }
            }

            for (size_t j = 0; j < states.size(); ++j) {
                if (states[j] == transitions[i].to) {
                    to = j;
                    break;
                }
            }

            if (!initR[from][to].empty()) {
                initR[from][to] += "|" + transitions[i].input;
            } else {
                initR[from][to] = transitions[i].input;
            }
        }

        for (size_t i = 0; i < states.size(); ++i) {
            if (initR[i][i].empty()) {
                initR[i][i] = "eps";
            } else {
                initR[i][i] += "|eps";
            }

            for (size_t j = 0; j < states.size(); ++j) {
                if (i != j && initR[i][j].empty()) {
                    initR[i][j] = "{}";
                }
            }
        }

        for (size_t k = 0; k < states.size(); ++k) {
            for (size_t i = 0; i < states.size(); ++i) {
                for (size_t j = 0; j < states.size(); ++j) {
                    if (k != 0) {
                        R[i][j][k] = "(" + R[i][k][k - 1] + ")(" + R[k][k][k - 1] + ")*(" +
                                R[k][j][k - 1] + ")|(" + R[i][j][k - 1] + ")";
                    } else {
                        R[i][j][k] = "((" + initR[i][k] + ")(" + initR[k][k] + ")*(" +
                                     initR[k][j] + ")|(" + initR[i][j] + "))";
                    }
                }
            }
        }

        size_t initialIndex;
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i] == initial) {
                initialIndex = i;
                break;
            }
        }

        for (size_t i = 0; i < accepting.size(); ++i) {
            size_t index;

            for (size_t j = 0; j < states.size(); ++j) {
                if (states[j] == accepting[i]) {
                    index = j;
                }
            }

            if (answer.empty()) {
                answer = R[initialIndex][index][states.size() - 1];
            } else {
                answer += "|" + R[initialIndex][index][states.size() - 1];
            }
        }

        if (answer.empty()) {
            answer = "{}";
        }

        std::cout << answer;
    }
};

int main() {
    std::freopen("input.txt", "r", stdin);
    std::unique_ptr<FSA> fsa = std::make_unique<FSA>();
    std::string initializationResult = fsa->initialization();
    if (!(initializationResult == "Success")) {
        std::cout << initializationResult;
        return 0;
    }

    if (fsa->checkDisjointness()) {
        std::cout << "E6: Some states are disjoint";
        return 0;
    }

    if(fsa->isNonDeterministic() && fsa->type == "deterministic") {
        std::cout << "E7: FSA is non-deterministic";
        return 0;
    }

    fsa->kleenesAlgorithm();
}
