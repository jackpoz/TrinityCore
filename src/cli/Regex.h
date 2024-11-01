#pragma once

#define TC_REGEX_NAMESPACE clidisabled

namespace clidisabled
{
    class regex
    {
    public:
        enum flag_type_
        {
            //
            // Divide the flags up into logical groups:
            // bits 0-7 indicate main synatx type.
            // bits 8-15 indicate syntax subtype.
            // bits 16-31 indicate options that are common to all
            // regex syntaxes.
            // In all cases the default is 0.
            //
            // Main synatx group:
            //
            perl_syntax_group = 0,                      // default
            basic_syntax_group = 1,                     // POSIX basic
            literal = 2,                                // all characters are literals
            main_option_type = literal | basic_syntax_group | perl_syntax_group, // everything!
            //
            // options specific to perl group:
            //
            no_bk_refs = 1 << 8,                        // \d not allowed
            no_perl_ex = 1 << 9,                        // disable perl extensions
            no_mod_m = 1 << 10,                         // disable Perl m modifier
            mod_x = 1 << 11,                            // Perl x modifier
            mod_s = 1 << 12,                            // force s modifier on (overrides match_not_dot_newline)
            no_mod_s = 1 << 13,                         // force s modifier off (overrides match_not_dot_newline)

            //
            // options specific to basic group:
            //
            no_char_classes = 1 << 8,                   // [[:CLASS:]] not allowed
            no_intervals = 1 << 9,                      // {x,y} not allowed
            bk_plus_qm = 1 << 10,                       // uses \+ and \?
            bk_vbar = 1 << 11,                          // use \| for alternatives
            emacs_ex = 1 << 12,                         // enables emacs extensions

            //
            // options common to all groups:
            //
            no_escape_in_lists = 1 << 16,                     // '\' not special inside [...]
            newline_alt = 1 << 17,                            // \n is the same as |
            no_except = 1 << 18,                              // no exception on error
            failbit = 1 << 19,                                // error flag
            icase = 1 << 20,                                  // characters are matched regardless of case
            nocollate = 0,                                    // don't use locale specific collation (deprecated)
            collate = 1 << 21,                                // use locale specific collation
            nosubs = 1 << 22,                                 // don't mark sub-expressions
            save_subexpression_location = 1 << 23,            // save subexpression locations
            no_empty_expressions = 1 << 24,                   // no empty expressions allowed
            optimize = 0,                                     // not really supported



            basic = basic_syntax_group | collate | no_escape_in_lists,
            extended = no_bk_refs | collate | no_perl_ex | no_escape_in_lists,
            normal = 0,
            emacs = basic_syntax_group | collate | emacs_ex | bk_vbar,
            awk = no_bk_refs | collate | no_perl_ex,
            grep = basic | newline_alt,
            egrep = extended | newline_alt,
            sed = basic,
            perl = normal,
            ECMAScript = normal,
            JavaScript = normal,
            JScript = normal
        };
        typedef unsigned int flag_type;
    };

    class wregex
    {
    public:
        enum flag_type_
        {
            //
            // Divide the flags up into logical groups:
            // bits 0-7 indicate main synatx type.
            // bits 8-15 indicate syntax subtype.
            // bits 16-31 indicate options that are common to all
            // regex syntaxes.
            // In all cases the default is 0.
            //
            // Main synatx group:
            //
            perl_syntax_group = 0,                      // default
            basic_syntax_group = 1,                     // POSIX basic
            literal = 2,                                // all characters are literals
            main_option_type = literal | basic_syntax_group | perl_syntax_group, // everything!
            //
            // options specific to perl group:
            //
            no_bk_refs = 1 << 8,                        // \d not allowed
            no_perl_ex = 1 << 9,                        // disable perl extensions
            no_mod_m = 1 << 10,                         // disable Perl m modifier
            mod_x = 1 << 11,                            // Perl x modifier
            mod_s = 1 << 12,                            // force s modifier on (overrides match_not_dot_newline)
            no_mod_s = 1 << 13,                         // force s modifier off (overrides match_not_dot_newline)

            //
            // options specific to basic group:
            //
            no_char_classes = 1 << 8,                   // [[:CLASS:]] not allowed
            no_intervals = 1 << 9,                      // {x,y} not allowed
            bk_plus_qm = 1 << 10,                       // uses \+ and \?
            bk_vbar = 1 << 11,                          // use \| for alternatives
            emacs_ex = 1 << 12,                         // enables emacs extensions

            //
            // options common to all groups:
            //
            no_escape_in_lists = 1 << 16,                     // '\' not special inside [...]
            newline_alt = 1 << 17,                            // \n is the same as |
            no_except = 1 << 18,                              // no exception on error
            failbit = 1 << 19,                                // error flag
            icase = 1 << 20,                                  // characters are matched regardless of case
            nocollate = 0,                                    // don't use locale specific collation (deprecated)
            collate = 1 << 21,                                // use locale specific collation
            nosubs = 1 << 22,                                 // don't mark sub-expressions
            save_subexpression_location = 1 << 23,            // save subexpression locations
            no_empty_expressions = 1 << 24,                   // no empty expressions allowed
            optimize = 0,                                     // not really supported



            basic = basic_syntax_group | collate | no_escape_in_lists,
            extended = no_bk_refs | collate | no_perl_ex | no_escape_in_lists,
            normal = 0,
            emacs = basic_syntax_group | collate | emacs_ex | bk_vbar,
            awk = no_bk_refs | collate | no_perl_ex,
            grep = basic | newline_alt,
            egrep = extended | newline_alt,
            sed = basic,
            perl = normal,
            ECMAScript = normal,
            JavaScript = normal,
            JScript = normal
        };
        typedef unsigned int flag_type;

        wregex()
        {
        }

        wregex(std::wstring p, int f)
        {
        }
    };

    class regex_match
    {

    };

    class regex_search
    {

    };


    bool regex_search(const std::wstring& s, wregex w)
    {
        return false;
    }
}

namespace Trinity
{
    using regex = TC_REGEX_NAMESPACE::regex;
    using wregex = TC_REGEX_NAMESPACE::wregex;

    using ::TC_REGEX_NAMESPACE::regex_match;
    using ::TC_REGEX_NAMESPACE::regex_search;
}
