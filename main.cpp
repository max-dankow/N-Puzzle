#include <stdlib.h>
#include <vector>
#include "cgamestate.h"
#include "cpuzzlesolver.h"

const int GAME_SIZE = 4;
const size_t RANDOM_MOVES_NUMBER = 200;
const std::string direction_names[4] = {"UP", "LEFT", "RIGHT", "DOWN"};

std::vector<char> generate_target_table(size_t size)
{
    std::vector<char> target_field;
    size_t element_number = size * size;
    for (size_t i = 0; i < element_number; ++i)
    {
        if (DOWN_LEFT_CONER_ZERO)
        {
            target_field.push_back((i + 1) % (element_number));
        }
        else
        {
            target_field.push_back(i % (element_number));
        }
    }
    return target_field;
}

void print_answer(const std::vector<Directions> &answer)
{
    std::cout << answer.size() << '\n';

    for (int d : answer)
    {
        std::cout << direction_names[d][0];
    }
    std::cout << '\n';
}

void read_input_puzzle(size_t size, std::vector<char> &field)
{
    size_t element_number = size * size;
    field.clear();
    for (size_t i = 0; i < element_number; ++i)
    {
        int input_value;
        std::cin >> input_value;
        field.push_back((char)input_value);
    }
}

void run_for_online_judge(void)
{
    int size = GAME_SIZE;
    size_t test_number;
    std::cin >> test_number;
    CGameState target(generate_target_table(size), size);
    for(size_t i = 0; i < test_number; ++i)
    {
        std::vector<char> test_field;
        read_input_puzzle(size, test_field);
        CGameState test_state(test_field, size);
        //test_state.print_field();

        std::vector<Directions> answer;
        CPuzzleSolver solver(size, target);
        if (solver.solve_puzzle(test_state, answer, A_STAR, 13))
        {
            print_answer(answer);
        }
        else
        {
            std::cout << "This puzzle is not solvable.\n";
        }
    }
}

int main()
{
    while(1)
    {
        int size = GAME_SIZE;
        CGameState target(generate_target_table(size), size);
        CGameState test_state = target.shuffle_field(RANDOM_MOVES_NUMBER);
        test_state.print_field();

        std::vector<Directions> answer;
        CPuzzleSolver solver(size, CGameState(generate_target_table(size), size));
        size_t limit = 0;
        std::cout << "A*" << limit << '\n';
        solver.solve_puzzle(test_state, answer, A_STAR, limit);
        if (solver.check_solution(answer, test_state))
        {
            std::cout << "CORRECT\n";
        }
        else
        {
            std::cout << "ERROR!\n";
        }
        print_answer(answer);
        std::cout << '\n';

        limit = 10;
        std::cout << "A*" << limit << '\n';
        solver.solve_puzzle(test_state, answer, A_STAR, limit);
        if (solver.check_solution(answer, test_state))
        {
            std::cout << "CORRECT\n";
        }
        else
        {
            std::cout << "ERROR!\n";
        }
        print_answer(answer);
        std::cout << '\n';

        limit = 13;
        std::cout << "A*" << limit << '\n';
        solver.solve_puzzle(test_state, answer, A_STAR, limit);
        if (solver.check_solution(answer, test_state))
        {
            std::cout << "CORRECT\n";
        }
        else
        {
            std::cout << "ERROR!\n";
        }
        print_answer(answer);
        std::cout << '\n';

        limit = 14;
        std::cout << "A*" << limit << '\n';
        solver.solve_puzzle(test_state, answer, A_STAR, limit);
        if (solver.check_solution(answer, test_state))
        {
            std::cout << "CORRECT\n";
        }
        else
        {
            std::cout << "ERROR!\n";
        }
        print_answer(answer);
        std::cout << '\n';

        limit = test_state.calculate_heuristic(target) / 7;
        std::cout << "A*(DYNAMIC)" << limit << '\n';
        solver.solve_puzzle(test_state, answer, A_STAR, limit);
        if (solver.check_solution(answer, test_state))
        {
            std::cout << "CORRECT\n";
        }
        else
        {
            std::cout << "ERROR!\n";
        }
        print_answer(answer);
        std::cout << '\n';

        std::cout << "IDA*\n";
        solver.solve_puzzle(test_state, answer, IDA_STAR, 0);
        if (solver.check_solution(answer, test_state))
        {
            std::cout << "CORRECT\n";
        }
        else
        {
            std::cout << "ERROR!\n";
        }
        print_answer(answer);
        std::cout << "\nPress enter ";
        getchar();
    }
    //run_for_online_judge();

    return EXIT_SUCCESS;
}
