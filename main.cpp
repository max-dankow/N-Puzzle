#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <set>

using namespace std;

const int SIZE = 3;

const int D_UP = 0;
const int D_LEFT = 1;
const int D_RIGTH = 2;
const int D_DOWN = 3;

class Field
{
public:
    unsigned long long state;
    size_t size;
    size_t element_number;
    size_t free_index;

    Field(void);
    void set_state(size_t new_size, const std::vector<int> &data);
    void print(void) const;
    void move_to(Field &dst, int direction);

    void swap_elements(ssize_t index1, ssize_t index2);
};

void Field::swap_elements(ssize_t index1, ssize_t index2)
{
    //assert(index1 >= 0 && index1 < element_number);
    //assert(index2 >= 0 && index2 < element_number);
    if (!(index1 >= 0 && index1 < element_number &&
        index2 >= 0 && index2 < element_number))
    {
        return;
    }

    std::vector<int> table;
    unsigned long long local_state = state;

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
    unsigned long long local_state = state;

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

void dijkstra(const Field &start)
{
    std::set<std::pair<long long, Field> > closed, open;
    open.insert(make_pair(start));
    while (open.size() != 0)
    {

    }
}

int main()
{
    freopen("input.txt", "r", stdin);

    Field start;
    std::vector<int> start_table;

    for (int i = 0; i < SIZE; ++i)
    {
        for (int i = 0; i < SIZE; ++i)
        {
            int input_number;
            std::cin >> input_number;
            start_table.push_back(input_number);
        }
    }

    std::cout << sizeof(start) << '\n';

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
    }

    return 0;
}
