#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <set>


int size = 3;
int element_number = size * size;

typedef unsigned long long tState;

typedef struct {
    tState state;
    long long priority;
    long long distance;
    tState parent;
} Info;

const int D_UP = 0;
const int D_LEFT = 1;
const int D_RIGTH = 2;
const int D_DOWN = 3;

bool operator ==(const Info &a, const Info &b)
{
    return a.state == b.state;
}

bool operator <(const Info &a, const Info &b)
{
    return a.priority < b.priority;
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
    return 0;
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

std::vector<int> restore_way(const std::set<Info> &closed, tState goal)
{
    //std::cout << "RESTORING...\n";
    Info tmp;
    tmp.state = goal;

    auto state_it = closed.find(tmp);
    std::vector<int> way;

    while (state_it->parent != -1)
    {
        //print(state_it->first);
        way.push_back(where_from(state_it->state, state_it->parent));

        tmp.state = state_it->parent;
        state_it = closed.find(tmp);
    }

    std::reverse(way.begin(), way.end());
    return way;
}

std::vector<int> dijkstra(const tState &start, const tState &goal)
{
    std::set<Info> closed, open;
    Info start_info;
    start_info.state = start;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = estimation(start);

    open.insert(start_info);
    //print(start);
    //size(goal);

    while (open.size() != 0)
    {
        Info current = (*open.rbegin());
        //print(current.first);
        open.erase(current);

        closed.insert(current);

        if (current.state == goal)
        {
            //std::cout << "FIND!!" << '\n';
            return restore_way(closed, goal);
        }

        for (int i = 0; i < 4; ++i)
        {
            tState new_state = move_to(current.state, i);
            Info to_find;
            to_find.state = new_state;

            if (closed.find(to_find) != closed.end())
                continue;

            if (new_state != current.state)
            {
                auto new_state_it = open.find(to_find);

                if (new_state_it == open.end())
                {
                    Info new_info;
                    new_info.state = new_state;
                    new_info.parent = current.state;
                    new_info.distance = current.distance + 1;
                    new_info.priority = new_info.distance + estimation(new_state);
                    open.insert(new_info);
                }
                else
                {
                    if (current.distance + 1 < new_state_it->distance)
                    {
                        Info new_info;
                        new_info.state = new_state;
                        new_info.parent = current.state;
                        new_info.distance = current.distance + 1;
                        new_info.priority = new_info.distance + estimation(new_state);
                        open.erase(new_info);
                        open.insert(new_info);
                    }
                }
            }
        }

    }
    std::vector<int> a;
    return a;
}

int main()
{
    //freopen("input.txt", "r", stdin);

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
