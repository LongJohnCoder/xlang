#include "pch.h"
#include <tao/pegtl/contrib/parse_tree.hpp>

struct writer : xlang::text::writer_base<writer>
{
    using writer_base<writer>::write;

    void write(tao::pegtl::position const& position)
    {
        write(tao::pegtl::to_string(position));
    }
};

namespace hello
{
    using namespace tao::pegtl;

    struct prefix : string<'H', 'e', 'l', 'l', 'o',',',' '>{};
    struct name : plus<alpha>{};
    struct grammar : must<prefix, name, one<'!'>, eof>{};

    template<typename Rule>
    struct action : nothing<Rule>{};

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

namespace foo
{
    using namespace tao::pegtl;

    struct A : one<'A'> { };
    struct B : one<'B'> { };


    struct comp : seq<plus<sor<seq<A, B>, A>>, eof> { };

    template< typename Rule >
    struct test_action : nothing< Rule > {};
}

void print_node(writer& w, const tao::pegtl::parse_tree::node& n, const std::string& s = "" )
{
    // detect the root node:
    if( n.is_root() ) {
        w.write("ROOT\n");
    }
    else {
        if( n.has_content() ) 
        {
            w.write("%% \"%\" at % to %\n", s, n.name(), n.content(), n.begin(), n.end());
        }
        else {
            w.write("%% at %\n", s, n.name(),n.begin());
        }
    }
    // print all child nodes
    if( !n.children.empty() ) {
        const auto s2 = s + "  ";
        for( auto& up : n.children ) {
        print_node(w, *up, s2 );
        }
    }
}

int main(int const /*argc*/, char** /*argv*/)
{
    using namespace tao::pegtl;

    string_input<> in("Hello, xlang!", "");

    // auto root = parse_tree::parse<foo::comp>(memory_input("AAB", ""));
    auto root = parse_tree::parse<hello::grammar>(in);

    // std::string name;
    // parse<hello::grammar, hello::action>(in, name);

    writer w;
    print_node(w, *root);
    w.flush_to_console();
}
