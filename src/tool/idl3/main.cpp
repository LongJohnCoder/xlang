#include "pch.h"

struct writer : xlang::text::writer_base<writer>
{
};

int main(int const argc, char** argv)
{
    writer w;

    try
    {
        w.write("idl3 compiler\n");
    }
    catch (std::exception const& e)
    {
        w.write("%\n", e.what());
    }

    w.flush_to_console();
}
