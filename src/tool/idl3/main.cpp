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

namespace idl3
{
    struct ws : pegtl::one< ' ', '\t', '\n', '\r' > {};

    template< typename R, typename P = ws >
    struct padr : internal::seq< R, internal::star< P > > {};

    struct NAMESPACE : pegtl::keyword<'n','a','m','e','s','p','a','c','e'>{};

    struct OPEN_CURLY : padr<pegtl::one<'{'>>{};
    struct CLOSE_CURLY : padr<pegtl::one<'}'>>{};

    struct namespace_ : pegtl::seq<NAMESPACE, pegtl::identifier, OPEN_CURLY, CLOSE_CURLY>{};
    struct grammar : pegtl::must<pegtl::star<ws>, pegtl::plus<namespace_>, pegtl::eof>{}; 

    template<typename Rule>
    struct action : pegtl::nothing<Rule>{};

    template<>
    struct action<pegtl::identifier>
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
    pegtl::string_input<> in("namespace ATest {}", "from_content");

    std::string name;
    pegtl::parse<hello::grammar, hello::action>(in, name);

    writer w;
    w.write("Good bye, %!\n", name);
    w.flush_to_console();
}
