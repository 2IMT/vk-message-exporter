#include <iostream>

#include "db/db.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    std::cout << "Hello World!" << std::endl;

    try
    {
        vme::db::db database("database.db");
    }
    catch (const vme::db::db_init_error& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
