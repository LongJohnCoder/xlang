#include "pch.h"
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

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

namespace cdl
{
    namespace peg = tao::pegtl;

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
    struct sep : peg::sor< peg::ascii::space, comment > {};
    struct seps : peg::star< sep > {};

    // 2.4 Tokens
    // 2.4.1 Unicode character escape sequences
    struct unicode_escape_sequence : peg::sor< 
        peg::seq< 
            peg::string< '\\', 'u' >, 
            peg::xdigit, peg::xdigit, peg::xdigit, peg::xdigit >,
        peg::seq< 
            peg::string< '\\', 'U' >, 
            peg::xdigit, peg::xdigit, peg::xdigit, peg::xdigit,
            peg::xdigit, peg::xdigit, peg::xdigit, peg::xdigit > > {};

    // 2.4.2 Identifiers
    struct keyword;
    struct identifier : peg::sor<
        peg::seq<
            peg::not_at< keyword >,
            peg::identifier >,
        peg::seq<
            peg::one< '@' >,
            peg::identifier > > {};

    // 2.4.3 Keywords
    struct str_abstract : TAO_PEGTL_STRING( "abstract" ){};
    struct str_add : TAO_PEGTL_STRING( "add" ){};
    struct str_assembly : TAO_PEGTL_STRING( "assembly" ){};
    struct str_attribute : TAO_PEGTL_STRING( "attribute" ){};
    struct str_Boolean : TAO_PEGTL_STRING( "Boolean" ){};
    struct str_Char : TAO_PEGTL_STRING( "Char" ){};
    struct str_class : TAO_PEGTL_STRING( "class" ){};
    struct str_const : TAO_PEGTL_STRING( "const" ){};
    struct str_default : TAO_PEGTL_STRING( "default" ){};
    struct str_define : TAO_PEGTL_STRING( "define" ){};
    struct str_delegate : TAO_PEGTL_STRING( "delegate" ){};
    struct str_disable : TAO_PEGTL_STRING( "disable" ){};
    struct str_Double : TAO_PEGTL_STRING( "Double" ){};
    struct str_elif : TAO_PEGTL_STRING( "elif" ){};
    struct str_else : TAO_PEGTL_STRING( "else" ){};
    struct str_endif : TAO_PEGTL_STRING( "endif" ){};
    struct str_endregion : TAO_PEGTL_STRING( "endregion" ){};
    struct str_enum : TAO_PEGTL_STRING( "enum" ){};
    struct str_error : TAO_PEGTL_STRING( "error" ){};
    struct str_event : TAO_PEGTL_STRING( "event" ){};
    struct str_false : TAO_PEGTL_STRING( "false" ){};
    struct str_field : TAO_PEGTL_STRING( "field" ){};
    struct str_get : TAO_PEGTL_STRING( "get" ){};
    struct str_Guid : TAO_PEGTL_STRING( "Guid" ){};
    struct str_hidden : TAO_PEGTL_STRING( "hidden" ){};
    struct str_if : TAO_PEGTL_STRING( "if" ){};
    struct str_in : TAO_PEGTL_STRING( "in" ){};
    struct str_Int8 : TAO_PEGTL_STRING( "Int8" ){};
    struct str_Int16 : TAO_PEGTL_STRING( "Int16" ){};
    struct str_Int32 : TAO_PEGTL_STRING( "Int32" ){};
    struct str_Int64 : TAO_PEGTL_STRING( "Int64" ){};
    struct str_interface : TAO_PEGTL_STRING( "interface" ){};
    struct str_internal : TAO_PEGTL_STRING( "internal" ){};
    struct str_line : TAO_PEGTL_STRING( "line" ){};
    struct str_method : TAO_PEGTL_STRING( "method" ){};
    struct str_module : TAO_PEGTL_STRING( "module" ){};
    struct str_namespace : TAO_PEGTL_STRING( "namespace" ){};
    struct str_new : TAO_PEGTL_STRING( "new" ){};
    struct str_null : TAO_PEGTL_STRING( "null" ){};
    struct str_Object : TAO_PEGTL_STRING( "Object" ){};
    struct str_out : TAO_PEGTL_STRING( "out" ){};
    struct str_override : TAO_PEGTL_STRING( "override" ){};
    struct str_param : TAO_PEGTL_STRING( "param" ){};
    struct str_partial : TAO_PEGTL_STRING( "partial" ){};
    struct str_pragma : TAO_PEGTL_STRING( "pragma" ){};
    struct str_private : TAO_PEGTL_STRING( "private" ){};
    struct str_property : TAO_PEGTL_STRING( "property" ){};
    struct str_protected : TAO_PEGTL_STRING( "protected" ){};
    struct str_public : TAO_PEGTL_STRING( "public" ){};
    struct str_ref : TAO_PEGTL_STRING( "ref" ){};
    struct str_region : TAO_PEGTL_STRING( "region" ){};
    struct str_remove : TAO_PEGTL_STRING( "remove" ){};
    struct str_restore : TAO_PEGTL_STRING( "restore" ){};
    struct str_return : TAO_PEGTL_STRING( "return" ){};
    struct str_sealed : TAO_PEGTL_STRING( "sealed" ){};
    struct str_set : TAO_PEGTL_STRING( "set" ){};
    struct str_Single : TAO_PEGTL_STRING( "Single" ){};
    struct str_static : TAO_PEGTL_STRING( "static" ){};
    struct str_String : TAO_PEGTL_STRING( "String" ){};
    struct str_struct : TAO_PEGTL_STRING( "struct" ){};
    struct str_true : TAO_PEGTL_STRING( "true" ){};
    struct str_type : TAO_PEGTL_STRING( "type" ){};
    struct str_UInt8 : TAO_PEGTL_STRING( "UInt8" ){};
    struct str_UInt16 : TAO_PEGTL_STRING( "UInt16" ){};
    struct str_UInt32 : TAO_PEGTL_STRING( "UInt32" ){};
    struct str_UInt64 : TAO_PEGTL_STRING( "UInt64" ){};
    struct str_undef : TAO_PEGTL_STRING( "undef" ){};
    struct str_using : TAO_PEGTL_STRING( "using" ){};
    struct str_virtual : TAO_PEGTL_STRING( "virtual" ){};
    struct str_void : TAO_PEGTL_STRING( "void" ){};
    struct str_warning : TAO_PEGTL_STRING( "warning" ){};

    struct str_keyword : peg::sor<
        str_abstract,
        str_add,
        str_assembly,
        str_attribute,
        str_Boolean,
        str_Char,
        str_class,
        str_const,
        str_default,
        str_define,
        str_delegate,
        str_disable,
        str_Double,
        str_elif,
        str_else,
        str_endif,
        str_endregion,
        str_enum,
        str_error,
        str_event,
        str_false,
        str_field,
        str_get,
        str_Guid,
        str_hidden,
        str_if,
        str_in,
        str_Int8,
        str_Int16,
        str_Int32,
        str_Int64,
        str_interface,
        str_internal,
        str_line,
        str_method,
        str_module,
        str_namespace,
        str_new,
        str_null,
        str_Object,
        str_out,
        str_override,
        str_param,
        str_partial,
        str_pragma,
        str_private,
        str_property,
        str_protected,
        str_public,
        str_ref,
        str_region,
        str_remove,
        str_restore,
        str_return,
        str_sealed,
        str_set,
        str_Single,
        str_static,
        str_String,
        str_struct,
        str_true,
        str_type,
        str_UInt8,
        str_UInt16,
        str_UInt32,
        str_UInt64,
        str_undef,
        str_using,
        str_virtual,
        str_void,
        str_warning > {};

    template< typename Key >
    struct key : peg::seq< 
        Key, 
        peg::not_at< peg::identifier_other > > {};

    struct key_abstract : key< str_abstract > {};
    struct key_add : key< str_add > {};
    struct key_assembly : key< str_assembly > {};
    struct key_attribute : key< str_attribute > {};
    struct key_Boolean : key< str_Boolean > {};
    struct key_Char : key< str_Char > {};
    struct key_class : key< str_class > {};
    struct key_const : key< str_const > {};
    struct key_default : key< str_default > {};
    struct key_define : key< str_define > {};
    struct key_delegate : key< str_delegate > {};
    struct key_disable : key< str_disable > {};
    struct key_Double : key< str_Double > {};
    struct key_elif : key< str_elif > {};
    struct key_else : key< str_else > {};
    struct key_endif : key< str_endif > {};
    struct key_endregion : key< str_endregion > {};
    struct key_enum : key< str_enum > {};
    struct key_error : key< str_error > {};
    struct key_event : key< str_event > {};
    struct key_false : key< str_false > {};
    struct key_field : key< str_field > {};
    struct key_get : key< str_get > {};
    struct key_Guid : key< str_Guid > {};
    struct key_hidden : key< str_hidden > {};
    struct key_if : key< str_if > {};
    struct key_in : key< str_in > {};
    struct key_Int16 : key< str_Int16 > {};
    struct key_Int32 : key< str_Int32 > {};
    struct key_Int64 : key< str_Int64 > {};
    struct key_Int8 : key< str_Int8 > {};
    struct key_interface : key< str_interface > {};
    struct key_internal : key< str_internal > {};
    struct key_line : key< str_line > {};
    struct key_method : key< str_method > {};
    struct key_module : key< str_module > {};
    struct key_namespace : key< str_namespace > {};
    struct key_new : key< str_new > {};
    struct key_null : key< str_null > {};
    struct key_Object : key< str_Object > {};
    struct key_out : key< str_out > {};
    struct key_override : key< str_override > {};
    struct key_param : key< str_param > {};
    struct key_partial : key< str_partial > {};
    struct key_pragma : key< str_pragma > {};
    struct key_private : key< str_private > {};
    struct key_property : key< str_property > {};
    struct key_protected : key< str_protected > {};
    struct key_public : key< str_public > {};
    struct key_ref : key< str_ref > {};
    struct key_region : key< str_region > {};
    struct key_remove : key< str_remove > {};
    struct key_restore : key< str_restore > {};
    struct key_return : key< str_return > {};
    struct key_sealed : key< str_sealed > {};
    struct key_set : key< str_set > {};
    struct key_Single : key< str_Single > {};
    struct key_static : key< str_static > {};
    struct key_String : key< str_String > {};
    struct key_struct : key< str_struct > {};
    struct key_true : key< str_true > {};
    struct key_type : key< str_type > {};
    struct key_UInt16 : key< str_UInt16 > {};
    struct key_UInt32 : key< str_UInt32 > {};
    struct key_UInt64 : key< str_UInt64 > {};
    struct key_UInt8 : key< str_UInt8 > {};
    struct key_undef : key< str_undef > {};
    struct key_using : key< str_using > {};
    struct key_virtual : key< str_virtual > {};
    struct key_void : key< str_void > {};
    struct key_warning : key< str_warning > {};

    struct keyword : key< str_keyword > {};

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
    
    // 2.4.4.1 Boolean Literals
    struct boolean_literal : peg::sor< 
        key_true,
        key_false > {};

    // 2.4.4.2 Integer literals
    struct integer_type_suffix : peg::sor< 
        TAO_PEGTL_STRING("UL"), TAO_PEGTL_STRING("Ul"), 
        TAO_PEGTL_STRING("uL"), TAO_PEGTL_STRING("ul"), 
        TAO_PEGTL_STRING("LU"), TAO_PEGTL_STRING("Lu"), 
        TAO_PEGTL_STRING("lU"), TAO_PEGTL_STRING("lu"), 
        peg::one< 'U', 'u', 'L', 'l' > > {};
    struct hexadecimal_integer_literal : peg::seq<
        peg::one< '0' >,
        peg::one< 'x', 'X' >,
        peg::plus< peg::xdigit > > {};
    struct binary_integer_literal : peg::seq<
        peg::one< '0' >,
        peg::one< 'b', 'b' >,
        peg::plus< peg::one< '0', '1' > > > {};
    struct decimal_integer_literal : peg::plus< peg::digit > {};
    struct integer_literal : peg::seq< 
        peg::sor< 
            hexadecimal_integer_literal,
            binary_integer_literal,
            decimal_integer_literal >,
        peg::opt< integer_type_suffix > > {};

    // 2.4.4.3 Real literals
    struct real_type_suffix : peg::one< 'F', 'f', 'D', 'd' > {};
    struct sign : peg::one< '+', '-' > {};
    struct exponent_part : peg::seq< 
        peg::one< 'e', 'E' >,
        peg::opt< sign >,
        peg::plus< peg::digit > > {};
    struct real_literal : peg::seq<
        peg::sor< 
            peg::seq< 
                peg::plus< peg::digit >, 
                peg::one< '.' >, 
                peg::star< peg::digit >, 
                peg::opt< exponent_part >, 
                peg::opt< real_type_suffix > >, 
            peg::seq< 
                peg::one< '.' >, 
                peg::plus< peg::digit >, 
                peg::opt< exponent_part >, 
                peg::opt< real_type_suffix > >, 
            peg::seq< 
                peg::plus< peg::digit >, 
                exponent_part, 
                peg::opt< real_type_suffix > >, 
            peg::seq< 
                peg::plus< peg::digit >, 
                real_type_suffix > > > {};

    // 2.4.4.4 Character literals
    struct hexadecimal_escape_sequence : peg::seq< 
        peg::string< '\\', 'x' >, 
        peg::xdigit, peg::opt< peg::xdigit >, peg::opt< peg::xdigit >, peg::opt< peg::xdigit > > {};
    struct simple_escape_sequence : peg::seq<
        peg::one< '\\' >,
        peg::one< '\'' , '"', '\\', '0', 'a', 'b', 'f', 'n', 'r', 't', 'v' > > {};
    struct single_character : peg::seq<
        peg::not_at< peg::one< '\'',  '\\' > >,
        peg::any > {};
    struct character : peg::sor< 
        single_character, 
        simple_escape_sequence, 
        hexadecimal_escape_sequence, 
        unicode_escape_sequence > {};
    struct character_literal : peg::seq<
        peg::seq< 
            peg::one< '\'' >, 
            character, 
            peg::one< '\'' > > > {};

    // 2.4.4.5 String literals
    struct single_regular_string_literal_character : peg::seq<
        peg::not_at< peg::one< '"', '\\' > >,
        peg::not_at< peg::eol >,
        peg::any > {};
    struct regular_string_literal_character : peg::sor< 
        single_regular_string_literal_character, 
        simple_escape_sequence, 
        hexadecimal_escape_sequence, 
        unicode_escape_sequence > {};
    struct verbatim_string_literal_character : peg::sor< 
        peg::seq< 
            peg::not_at< peg::one< '"' > >,
            peg::any >, 
        peg::string< '"', '"' > > {};
    struct verbatim_string_literal : peg::seq< 
        peg::string< '@', '"' >, 
        peg::star< verbatim_string_literal_character >, 
        peg::one< '"' > > {};
    struct regular_string_literal : peg::seq< 
        peg::one< '"' >, 
        peg::star< regular_string_literal_character >, 
        peg::one< '"' > > {};
    struct string_literal :  peg::seq<
        peg::sor< 
            regular_string_literal, 
            verbatim_string_literal > > {};

    // 2.4.4.6 Null Literal
    struct null_literal : peg::seq< key_null > {};

    // 2.4.5 Operators and punctuators

    // 2.5 Pre-processing directives
    struct pp_declaration;
    struct pp_conditional;
    struct pp_line;
    struct pp_diagnostic;
    struct pp_region;
    struct pp_pragma;
    struct pp_directive : peg::sor< 
        pp_declaration, 
        pp_conditional, 
        pp_line, 
        pp_diagnostic, 
        pp_region, 
        pp_pragma > {};

    // 2.5.1 Conditional compilation symbols
    struct conditional_symbol : peg::seq<
        peg::not_at< boolean_literal >,
        peg::identifier > {};

    // 2.5.2 Pre-processing expressions
    //       Note, reconsider seps vs ws here
    template< typename S, typename O >
    struct left_assoc : peg::seq< S, seps, peg::star_must< O, seps, S, seps > > {};
    template< typename S, typename O >
    struct right_assoc : peg::seq< S, seps, peg::opt_must< O, seps, right_assoc< S, O > > > {};

    struct pp_expression;
    struct pp_primary_expression : peg::seq<
        key_true,
        key_false, 
        conditional_symbol,
        peg::seq<
            peg::one< '(' >,
            seps,
            pp_expression,
            seps,
            peg::one< ')' > > > {};
    struct pp_unary_expression : peg::seq<
        pp_primary_expression, 
        peg::if_must< peg::one< '!' >, seps, pp_unary_expression > > {};
    struct pp_equality_expression : peg::sor<
        pp_unary_expression,
        left_assoc< pp_unary_expression, peg::string< '=', '=' > >,
        left_assoc< pp_unary_expression, peg::string< '!', '=' > > > {};
    struct pp_and_expression : left_assoc< pp_equality_expression, peg::string< '&', '&' > > {};
    struct pp_or_expression : left_assoc< pp_and_expression, peg::string< '|', '|' > > {};
    struct pp_expression : peg::pad< pp_or_expression, seps > {};

    // 2.5.3 Declaration directives
    struct pp_new_line : peg::seq<
        peg::star< peg::ascii::blank >,
        peg::sor<
            single_line_comment,
            peg::eol > > {};

    struct pp_declaration : peg::sor< 
        peg::seq< 
            peg::one< '#' >, 
            peg::star< peg::ascii::blank >, 
            peg::sor<
                key_define,
                key_undef >,
            peg::plus< peg::ascii::blank >, 
            conditional_symbol,
            pp_new_line > > {}; 

    // 2.5.4 Conditional compilation directives
    // struct input_section; // TODO
    // struct skipped_section_part; // : peg::sor< peg::seq< peg::opt< skipped_characters >, new_line >, pp_directive > {};
    // struct skipped_section : peg::star< skipped_section_part > {};
    // struct conditional_section : peg::sor< 
    //     input_section, 
    //     skipped_section > {};
    // struct pp_if_section : peg::sor< 
    //     peg::seq< 
    //         peg::one< '#' >, 
    //         peg::star< peg::blanks >, 
    //         peg::sor<
    //             key_define,
    //             key_undef >,
    //         peg::plus< peg::blanks >, 
    //         conditional_symbol,
    //         pp_new_line > > {}; 

    // struct pp_elseif_section : peg::seq< 
    //     peg::one< '#' >, 
    //     peg::star< peg::ascii::blanks >, 
    //     key_elseif,
    //     pp_new_line
    //     peg::opt< conditional_section > > {};

    // struct pp_else_section : peg::seq< 
    //     peg::one< '#' >, 
    //     peg::star< peg::ascii::blanks >, 
    //     key_else,
    //     pp_new_line
    //     peg::opt< conditional_section > > {};

    // struct pp_endif : peg::seq< 
    //     peg::one< '#' >, 
    //     peg::star< peg::ascii::blanks >, 
    //     key_endif,
    //     pp_new_line > {};

    // struct pp_conditional : peg::seq< 
    //     pp_if_section, 
    //     peg::star< pp_elif_section >, 
    //     peg::opt< pp_else_section >, 
    //     pp_endif > {};

    // 2.5.5 Diagnostic directives

    // 2.5.6 Region directives

    // 2.5.7 Line directives

    // 2.5.8 Pragma directives




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

    // const std::size_t issues_found = tao::pegtl::analyze< cdl::comment >();

    // auto root = parse_tree::parse<cdl::identifier, cdl::selector>(memory_input("_namespace", ""));
    // auto hello_root = parse_tree::parse<hello::grammar>(memory_input("hello, xlang!", ""));

    // string_input<> in("   namespace xlang  { } namespace Windows.Foundation  { }  ", "");
    // auto root = parse_tree::parse<xlangidl::grammar, xlangidl::selector>(in);

    // std::string name;
    // parse<hello::grammar, hello::action>(in, name);

    // writer w;
    // if (root)
    // {
    //     print_node(w, *root);
    // }
    // else
    // {
    //     w.write("fail\n");
    // }
    // w.flush_to_console();
}
