#include "../MainInc/main.hpp"

void check_param_nbr(int argc)
{
    if (argc != 2)
        std::cerr << INVALID_ARGS << std::endl;
}