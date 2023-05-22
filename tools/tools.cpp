#include "../MainInc/main.hpp"

void   invalid_argc(int argc)
{
    if (argc != 2)
    {
        std::cerr << INVALID_ARGS << std::endl;
        exit(EXIT_FAILURE);
    }
}
void   get_help(char **argv)
{
    if (!strcmp(argv[1], "-h"))
    {
        std::cerr << HELP << std::endl;
        exit (EXIT_SUCCESS);
    }
}