#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <map>


int size = 3;
int element_number = size * size;

typedef unsigned long long tState;
typedef struct {
    long long priority;
    long long distance;
    tState parent;
} Info;

typedef std::pair<tState, Info> Node_Pair;

const int D_UP = 0;
const int D_LEFT = 1;
const int D_RIGTH = 2;
const int D_DOWN = 3;

bool operator ==(const Node_Pair &a, const Node_Pair &b)
{
    return a.first == b.first;
}


/*
class Field
{
public:
    tState state;
    size_t size;
    size_t element_number;
    size_t free_index;

    Field(void);
    void set_state(size_t new_size, const std::vector<int> &data);
    void print(void) const;
    void move_to(Field &dst, int direction);

    void swap_elements(ssize_t index1, ssize_t index2);
};
*/
/*
void Field::swap_elements(ssize_t index1, ssize_t index2)
{
    if (!(index1 >= 0 && index1 < element_number &&
        index2 >= 0 && index2 < element_number))
    {
        return;
    }

    std::vector<int> table;
    tState local_state = state;

    for (size_t i = 0; i < element_number; ++i)
    {
        table.push_back(local_state % element_number);
        local_state /= element_number;
    }

    std::swap(table[index1], table[index2]);
    set_state(size, table);
    print();
}

void Field::print(void) const
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

Field::Field(void)
{
}

void Field::set_state(size_t new_size, const std::vector<int> &data)
{
    assert(new_size > 0);
    assert(data.size() == new_size * new_size);

    state = 0;
    size = new_size;
    element_number = size * size;
    free_index = element_number;

    for (ssize_t i = data.size() - 1; i >= 0; --i)
    {
        if (data[i] == 0)
        {
            free_index = i;
        }

        state = state * element_number + data[i];
        //std::cout << state << ' ' << data[i] << '\n';
    }

    assert(free_index < element_number);
}

void Field::move_to(Field &dst, int direction)
{
    dst.element_number = element_number;
    dst.free_index = free_index;
    dst.size = size;
    dst.state = state;

    switch (direction)
    {
    case D_DOWN:
        dst.swap_elements(dst.free_index, dst.free_index + size);
        break;

    case D_UP:
        dst.swap_elements(dst.free_index, dst.free_index - size);
        break;

    case D_LEFT:
        dst.swap_elements(dst.free_index, dst.free_index - 1);
        break;

    case D_RIGTH:
        dst.swap_elements(dst.free_index, dst.free_index + 1);
        break;
    }
}
*/
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
    size_t free_index = element_number;

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
        offset = size;
        break;

    case D_UP:
        offset = -size;
        break;

    case D_LEFT:
        offset = -1;
        break;

    case D_RIGTH:
        offset = 1;
        break;
    }

    if (!(free_index >= 0 && free_index < element_number &&
        free_index + offset >= 0 && free_index + offset < element_number))
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

std::vector<int> restore_way(const std::map<tState, Info> &closed, tState goal)
{
    std::cout << "RESTORING...\n";
    auto state_it = closed.find(goal);
    std::vector<int> way;

    while (state_it->second.parent != -1)
    {
        print(state_it->first);
        way.push_back(where_from(state_it->first, state_it->second.parent));

        state_it = closed.find(state_it->second.parent);
    }

    std::reverse(way.begin(), way.end());
    return way;
}

std::vector<int> dijkstra(const tState &start, const tState &goal)
{
    std::map<tState, Info> closed, open;
    Info start_info;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = estimation(start);

    open.insert(std::make_pair(start, start_info));
    print(start);
    print(goal);

    while (open.size() != 0)
    {
        Node_Pair current = (*open.rbegin());
        print(current.first);
        open.erase(current.first);

        closed.insert(current);

        if (current.first == goal)
        {
            std::cout << "FIND!!" << '\n';
            return restore_way(closed, goal);
        }

        for (int i = 0; i < 4; ++i)
        {
            tState new_state = move_to(current.first, i);

            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.first)
            {
                auto new_state_it = open.find(new_state);

                if (new_state_it == open.end())
                {
                    Info new_info;
                    new_info.parent = current.first;
                    new_info.distance = current.second.distance + 1;
                    new_info.distance = new_info.distance + estimation(new_state);
                    open.insert(std::make_pair(new_state, new_info));
                }
                else
                {
                    if (current.second.distance + 1 < new_state_it->second.distance)
                    {
                        Info new_info;
                        new_info.parent = current.first;
                        new_info.distance = current.second.distance + 1;
                        new_info.distance = new_info.distance + estimation(new_state);
                        open.erase(new_state);
                        open.insert(std::make_pair(new_state, new_info));
                    }
                }
            }
        }

    }
}

int main()
{
    freopen("input.txt", "r", stdin);

    //Field start;
    std::vector<int> start_table, goal_table;

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

    /*std::cout << sizeof(start) << '\n';

    start.set_state(SIZE, start_table);
    start.print();
    for (int i = 0; i < 4; ++i)
    {
        Field start2;
        start2.element_number = start.element_number;
        start2.free_index = start.free_index;
        start2.size = start.size;
        start2.state = start.state;
        start2.move_to(start2, i);
    }*/
    std::vector<int> answer;
    answer = dijkstra(get_state(start_table), get_state(goal_table));
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
