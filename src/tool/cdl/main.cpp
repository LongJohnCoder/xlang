#include "pch.h"
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/analyze.hpp>

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

namespace cdl
{
    namespace peg = tao::pegtl;

    // 2.3.1 Line terminators
    // use peg::eol

    // 2.3.2 Comments
    struct single_line_comment : peg::seq< 
        peg::string< '/', '/' >, 
        peg::until< peg::eol > >  {};
    struct delimited_comment : peg::seq< 
        peg::string< '/', '*' >, 
        peg::until< peg::string< '*', '/' > > > {};
    struct comment : peg::sor< 
        single_line_comment, 
        delimited_comment > {}; 

    // 2.3.3 White space
    // use peg::blank

    // 2.4 Tokens
    struct identifier;
    struct keyword;
    struct integer_literal; 
    struct real_literal;
    struct character_literal; 
    struct string_literal;
    struct operator_or_punctuator;
    struct token : peg::sor< 
        identifier, 
        keyword, 
        integer_literal, 
        real_literal, 
        character_literal, 
        string_literal, 
        operator_or_punctuator > {};

    // 2.4.1 Unicode character escape sequences
    struct unicode_escape_sequence : peg::sor< 
        peg::seq< 
            peg::string< '\\', 'u' >, 
            peg::xdigit,
            peg::xdigit,
            peg::xdigit,
            peg::xdigit >,
        peg::seq< 
            peg::string< '\\', 'U' >, 
            peg::xdigit,
            peg::xdigit,
            peg::xdigit,
            peg::xdigit,
            peg::xdigit,
            peg::xdigit,
            peg::xdigit,
            peg::xdigit > > {};

    // 2.4.2 Identifiers
    struct keyword;
    struct identifier_start_character : peg::sor< 
        peg::alpha, 
        peg::one< '_' > >{};
    struct identifier_part_character : peg::sor< 
        identifier_start_character, 
        peg::digit > {};
    struct identifier_or_keyword : peg::seq< 
        identifier_start_character, 
        peg::star< identifier_part_character > > {};
    struct available_identifier : peg::seq<
        peg::not_at< keyword >,
        identifier_or_keyword > {};
    struct identifier : peg::sor< 
        available_identifier,
        peg::seq< peg::one< '@' >, identifier_or_keyword > >{};

    // 2.4.3 Keywords
    struct keyword : peg::sor< 
        peg::string< 'a', 'b', 's', 't', 'r', 'a', 'c', 't' >, 
        peg::string< 'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e' >, 
        peg::string< 'B', 'o', 'o', 'l', 'e', 'a', 'n' >, 
        peg::string< 'C', 'h', 'a', 'r' >, 
        peg::string< 'c', 'l', 'a', 's', 's' >, 
        peg::string< 'c', 'o', 'n', 's', 't' >, 
        peg::string< 'd', 'e', 'f', 'a', 'u', 'l', 't' >, 
        peg::string< 'd', 'e', 'l', 'e', 'g', 'a', 't', 'e' >, 
        peg::string< 'D', 'o', 'u', 'b', 'l', 'e' >, 
        peg::string< 'e', 'n', 'u', 'm' >, 
        peg::string< 'e', 'v', 'e', 'n', 't' >, 
        peg::string< 'f', 'a', 'l', 's', 'e' >, 
        peg::string< 'G', 'u', 'i', 'd' >, 
        peg::string< 'i', 'n' >, 
        peg::string< 'I', 'n', 't', '8' >, 
        peg::string< 'I', 'n', 't', '1', '6' >, 
        peg::string< 'I', 'n', 't', '3', '2' >, 
        peg::string< 'I', 'n', 't', '6', '4' >, 
        peg::string< 'i', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e' >, 
        peg::string< 'l', 'o', 'n', 'g' >, 
        peg::string< 'n', 'a', 'm', 'e', 's', 'p', 'a', 'c', 'e' >, 
        peg::string< 'n', 'u', 'l', 'l' >, 
        peg::string< 'O', 'b', 'j', 'e', 'c', 't' >, 
        peg::string< 'o', 'u', 't' >, 
        peg::string< 'o', 'v', 'e', 'r', 'r', 'i', 'd', 'e' >, 
        peg::string< 'p', 'r', 'o', 't', 'e', 'c', 't', 'e', 'd' >, 
        peg::string< 'p', 'u', 'b', 'l', 'i', 'c' >, 
        peg::string< 'r', 'e', 'f' >, 
        peg::string< 's', 'e', 'a', 'l', 'e', 'd' >, 
        peg::string< 's', 'h', 'o', 'r', 't' >, 
        peg::string< 's', 't', 'a', 't', 'i', 'c' >, 
        peg::string< 'S', 't', 'r', 'i', 'n', 'g' >, 
        peg::string< 's', 't', 'r', 'u', 'c', 't' >, 
        peg::string< 'S', 'i', 'n', 'g', 'l', 'e' >, 
        peg::string< 't', 'r', 'u', 'e' >, 
        peg::string< 'U', 'I', 'n', 't', '8' >, 
        peg::string< 'U', 'I', 'n', 't', '1', '6' >, 
        peg::string< 'U', 'I', 'n', 't', '3', '2' >, 
        peg::string< 'U', 'I', 'n', 't', '6', '4' >, 
        peg::string< 'u', 's', 'i', 'n', 'g' >, 
        peg::string< 'v', 'i', 'r', 't', 'u', 'a', 'l' >, 
        peg::string< 'v', 'o', 'i', 'd' > > {};

    // 2.4.4 Literals
    struct boolean_literal; 
    struct integer_literal; 
    struct real_literal;
    struct character_literal; 
    struct string_literal;
    struct null_literal;
    struct literal : peg::sor<
        boolean_literal, 
        integer_literal, 
        real_literal, 
        character_literal, 
        string_literal, 
        null_literal > {};

    // 2.4.4.1 Boolean literals
    struct boolean_literal : peg::sor< 
        peg::string< 't', 'r', 'u', 'e' >, 
        peg::string< 'f', 'a', 'l', 's', 'e' > > {};

    // 2.4.4.2 Integer literals
    struct integer_type_suffix : peg::sor< 
        peg::one< 'U' >, 
        peg::one< 'u' >, 
        peg::one< 'L' >, 
        peg::one< 'l' >, 
        peg::string< 'U', 'L' >, 
        peg::string< 'U', 'l' >, 
        peg::string< 'u', 'L' >, 
        peg::string< 'u', 'l' >, 
        peg::string< 'L', 'U' >, 
        peg::string< 'L', 'u' >, 
        peg::string< 'l', 'U' >, 
        peg::string< 'l', 'u' > > {};
    struct hex_digits : peg::plus< peg::xdigit > {};
    struct hexadecimal_integer_literal : peg::seq<
        peg::one< '0' >,
        peg::sor< peg::one< 'x' >, peg::one< 'X' >>,
        hex_digits,
        peg::opt< integer_type_suffix > > {};
    struct decimal_digits : peg::plus< peg::digit > {};
    struct decimal_integer_literal : peg::seq< decimal_digits, peg::opt< integer_type_suffix > > {};
    struct integer_literal : peg::sor< decimal_integer_literal, hexadecimal_integer_literal > {};

    // 2.4.4.3 Real literals
    struct real_type_suffix : peg::sor< 
        peg::one< 'F' >, 
        peg::one< 'f' >, 
        peg::one< 'D' >, 
        peg::one< 'd' > > {};
    struct sign : peg::sor< 
        peg::one< '+' >, 
        peg::one< '-' > > {};
    struct exponent_part : peg::seq< 
        peg::sor< peg::one< 'e' >,  peg::one< 'E' > >,
        peg::opt< sign >,
        decimal_digits > {};
    struct real_literal : peg::sor< 
        peg::seq< 
            decimal_digits, 
            peg::one< '.' >, 
            decimal_digits, 
            peg::opt< exponent_part >, 
            peg::opt< real_type_suffix > >, 
        peg::seq< 
            peg::one< '.' >, 
            decimal_digits, 
            peg::opt< exponent_part >, 
            peg::opt< real_type_suffix > >, 
        peg::seq< 
            decimal_digits, 
            exponent_part, 
            peg::opt< real_type_suffix > >, 
        peg::seq< 
            decimal_digits, 
            real_type_suffix > > {};

    // 2.4.4.4 Character literals
    struct hexadecimal_escape_sequence : peg::seq< 
        peg::string< '\\', 'x' >, 
        peg::xdigit, 
        peg::opt< peg::xdigit >, 
        peg::opt< peg::xdigit >, 
        peg::opt< peg::xdigit > > {};
    struct simple_escape_sequence : peg::sor< 
        peg::string< '\\', '\'' >, 
        peg::string< '\\', '"' >, 
        peg::string< '\\', '\\' >, 
        peg::string< '\\', '0' >, 
        peg::string< '\\', 'a' >, 
        peg::string< '\\', 'b' >, 
        peg::string< '\\', 'f' >, 
        peg::string< '\\', 'n' >, 
        peg::string< '\\', 'r' >, 
        peg::string< '\\', 't' >, 
        peg::string< '\\', 'v' > > {};
    struct single_character : peg::seq<
        peg::not_at< peg::one< '\'' > >,
        peg::not_at< peg::one< '\\' > >,
        peg::not_at< peg::eol >,
        peg::any > {};
    struct character : peg::sor< 
        single_character, 
        simple_escape_sequence, 
        hexadecimal_escape_sequence, 
        unicode_escape_sequence > {};
    struct character_literal : peg::seq< 
        peg::one< '\'' >, 
        character, 
        peg::one< '\'' > > {};

    // 2.4.4.5 String literals
    struct single_verbatim_string_literal_character : peg::seq<
        peg::not_at< peg::one< '"' > >,
        peg::any > {};
    struct quote_escape_sequence : peg::string< '"', '"' > {};
    struct verbatim_string_literal_character : peg::sor< 
        single_verbatim_string_literal_character, 
        quote_escape_sequence > {};
    struct verbatim_string_literal_characters : peg::plus< verbatim_string_literal_character > {};
    struct verbatim_string_literal : peg::seq< 
        peg::string< '@', '"' >, 
        peg::opt< verbatim_string_literal_characters >, 
        peg::one< '"' > > {};
    struct single_regular_string_literal_character : peg::seq<
        peg::not_at< peg::one< '"' > >,
        peg::not_at< peg::one< '\\' > >,
        peg::not_at< peg::eol >,
        peg::any > {};
    struct regular_string_literal_character : peg::sor< 
        single_regular_string_literal_character, 
        simple_escape_sequence, 
        hexadecimal_escape_sequence, 
        unicode_escape_sequence > {};
    struct regular_string_literal_characters : peg::plus< regular_string_literal_character > {};
    struct regular_string_literal : peg::seq< 
        peg::one< '"' >, 
        peg::opt< regular_string_literal_characters >, 
        peg::one< '"' > > {};
    struct string_literal : peg::sor< 
        regular_string_literal, 
        verbatim_string_literal > {};

    // 2.4.4.6 The null literal
    struct null_literal : peg::string< 'n', 'u', 'l', 'l' > {};

    // 2.4.5 Operators and punctuators
    struct operator_or_punctuator : peg::sor< 
        peg::one< '{' >, peg::one< '}' >, 
        peg::one< '[' >, peg::one< ']' >, 
        peg::one< '(' >, peg::one< ')' >, 
        peg::one< '.' >,
        peg::one< ',' >, 
        peg::string< ':', ':' >, peg::one< ':' >, 
        peg::one< ';' >, 
        peg::string< '+', '+' >, 
        peg::one< '+' >, 
        peg::string< '-', '-' >, 
        peg::one< '-' >, 
        peg::one< '*' >, 
        peg::one< '/' >, 
        peg::one< '%' >, 
        peg::one< '&' >, 
        peg::one< '|' >, 
        peg::one< '^' >, 
        peg::one< '!' >, 
        peg::one< '~' >, 
        peg::one< '=' >, 
        peg::string< '<', '<' >,
        peg::one< '<' >, 
        peg::string< '>', '>' >,
        peg::one< '>' > > {};

    // 2.5 Pre_processing directives
    // TBD

    // 3.6 Namespace and type names
    struct type_argument_list;
    struct qualified_alias_member;
    struct namespace_or_type_name : peg::sor< 
        peg::seq<
            peg::list< 
                identifier, 
                peg::one< '.' > >, 
            type_argument_list >,
         qualified_alias_member > {};
    struct type_name : peg::seq< namespace_or_type_name > {};
    struct namespace_name : peg::seq< namespace_or_type_name > {};

    // 4. Types
    struct value_type;
    struct reference_type;
    struct type_parameter;
    struct type : peg::sor< 
        value_type, 
        reference_type, 
        type_parameter > {};

    // 4.1 Value types
    struct integral_type : peg::sor< 
        peg::string< 'C', 'h', 'a', 'r' >, 
        peg::string< 'I', 'n', 't', '8' >, 
        peg::string< 'I', 'n', 't', '1', '6' >, 
        peg::string< 'I', 'n', 't', '3', '2' >, 
        peg::string< 'I', 'n', 't', '6', '4' >, 
        peg::string< 'U', 'I', 'n', 't', '8' >, 
        peg::string< 'U', 'I', 'n', 't', '1', '6' >, 
        peg::string< 'U', 'I', 'n', 't', '3', '2' >, 
        peg::string< 'U', 'I', 'n', 't', '6', '4' > > {};
    struct floating_point_type : peg::sor< 
        peg::string< 'S', 'i', 'n', 'g', 'l', 'e' >, 
        peg::string< 'D', 'o', 'u', 'b', 'l', 'e' > > {};
    struct numeric_type : peg::sor< 
        integral_type, 
        floating_point_type > {};
    struct simple_type : peg::sor< 
        numeric_type, 
        peg::string< 'B', 'o', 'o', 'l', 'e', 'a', 'n' > > {}; 
    struct non_nullable_value_type : peg::seq< type > {};
    struct nullable_type : peg::seq< 
        non_nullable_value_type, 
        peg::one< '?' > > {};
    struct struct_type : peg::sor< 
        type_name, 
        simple_type,
        nullable_type> {}; 
    struct enum_type : peg::seq< type > {};
    struct value_type : peg::sor< 
        struct_type, 
        enum_type> {}; 

    // 4.2 Reference types
    struct class_type : peg::sor< 
        type_name, 
        peg::string< 'O', 'b', 'j', 'e', 'c', 't' > > {};
    struct interface_type : peg::seq< type_name > {};
    struct delegate_type : peg::seq< type_name > {};
    struct non_array_type : peg::seq< type > {};
    struct rank_specifier : peg::seq<
        peg::one< '[' >,
        peg::one< ']' > > {};
    struct array_type : peg::seq< 
        non_array_type,
        rank_specifier > {};
    struct reference_type : peg::sor< 
        class_type, 
        interface_type, 
        array_type, 
        delegate_type > {};

    // 4.3.1 Type arguments
    struct type_argument : peg::seq< type > {};
    struct type_arguments : peg::list< type_argument, peg::one< ',' >> {};
    struct type_argument_list : peg::seq< 
        peg::one< '<' >, 
        type_arguments, 
        peg::one< '>' > > {};

    // 4.4 Type parameters
    struct type_parameter : peg::seq< identifier > {};

    // 5.2 Primary expressions
    // TBD

    // 6.1 Compilation units
    struct using_directives;
    struct global_attributes;
    struct namespace_member_declarations;
    struct compilation_unit : peg::seq< 
        peg::opt< using_directives >, 
        // peg::opt< global_attributes >,  I think this is missing from the text
        peg::opt< namespace_member_declarations > > {};

    // 6.2 Namespace declarations
    struct qualified_identifier : peg::list< identifier, peg::one< '.' > > {};
    struct namespace_body : peg::seq< 
        peg::one< '{' >, 
        peg::opt< using_directives >, 
        peg::opt< namespace_member_declarations >, 
        peg::one< '}' > > {};
    struct namespace_declaration : peg::seq< 
        peg::string< 'n', 'a', 'm', 'e', 's', 'p', 'a', 'c', 'e' >, 
        qualified_identifier, 
        namespace_body, 
        peg::opt< peg::one< ';' > > > {};

    // 6.3 Using directives
    struct using_alias_directive;
    struct using_namespace_directive;
    struct using_directive : peg::sor< 
        using_alias_directive, 
        using_namespace_directive > {};
    struct using_directives : peg::plus< using_directive > {};

    // 6.3.1 Using alias directives
    struct using_alias_directive : peg::seq< 
        peg::string< 'u', 's', 'i', 'n', 'g' >, 
        identifier, 
        peg::one< '=' >, 
        namespace_or_type_name, 
        peg::one< ';' > > {};

    // 6.3.2 Using namespace directives
    struct using_namespace_directive : peg::seq< 
        peg::string< 'u', 's', 'i', 'n', 'g' >, 
        namespace_name, 
        peg::one< ';' > > {};

    // 6.4 Namespace members
    struct type_declaration;
    struct namespace_member_declaration : peg::sor< 
        namespace_declaration, 
        type_declaration > {};
    struct namespace_member_declarations : peg::plus< namespace_member_declaration > {};

    // 6.5 Type declarations
    struct class_declaration;
    struct struct_declaration;
    struct interface_declaration;
    struct enum_declaration;
    struct delegate_declaration;
    struct type_declaration : peg::sor< 
        class_declaration, 
        struct_declaration, 
        interface_declaration, 
        enum_declaration, 
        delegate_declaration > {};

    // 6.6 Namespace alias qualifiers
    struct qualified_alias_member : peg::seq< 
        identifier, 
        peg::string< ':', ':' >, 
        identifier, 
        peg::opt< type_argument_list > > {};

    template< typename Rule > struct selector : std::false_type {};
    template<> struct selector< comment > : std::true_type {};
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

    const std::size_t issues_found = tao::pegtl::analyze< cdl::comment >();

    auto root = parse_tree::parse<cdl::identifier, cdl::selector>(memory_input("_namespace", ""));
    // auto hello_root = parse_tree::parse<hello::grammar>(memory_input("hello, xlang!", ""));

    // string_input<> in("   namespace xlang  { } namespace Windows.Foundation  { }  ", "");
    // auto root = parse_tree::parse<xlangidl::grammar, xlangidl::selector>(in);

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
