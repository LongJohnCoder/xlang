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

namespace xlangidl
{
    using namespace tao::pegtl;

    template< typename R, typename P = space >
    struct padr : seq< R, star< P > > {};

    struct open_curly : padr< one< '{' > > {};
    struct close_curly : padr< one< '}' > > {};

    struct dotted_identifier : seq< identifier, star< seq< one<'.'>, identifier >>> {};

    struct namespace_ : seq< 
        string< 'n', 'a', 'm', 'e', 's', 'p', 'a', 'c', 'e' >, 
        plus<space>, 
        padr<dotted_identifier>, 
        open_curly, 
        close_curly> {};

    struct something : sor< space, namespace_ > {};
    struct grammar : until< eof, must< something > >{};

    template< typename Rule > struct selector : std::false_type {};
    template<> struct selector< grammar > : std::true_type {};
    template<> struct selector< namespace_ > : std::true_type {};
    template<> struct selector< identifier > : std::true_type {};

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

    // auto root = parse_tree::parse<foo::comp>(memory_input("AAB", ""));
    // auto hello_root = parse_tree::parse<hello::grammar>(memory_input("hello, xlang!", ""));

    string_input<> in("   namespace xlang  { } namespace Windows.Foundation  { }  ", "");
    auto root = parse_tree::parse<xlangidl::grammar, xlangidl::selector>(in);

    // std::string name;
    // parse<hello::grammar, hello::action>(in, name);

    writer w;
    if (root)
    {
        print_node(w, *root);
    }
    else
    {
        w.write("fail\n");
    }
    w.flush_to_console();
}
