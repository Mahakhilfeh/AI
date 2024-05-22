#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>

using namespace std;

const int BOARD_SIZE = 4;

struct State {
    char board[BOARD_SIZE][BOARD_SIZE];

    size_t operator()(const State& s) const {
        size_t hash = 0;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                hash ^= (hash << 5) + (hash >> 2) + s.board[i][j];
            }
        }
        return hash;
    }

    bool operator==(const State& other) const {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] != other.board[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
};

bool isGoalState(const State& state) {
    char goalBoard[BOARD_SIZE][BOARD_SIZE] = {
        {'-', '-', '-', '-'},
        {'A', '-', 'D', '-'},
        {'-', 'B', 'C', '-'},
        {'-', '-', '-', '-'}
    };

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state.board[i][j] != goalBoard[i][j]) {
                return false;
            }
        }
    }
    return true;
}

vector<State> generateSuccessors(const State& currentState) {
    vector<State> successors;

    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (currentState.board[i][j] != '-') {
                char letter = currentState.board[i][j];
                for (int k = 0; k < 4; ++k) {
                    int newX = i + dx[k];
                    int newY = j + dy[k];
                    if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE &&
                        currentState.board[newX][newY] == '-') {
                        State nextState = currentState;
                        nextState.board[i][j] = '-';
                        nextState.board[newX][newY] = letter;
                        successors.push_back(nextState);
                    }
                }
            }
        }
    }

    return successors;
}


bool depthFirstSearch(const State& initialState, unordered_set<State, State>& visited, vector<State>& path) {
    vector<State> stack;
    stack.push_back(initialState);

    while (!stack.empty()) {
        State currentState = stack.back();
        stack.pop_back();

        path.push_back(currentState);

        if (isGoalState(currentState)) {
            return true;
        }

        visited.insert(currentState);

        for (const auto& nextState : generateSuccessors(currentState)) {
            if (visited.find(nextState) == visited.end()) {
                stack.push_back(nextState);
            }
        }
    }

    return false;
}


int heuristic(const State& state) {
    int manhattanDistance = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state.board[i][j] != '-') {
                char letter = state.board[i][j];
    
                int goalRow{}, goalCol{};
                if (letter == 'A') {
                    goalRow = 1;
                    goalCol = 0;
                }
                else if (letter == 'B') {
                    goalRow = 2;
                    goalCol = 1;
                }
                else if (letter == 'C') {
                    goalRow = 2;
                    goalCol = 2;
                }
                else if (letter == 'D') {
                    goalRow = 1;
                    goalCol = 2;
                }
                // calculate manhattan distance
                manhattanDistance += abs(i - goalRow) + abs(j - goalCol);
            }
        }
    }
    return manhattanDistance;
}



bool bestFirstSearch(const State& initialState, vector<State>& path) {
    auto comp = [](const State& a, const State& b) {
        return heuristic(a) > heuristic(b); 
    };

    priority_queue<State, vector<State>, decltype(comp)> pq(comp);

    unordered_set<State, State> visited;

    pq.push(initialState);

    while (!pq.empty()) {
        State currentState = pq.top();
        pq.pop();

        path.push_back(currentState);

        if (isGoalState(currentState)) {
            return true;
        }

        visited.insert(currentState);

        for (const auto& nextState : generateSuccessors(currentState)) {
            if (visited.find(nextState) == visited.end()) {
                pq.push(nextState);
            }
        }
    }

    return false;
}

void printBoard(const State& state) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cout << state.board[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    ifstream inFile("states.txt");

    State initialState, goalState;
    string line;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        getline(inFile, line);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            initialState.board[i][j] = line[j];
        }
    }

    getline(inFile, line);

    for (int i = 0; i < BOARD_SIZE; ++i) {
        getline(inFile, line);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            goalState.board[i][j] = line[j];
        }
    }

    inFile.close();

    int choice;
    cout << "Choose algorithm:\n1. Depth-First Search\n2. Best-First Search\nEnter choice: ";
    cin >> choice;

    vector<State> path;
    bool solutionFound = false;

    if (choice == 1) {
        unordered_set<State, State> visited;
        solutionFound = depthFirstSearch(initialState, visited, path);
    }
    else if (choice == 2) {
        solutionFound = bestFirstSearch(initialState, path);
    }
    else {
        cout << "Invalid choice" << endl;
        return 1;
    }

    if (solutionFound) {
        cout << "Solution:" << endl;
        for (const auto& state : path) {
            printBoard(state);
        }
        cout << "Number of states expanded: " << path.size() << endl;
    }
    else {
        cout << "No solution found." << endl;
    }

    return 0;
}
