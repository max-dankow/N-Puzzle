#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <set>
#include <map>

typedef unsigned long long State;//тип в котром будем хранить закодированное состояние поля

struct SetNode
{
    State state;
    long long priority;
    long long distance;
    State parent;
};

struct Info
{
    long long priority;
    long long distance;
    State parent;
};

bool operator <(const SetNode &a, const SetNode &b)
{
    return (a.priority < b.priority || (a.priority == b.priority && a.state < b.state));
}

enum Directions
{
    UP = 0,
    LEFT = 1,
    RIGTH = 2,
    DOWN = 3
};

static std::string direction_names[4] = {"UP", "LEFT", "DOWN", "RIGHT"};

class CPuzzleSolver
{
public:
    void print_field(State state);
    CPuzzleSolver(ssize_t new_size):size(new_size),
        element_number(new_size * new_size){}
    State encode_vector_to_state(const std::vector<int> &data);//закодировать поле
    std::vector<int> solve_puzzle(const State &start, const State &goal);

private:
    ssize_t size;
    ssize_t element_number;
    State start, goal;

    struct Offset
    {
        int x[4];
        int y[4];
    } moves = {{0, -1, 0, 1}, {-1, 0, 1, 0}};

    State move_free_cell_to(State state, Directions direction);
    long long calculate_manhattan_distance(State state);
    int recognize_direction(State start, State finish);
    std::vector<int> restore_way(const std::map<State, Info> &closed, State goal);
    bool is_valid_coordinates(int x, int y);
    bool is_solvable(State state);
};

bool CPuzzleSolver::is_solvable(State state)
{
    std::vector<int> table;
    State local_state = state;
    ssize_t free_index = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        size_t element = local_state % element_number;

        if (element == 0)
        {
            free_index = i;
        }

        table.push_back(element);
        local_state /= element_number;
    }

    assert(free_index < element_number);
    int sum = free_index % size + 1;
    for (size_t i = 0; i < element_number; ++i)
    {
        if (table[i] == 0)
            continue;
        for (size_t j = i + 1; i < element_number; ++i)
        {
            assert(table[i] != table[j]);

            if (table[j] < table[i])
            {
                ++sum;
            }
        }
    }

    return sum % 2 == 0;
}

bool CPuzzleSolver::is_valid_coordinates(int x, int y)
{
    return (x >= 0 && y >= 0 && x < size && y < size);
}

void CPuzzleSolver::print_field(State state)
{
    State local_state = state;

    for (size_t i = 0; i < size; ++i)
    {
        for (size_t i = 0; i < size; ++i)
        {
            std::cout << local_state % element_number << ' ';
            local_state /= element_number;
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

State CPuzzleSolver::encode_vector_to_state(const std::vector<int> &data)
{
    assert(size > 0);
    assert(data.size() == size * size);

    State state = 0;

    for (ssize_t i = data.size() - 1; i >= 0; --i)
    {
        state = state * element_number + data[i];
    }
    return state;
}

State CPuzzleSolver::move_free_cell_to(State state, Directions direction)
{
    std::vector<int> table;
    State local_state = state;
    ssize_t free_index = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        size_t element = local_state % element_number;

        if (element == 0)
        {
            free_index = i;
        }

        table.push_back(element);
        local_state /= element_number;
    }

    assert(free_index < element_number);

    int new_x = free_index % size + moves.x[direction];
    int new_y = free_index / size + moves.y[direction];

    if (is_valid_coordinates(new_x, new_y))
    {
        std::swap(table[free_index], table[free_index + moves.x[direction] + moves.y[direction] * size]);
    }
    return encode_vector_to_state(table);
}

long long CPuzzleSolver::calculate_manhattan_distance(State state)
{
    int sum = 0;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            int number = state % element_number;

            if (number != 0)
            {
                sum += abs(number / size - i) + abs(number % size - j);
            }

            state /= element_number;
        }
    }
    return sum;
}

int CPuzzleSolver::recognize_direction(State start, State finish)
{
    ssize_t free_index_start = element_number;
    ssize_t free_index_finish = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        if (start % element_number == 0)
        {
            free_index_start = i;
        }
        start /= element_number;

        if (finish % element_number == 0)
        {
            free_index_finish = i;
        }
        finish /= element_number;
    }

    assert(free_index_start < element_number);
    assert(free_index_finish < element_number);

    for (int i = 0; i < 4; ++i)
    {
        if(free_index_start - free_index_finish == moves.x[i] + moves.y[i] * size)
            return Directions(i);
    }
    assert(false);
}

std::vector<int> CPuzzleSolver::restore_way(const std::map<State, Info> &closed, State goal)
{
    auto state_it = closed.find(goal);
    std::vector<int> way;

    while (state_it->second.parent != -1)
    {
        way.push_back(recognize_direction(state_it->first, state_it->second.parent));
        state_it = closed.find(state_it->second.parent);
    }

    std::reverse(way.begin(), way.end());
    return way;
}

std::vector<int> CPuzzleSolver::solve_puzzle(const State &start, const State &goal)
{
    std::set<SetNode> open;
    std::map<State, Info> closed;
    SetNode start_info;
    start_info.state = start;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = calculate_manhattan_distance(start);
    open.insert(start_info);

    while (open.size() != 0)
    {
        SetNode current = (*open.begin());
        open.erase(current);

        Info to_close;
        to_close.distance = current.distance;
        to_close.parent = current.parent;
        to_close.priority = current.priority;
        closed.insert(std::make_pair(current.state, to_close));

        if (current.state == goal)
        {
            return restore_way(closed, goal);
        }

        for (int i = 0; i < 4; ++i)
        {
            State new_state = move_free_cell_to(current.state, Directions(i));

            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                SetNode new_info;
                new_info.state = new_state;
                new_info.parent = current.state;
                new_info.distance = current.distance + 1;
                new_info.priority = new_info.distance + calculate_manhattan_distance(new_state);
                open.insert(new_info);
            }
        }

    }
    return std::vector<int>();
}

void print_answer(const std::vector<int> &answer)
{
    std::cout << answer.size() << '\n';

    for (int d : answer)
    {
        std::cout << direction_names[d] << '\n';
    }
}

void read_input(std::vector<int> &start_table, std::vector<int> &goal_table, ssize_t &size)
{
    std::cin >> size;
    start_table.clear();
    goal_table.clear();

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            int input_number;
            std::cin >> input_number;
            start_table.push_back(input_number);
            goal_table.push_back(i * size + j);
        }
    }
}

int main()
{
    freopen("input.txt", "r", stdin);
    std::vector<int> start_table, goal_table;
    ssize_t size;
    read_input(start_table, goal_table, size);

    CPuzzleSolver solver(size);
    State start = solver.encode_vector_to_state(start_table);
    State goal = solver.encode_vector_to_state(goal_table);
    std::vector<int> answer;
    answer = solver.solve_puzzle(start, goal);

    print_answer(answer);
    return 0;
}
