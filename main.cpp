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
    CGameState(const std::vector<char> &new_field, size_t new_size);
    void print_field(void);

private:
    const ssize_t INVALID_COORDINATES = -1;

    std::vector<char> field;
    size_t size;

    ssize_t get_index(ssize_t row, ssize_t column);
};

class CPuzzleSolver
{
public:
    CPuzzleSolver(size_t new_game_size, CGameState new_target);
    bool solve_puzzle(CGameState start, std::vector<int> &answer);
private:
    CGameState target;
    size_t game_size;
};
/********************************* CGameState ****************************************/

CGameState::CGameState(const std::vector<char> &new_field, size_t new_size):field(new_field), size(new_size)
{
    assert(new_field.size() == new_size * new_size);
}

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

void CGameState::print_field(void)
{
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            std::cout << (int)(field[get_index(row, col)]) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

std::vector<char> generate_target_table(size_t size)
{
    std::vector<char> target_field;
    size_t element_number = size * size;
    for (size_t i = 0; i < element_number; ++i)
    {
        target_field.push_back((i + 1) % (element_number));
    }
    return target_field;
}

/********************************* CPuzzleSolver *************************************/

CPuzzleSolver::CPuzzleSolver(size_t new_game_size, CGameState new_target):
    target(new_target), game_size(new_game_size) {}

int main()
{
    CGameState target(generate_target_table(4), 4);
    target.print_field();
    CPuzzleSolver solver(4, CGameState(generate_target_table(4), 4));
    return EXIT_SUCCESS;
}
