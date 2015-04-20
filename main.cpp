#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <set>
#include <map>

typedef unsigned long long State;

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

const int D_UP = 0;
const int D_LEFT = 1;
const int D_RIGTH = 2;
const int D_DOWN = 3;

class CPuzzleSolver
{
public:
    void print_field(State state);

    CPuzzleSolver(ssize_t new_size);
    State get_state(const std::vector<int> &data);
    std::vector<int> solve_puzzle(const State &start, const State &goal);

private:
    ssize_t size;
    ssize_t element_number;
    State start, goal;

    State move_to(State state, int direction);
    long long estimation(State state);
    int get_direction(State start, State finish);
    std::vector<int> restore_way(const std::map<State, Info> &closed, State goal);
};

CPuzzleSolver::CPuzzleSolver(ssize_t new_size)
{
    size = new_size;
    element_number = new_size * new_size;
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

State CPuzzleSolver::get_state(const std::vector<int> &data)
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

State CPuzzleSolver::move_to(State state, int direction)
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
    ssize_t offset = 0;

    switch (direction)
    {
    case D_DOWN:
        if ((free_index / size) + 1 < size)
        {
            offset = size;
        }
        break;

    case D_UP:
        if ((free_index / size) - 1 >= 0)
        {
            offset = -size;
        }
        break;

    case D_LEFT:
        if ((free_index % size) - 1 >= 0)
        {
            offset = -1;
        }
        break;

    case D_RIGTH:
        if ((free_index % size) + 1 < size)
        {
            offset = 1;
        }
        break;
    }

    if (offset == 0)
    {
        return state;
    }

    std::swap(table[free_index], table[free_index + offset]);

    return get_state(table);
}

long long CPuzzleSolver::estimation(State state)
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

int CPuzzleSolver::get_direction(State start, State finish)
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

    if (free_index_start == free_index_finish + 1)
    {
        return D_RIGTH;
    }

    if (free_index_start == free_index_finish - 1)
    {
        return D_LEFT;
    }

    if (free_index_start == free_index_finish + size)
    {
        return D_DOWN;
    }

    if (free_index_start == free_index_finish - size)
    {
        return D_UP;
    }

    assert(false);
}

std::vector<int> CPuzzleSolver::restore_way(const std::map<State, Info> &closed, State goal)
{
    auto state_it = closed.find(goal);
    std::vector<int> way;

    while (state_it->second.parent != -1)
    {
        way.push_back(get_direction(state_it->first, state_it->second.parent));
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
    start_info.priority = estimation(start);
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
            State new_state = move_to(current.state, i);

            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                SetNode new_info;
                new_info.state = new_state;
                new_info.parent = current.state;
                new_info.distance = current.distance + 1;
                new_info.priority = new_info.distance + estimation(new_state);
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
        switch (d)
        {
        case D_DOWN:
            std::cout << "DOWN\n";
            break;
        case D_UP:
            std::cout << "UP\n";
            break;
        case D_LEFT:
            std::cout << "LEFT\n";
            break;
        case D_RIGTH:
            std::cout << "RIGHT\n";
            break;
        }
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
    //freopen("input.txt", "r", stdin);
    std::vector<int> start_table, goal_table;
    ssize_t size;
    read_input(start_table, goal_table, size);

    CPuzzleSolver solver(size);
    State start = solver.get_state(start_table);
    State goal = solver.get_state(goal_table);
    std::vector<int> answer;
    answer = solver.solve_puzzle(start, goal);

    print_answer(answer);
    return 0;
}
