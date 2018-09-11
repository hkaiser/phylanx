// Copyright (c) 2018 Hartmut Kaiser
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(PHYLANX_CONSTANTS_OF_NATURE)
#define PHYLANX_CONSTANTS_OF_NATURE

#include <phylanx/config.hpp>
#include <phylanx/execution_tree/primitives/base_primitive.hpp>
#include <phylanx/execution_tree/primitives/primitive_component_base.hpp>

#include <hpx/lcos/future.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace phylanx { namespace execution_tree { namespace primitives
{
    class constants_of_nature
      : public primitive_component_base
      , public std::enable_shared_from_this<constants_of_nature>
    {
    protected:
        hpx::future<primitive_argument_type> eval_helper(
            std::vector<primitive_argument_type> const& operands,
            std::vector<primitive_argument_type> const& args) const;

        double calculate_constant(std::string const& name) const;

    public:
        static match_pattern_type const match_data;
        static match_pattern_type const match_data_e;
        static match_pattern_type const match_data_pi;
        static match_pattern_type const match_data_ua;

        constants_of_nature() = default;

        constants_of_nature(std::vector<primitive_argument_type> && args,
            std::string const& name, std::string const& codename);

        hpx::future<primitive_argument_type> eval(
            std::vector<primitive_argument_type> const& params) const override;
    };

    inline primitive create_constants_of_nature(hpx::id_type const& locality,
        primitive_arguments_type&& operands,
        std::string const& name = "", std::string const& codename = "")
    {
        return create_primitive_component(
            locality, "con", std::move(operands), name, codename);
    }
}}}

#endif
