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
    // http://bford.info/packrat/
    // https://github.com/taocpp/PEGTL

    namespace peg = tao::pegtl;

    // 1. Introduction
    //    This section is an intro to the type system.

    // 2. Lexical structure
    // 2.1 Declaration
    // 2.2 Grammars
    // 2.3 Lexical analysis
    //     Note, this grammar is written with the assumption of being implemented with
    //     seperate lexing and parsing steps. This will need to change in order to be 
    //     implemented as a recursive decent parser.
    struct new_line;
    struct pp_directive;
    struct whitespace;
    struct comment;
    struct token;

    struct input_element : peg::sor<
        whitespace,
        comment,
        token > {};
    struct input_elements : peg::plus< input_element > {};
    struct input_section_part : peg::sor< 
        peg::seq< 
            peg::opt< input_elements >, 
            peg::eol >, // Note see 2.3.1 below regarding line termination 
        pp_directive > {};
    struct input_section : peg::plus< input_section_part > {};
    struct input : peg::opt< input_section > {};

    // 2.3.1 Line terminators
    //       Note, Spec calls for multiple line terminators, but PEGTL appears to handle
    //       line termination as part of the input class so simply use peg::eol instead of new_line

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
    //       Note, spec calls for all of Zs unicode class to be included in whitespace
    //       For now, explicitly referencing space character an ignoring the rest of Zs
    struct whitespace : peg::sor<
        peg::one< ' ' >, // TODO: all of Zs unicode class
        peg::one< '\u0009' >,
        peg::one< '\u000B' >,
        peg::one< '\u000C' > > {};

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
            peg::xdigit, peg::xdigit, peg::xdigit, peg::xdigit >,
        peg::seq< 
            peg::string< '\\', 'U' >, 
            peg::xdigit, peg::xdigit, peg::xdigit, peg::xdigit,
            peg::xdigit, peg::xdigit, peg::xdigit, peg::xdigit > > {};

    // 2.4.2 Identifiers
    //       Note, spec calls for more unicode classes as valid identifier_start_character
    //       and identifier_part_character choices. Simply using ascii (peg::alpha and peg::digit)
    //       values for now.
    struct keyword;
    struct letter_character : peg::seq< peg::alpha > {}; // TODO, add remaining unicode catgory values
    struct decimal_digit_character : peg::seq< peg::digit > {}; // TODO, add remaining unicode catgory values
    struct connecting_character : peg::one< '_' > {}; // TODO, add remaining unicode catgory values
    struct identifier_part_character : peg::sor< // TODO, add combining-character and formatting-character
        letter_character,
        decimal_digit_character,
        connecting_character > {};
    struct identifier_part_characters : peg::plus< identifier_part_character > {};
    struct identifier_start_character : peg::sor< 
        letter_character, 
        peg::one< '_' > >{};
    struct identifier_or_keyword : peg::seq< 
        identifier_start_character, 
        peg::opt< identifier_part_characters > > {};
    struct available_identifier : peg::seq<
        peg::not_at< keyword >,
        identifier_or_keyword > {};
    struct identifier : peg::sor< 
        available_identifier,
        peg::seq< 
            peg::one< '@' >, 
            identifier_or_keyword > >{};

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
    struct str_Int16 : TAO_PEGTL_STRING( "Int16" ){};
    struct str_Int32 : TAO_PEGTL_STRING( "Int32" ){};
    struct str_Int64 : TAO_PEGTL_STRING( "Int64" ){};
    struct str_Int8 : TAO_PEGTL_STRING( "Int8" ){};
    struct str_interface : TAO_PEGTL_STRING( "interface" ){};
    struct str_internal : TAO_PEGTL_STRING( "internal" ){};
    struct str_line : TAO_PEGTL_STRING( "line" ){};
    struct str_long : TAO_PEGTL_STRING( "long" ){};
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
    struct str_short : TAO_PEGTL_STRING( "short" ){};
    struct str_Single : TAO_PEGTL_STRING( "Single" ){};
    struct str_static : TAO_PEGTL_STRING( "static" ){};
    struct str_String : TAO_PEGTL_STRING( "String" ){};
    struct str_struct : TAO_PEGTL_STRING( "struct" ){};
    struct str_true : TAO_PEGTL_STRING( "true" ){};
    struct str_type : TAO_PEGTL_STRING( "type" ){};
    struct str_UInt16 : TAO_PEGTL_STRING( "UInt16" ){};
    struct str_UInt32 : TAO_PEGTL_STRING( "UInt32" ){};
    struct str_UInt64 : TAO_PEGTL_STRING( "UInt64" ){};
    struct str_UInt8 : TAO_PEGTL_STRING( "UInt8" ){};
    struct str_undef : TAO_PEGTL_STRING( "undef" ){};
    struct str_using : TAO_PEGTL_STRING( "using" ){};
    struct str_virtual : TAO_PEGTL_STRING( "virtual" ){};
    struct str_void : TAO_PEGTL_STRING( "void" ){};
    struct str_warning : TAO_PEGTL_STRING( "warning" ){};

    template< typename Key >
    struct key : peg::seq< Key, peg::not_at< peg::identifier_other > > {};

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
    struct key_long : key< str_long > {};
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
    struct key_short : key< str_short > {};
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

    struct keyword : peg::sor<
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
        str_Int16,
        str_Int32,
        str_Int64,
        str_Int8,
        str_interface,
        str_internal,
        str_line,
        str_long,
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
        str_short,
        str_Single,
        str_static,
        str_String,
        str_struct,
        str_true,
        str_type,
        str_UInt16,
        str_UInt32,
        str_UInt64,
        str_UInt8,
        str_undef,
        str_using,
        str_virtual,
        str_void,
        str_warning > {};

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
    struct decimal_digit : peg::seq< peg::digit > {};
    struct decimal_digits : peg::plus< decimal_digit > {};
    struct hex_digit : peg::seq< peg::xdigit > {};
    struct hex_digits : peg::plus< hex_digit > {};
    struct integer_type_suffix : peg::sor< 
        peg::one< 'U' >, peg::one< 'u' >, 
        peg::one< 'L' >, peg::one< 'l' >, 
        peg::string< 'U', 'L' >, peg::string< 'U', 'l' >, 
        peg::string< 'u', 'L' >, peg::string< 'u', 'l' >, 
        peg::string< 'L', 'U' >, peg::string< 'L', 'u' >, 
        peg::string< 'l', 'U' >, peg::string< 'l', 'u' > > {};
    struct hexadecimal_integer_literal : peg::seq<
        peg::one< '0' >,
        peg::sor< peg::one< 'x' >, peg::one< 'X' >>,
        hex_digits,
        peg::opt< integer_type_suffix > > {};
    struct decimal_integer_literal : peg::seq< 
        decimal_digits, 
        peg::opt< integer_type_suffix > > {};
    struct integer_literal : peg::sor< 
        decimal_integer_literal, 
        hexadecimal_integer_literal > {};

    // 2.4.4.3 Real literals
    struct real_type_suffix : peg::sor< 
        peg::one< 'F' >, peg::one< 'f' >, 
        peg::one< 'D' >, peg::one< 'd' > > {};
    struct sign : peg::sor< 
        peg::one< '+' >, peg::one< '-' > > {};
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
        hex_digit, 
        peg::opt< hex_digit >, 
        peg::opt< hex_digit >, 
        peg::opt< hex_digit > > {};
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
    //       note, angle bracket operators will likely need special handling
    //       to support usage as both shift operators and type param markers 
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
    //     Note, skipping PP rules for now

    // 3. Basic concepts
    // 3.1 Declarations
    // 3.2 Members
    // 3.3 Member access
    // 3.4 Signatures and overloading
    // 3.5 Scopes
    //     Overview of the CDL file layout 

    // 3.6 Namespace and type names
    //     Note, namespaces are not allowed to have type arguments, so perhaps 
    //     type_name and namespace_name should have separate definitions?
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

    // 7.1 Class declarations
    struct class_declaration;

    // 7.1.1 Class modifiers
    struct class_modifier : peg::sor< 
        peg::string< 's', 'e', 'a', 'l', 'e', 'd' >, 
        peg::string< 's', 't', 'a', 't', 'i', 'c' > > {};
    struct class_modifiers : peg::plus< class_modifier > {};

    // 7.1.2 Partial modifier

    // 7.1.3 Type parameters 
    // WinRT classes don't support type parameterization, so not sure why this section is here

    // // 7.1.4 Class base specification
    // struct interface_static_modifier : peg::string< 's', 't', 'a', 't', 'i', 'c' > {};
    // struct interface_type_list : peg::sor<
    //     peg::seq<
    //         peg::opt< attributes >, 
    //         peg::opt< interface_static_modifier >,
    //         interface_type >,
        
 

    //     interface_type, peg::star< peg::seq< peg::one< ',' >, interface_type > > > {};
    // struct class_base : peg::sor< 
    //     peg::seq< 
    //         peg::one< ':' >, 
    //         class_type >, 
    //     peg::seq< 
    //         peg::one< ':' >, 
    //         interface_type_list >, 
    //     peg::seq< 
    //         peg::one< ':' >, 
    //         class_type, 
    //         peg::one< ',' >, 
    //         interface_type_list > > {};

    // struct attributes;
    // struct type_parameter_list;
    // struct class_base;
    // struct class_body;
    // struct class_declaration : peg::seq< 
    //     peg::opt< attributes >, 
    //     peg::opt< class_modifiers >, 
    //     peg::opt< peg::string< 'p', 'a', 'r', 't', 'i', 'a', 'l' > >, 
    //     peg::string< 'c', 'l', 'a', 's', 's' >, 
    //     identifier,
    //     peg::opt< type_parameter_list >, 
    //     peg::opt< class_base >, 
    //     class_body, 
    //     peg::opt< peg::one< ';' > > > {};


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
