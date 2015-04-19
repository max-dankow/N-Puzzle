#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <set>
#include <map>


int size = 3;
int element_number = size * size;

typedef unsigned long long tState;

struct Set_Node
{
    tState state;
    long long priority;
    long long distance;
    tState parent;
};

struct Info
{
    long long priority;
    long long distance;
    tState parent;
};

const int D_UP = 0;
const int D_LEFT = 1;
const int D_RIGTH = 2;
const int D_DOWN = 3;

bool operator <(const Set_Node &a, const Set_Node &b)
{
    return (a.priority < b.priority ||
            (a.priority == b.priority && a.state < b.state));
}

void print(tState state)
{
    tState local_state = state;

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

tState get_state(const std::vector<int> &data)
{
    assert(size > 0);
    assert(data.size() == size * size);

    tState state = 0;

    for (ssize_t i = data.size() - 1; i >= 0; --i)
    {
        state = state * element_number + data[i];
    }

    return state;
}

tState move_to(tState state, int direction)
{
    std::vector<int> table;
    tState local_state = state;
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

long long estimation(tState state)
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

int where_from(tState start, tState finish)
{
    ssize_t free_index1 = element_number;
    ssize_t free_index2 = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        if (start % element_number == 0)
        {
            free_index1 = i;
        }
        start /= element_number;

        if (finish % element_number == 0)
        {
            free_index2 = i;
        }

        finish /= element_number;
    }

    assert(free_index1 < element_number);
    assert(free_index2 < element_number);

    if (free_index1 == free_index2 + 1)
    {
        return D_RIGTH;
    }

    if (free_index1 == free_index2 - 1)
    {
        return D_LEFT;
    }

    if (free_index1 == free_index2 + size)
    {
        return D_DOWN;
    }

    if (free_index1 == free_index2 - size)
    {
        return D_UP;
    }

    assert(false);
}

std::vector<int> restore_way(const std::map<tState, Info> &closed, tState goal)
{
    //std::cout << "RESTORING...\n";

    auto state_it = closed.find(goal);
    std::vector<int> way;

    while (state_it->second.parent != -1)
    {
        //print(state_it->first);
        way.push_back(where_from(state_it->first, state_it->second.parent));

        state_it = closed.find(state_it->second.parent);
    }

    std::reverse(way.begin(), way.end());
    return way;
}

std::vector<int> dijkstra(const tState &start, const tState &goal)
{
    std::set<Set_Node> open;
    std::map<tState, Info> closed;
    Set_Node start_info;
    start_info.state = start;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = estimation(start);

    open.insert(start_info);
    //print(start);
    //size(goal);

    while (open.size() != 0)
    {
        Set_Node current = (*open.begin());
        //print(current.first);
        open.erase(current);

        Info to_close;// = {current.parent,  current.priority, current.distance};
        to_close.distance = current.distance;
        to_close.parent = current.parent;
        to_close.priority = current.priority;

        closed.insert(std::make_pair(current.state, to_close));

        if (current.state == goal)
        {
            //std::cout << "FIND!!" << '\n';
            return restore_way(closed, goal);
        }

        for (int i = 0; i < 4; ++i)
        {
            tState new_state = move_to(current.state, i);

            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                Set_Node new_info;
                new_info.state = new_state;
                new_info.parent = current.state;
                new_info.distance = current.distance + 1;
                new_info.priority = new_info.distance + estimation(new_state);
                open.insert(new_info);
            }
        }

    }
    std::vector<int> a;
    return a;
}

int main()
{
    freopen("input.txt", "r", stdin);

    std::vector<int> start_table, goal_table;

    std::cin >> size;
    element_number = size * size;

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

    std::cout << estimation(get_state(start_table))<< ' '<<estimation(get_state(goal_table))<<'\n';

    std::vector<int> answer;
    answer = dijkstra(get_state(start_table), get_state(goal_table));
    std::cout << answer.size() << '\n';
    for (int d : answer)
    {
        switch (d) {
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

    return 0;
}
