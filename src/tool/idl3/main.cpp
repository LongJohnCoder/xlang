#include "pch.h"

struct writer : xlang::text::writer_base<writer>
{
};

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

namespace hello
{
    struct prefix : pegtl::string<'H', 'e', 'l', 'l', 'o',',',' '>{};
    struct name : pegtl::plus<pegtl::alpha>{};
    struct grammar : pegtl::must<prefix, name, pegtl::one<'!'>, pegtl::eof>{};

    template<typename Rule>
    struct action : pegtl::nothing<Rule>{};

    template<>
    struct action<name>
    {
        template<typename Input>
        static void apply(Input const& in, std::string& v)
        {
            v = in.string();
        }
    };
}

int main(int const /*argc*/, char** /*argv*/)
{
    pegtl::string_input<> in("Hello, Harry!", "from_content");

    std::string name;
    pegtl::parse<hello::grammar, hello::action>(in, name);

    writer w;
    w.write("Good bye, %!\n", name);
    w.flush_to_console();
}
