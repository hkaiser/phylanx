// Copyright (c) 2018 Hartmut Kaiser
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/config.hpp>
#include <phylanx/plugins/constants_of_nature/constants_of_nature.hpp>

#include <hpx/include/lcos.hpp>
#include <hpx/include/util.hpp>
#include <hpx/throw_exception.hpp>

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace phylanx { namespace execution_tree { namespace primitives
{
    constexpr char const* const help_string = R"(
        con(name)
        Args:

            name (string) : either `"e"`, `"pi"`, or `"ua"`

        Returns:

            the value of either `"e"`, `"pi"`, or `"ua"`
        )";

    ///////////////////////////////////////////////////////////////////////////
    match_pattern_type const constants_of_nature::match_data =
    {
        hpx::util::make_tuple("con",
            std::vector<std::string>{"con(_1)"},
            &create_constants_of_nature,
            &create_primitive<constants_of_nature>,
            help_string
        )
    };

    match_pattern_type const constants_of_nature::match_data_e =
    {
        hpx::util::make_tuple("e",
            std::vector<std::string>{"e()"},
            &create_constants_of_nature,
            &create_primitive<constants_of_nature>,
            help_string
        )
    };

    match_pattern_type const constants_of_nature::match_data_pi =
    {
        hpx::util::make_tuple("pi",
            std::vector<std::string>{"pi()"},
            &create_constants_of_nature,
            &create_primitive<constants_of_nature>,
            help_string
        )
    };

    match_pattern_type const constants_of_nature::match_data_ua =
    {
        hpx::util::make_tuple("ua",
            std::vector<std::string>{"ua()"},
            &create_constants_of_nature,
            &create_primitive<constants_of_nature>,
            help_string
        )
    };

    namespace detail
    {
        std::string extract_function_name(std::string const& name)
        {
            compiler::primitive_name_parts name_parts;
            if (!compiler::parse_primitive_name(name, name_parts))
            {
                return name;
            }
            return name_parts.primitive;
        }
    }

    constants_of_nature::constants_of_nature(
            std::vector<primitive_argument_type>&& operands,
            std::string const& name, std::string const& codename)
      : primitive_component_base(std::move(operands), name, codename)
    {}

    double constants_of_nature::calculate_constant(std::string const& name) const
    {
        if (name == "e")
        {
            return 2.718281828459045235360287471352662497757247093699959574966;
        }
        else if (name == "pi")
        {
            return 3.141592653589793238462643383279502884197169399375105820974;
        }
        else if (name == "ua")
        {
            return 42;
        }

        HPX_THROW_EXCEPTION(hpx::bad_parameter,
            "phylanx::execution_tree::primitives::constants_of_nature::eval",
            generate_error_message(
                "unknown constant of nature requested: " + name));
    }

    hpx::future<primitive_argument_type> constants_of_nature::eval_helper(
        std::vector<primitive_argument_type> const& operands,
        std::vector<primitive_argument_type> const& args) const
    {
        if (operands.size() > 1)
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::constants_of_nature::eval",
                generate_error_message(
                    "constants_of_nature accepts either none or exactly one "
                    "argument"));
        }

        if (operands.empty())
        {
            // no arguments, derive functionality from primitive name
            return hpx::make_ready_future(primitive_argument_type{
                calculate_constant(detail::extract_function_name(name_))});
        }

        hpx::future<std::string> f = string_operand(operands[0], args, name_, codename_);

        auto this_ = this->shared_from_this();
        return f.then(
                [this_](hpx::future<std::string> val) -> primitive_argument_type
                {
                    return primitive_argument_type{
                        this_->calculate_constant(val.get())};
                });

    }

    hpx::future<primitive_argument_type> constants_of_nature::eval(
        std::vector<primitive_argument_type> const& args) const
    {
        if (this->no_operands())
        {
            return eval_helper(args, noargs);
        }
        return eval_helper(this->operands(), args);
    }
}}}
