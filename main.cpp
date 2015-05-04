#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <chrono>

class CGameState
{
public:
    CGameState(std::vector<char> new_field):field(new_field), size(new_field.size()){}
    void print_state(void);
private:
    const ssize_t INVALID_COORDINATES = -1;

    std::vector<char> field;
    size_t size;

    ssize_t get_index(ssize_t row, ssize_t column);
};

class CPuzzleSolver
{
public:
    CPuzzleSolver() {}
};

ssize_t CGameState::get_index(ssize_t row, ssize_t column)
{
    if (row >= 0 && column >= 0 && row < size && column < size)
    {
        return row * size + column;
    }
    else
    {
        return INVALID_COORDINATES;
    }
}

void CGameState::print_state(void)
{
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            std::cout << (int)(field[get_index(row, col)]);
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
int main()
{
    return EXIT_SUCCESS;
}
