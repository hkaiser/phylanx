// Copyright (c) 2017 Bibek Wagle
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/config.hpp>
#include <phylanx/execution_tree/primitives/for_operation.hpp>
#include <phylanx/ir/node_data.hpp>
#include <phylanx/util/optional.hpp>
#include <phylanx/util/serialization/ast.hpp>
#include <phylanx/util/serialization/optional.hpp>

#include <hpx/include/components.hpp>
#include <hpx/include/lcos.hpp>

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
typedef hpx::components::component<
    phylanx::execution_tree::primitives::for_operation>
    for_operation_type;
HPX_REGISTER_DERIVED_COMPONENT_FACTORY(
    for_operation_type, phylanx_for_operation_component,
    "phylanx_primitive_component", hpx::components::factory_enabled)
HPX_DEFINE_GET_COMPONENT_TYPE(for_operation_type::wrapped_type)

///////////////////////////////////////////////////////////////////////////////
namespace phylanx { namespace execution_tree { namespace primitives
{
    ///////////////////////////////////////////////////////////////////////////
    std::vector<match_pattern_type> const for_operation::match_data =
    {
        hpx::util::make_tuple(
            "for", "for(_1, _2, _3, _4)", &create<for_operation>)
    };

    ///////////////////////////////////////////////////////////////////////////
    for_operation::for_operation(
            std::vector<primitive_argument_type>&& operands)
      : operands_(std::move(operands))
    {
        if (operands_.size() != 4)
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::for_operation::"
                    "for_operation",
                "the for_operation primitive requires exactly four arguments");
        }

        if (!valid(operands_[0]) || !valid(operands_[1])
            || !valid(operands_[2]) || !valid(operands_[3]))
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::for_operation::"
                    "for_operation",
                "the for_operation primitive requires that the arguments "
                    "given by the operands array are valid");
        }
    }

    namespace detail
    {
        struct iteration_for : std::enable_shared_from_this<iteration_for>
        {
            iteration_for(std::vector<primitive_argument_type> const& operands)
              : operands_(operands)
            {}

          hpx::future<primitive_result_type> init()
          {
            auto this_ = this->shared_from_this();
            return literal_operand(operands_[0]).then(
                [this_](auto val)
                {
                  val.get(); //this future should already be ready and hence not block
                  return this_->loop();
                });
          }

          hpx::future<primitive_result_type> reinit()
          {
            auto this_ = this->shared_from_this();
            return literal_operand(operands_[2]).then(
                [this_](auto val)
                {
                  val.get(); //this future should already be ready
                  // and hence not block

                  //call the loop again
                  return this_->loop();
                });
          }

            hpx::future<primitive_result_type> body(
                hpx::future<primitive_result_type>&& cond)
            {
                if (extract_boolean_value(cond.get()))
                {
                    // evaluate body of for statement
                    auto this_ = this->shared_from_this();
                    return literal_operand(operands_[3]).then(
                        [this_](
                            hpx::future<primitive_result_type> && result
                        ) mutable
                        {
                            this_->result_ = result.get();
                            //do the reinit statement
                            return  this_->reinit();
                        });
                }

                hpx::future<primitive_result_type> f = p_.get_future();
                p_.set_value(std::move(result_));
                return f;
            }

            hpx::future<primitive_result_type> loop()
            {
                // evaluate condition of for statement
                auto this_ = this->shared_from_this();
                return literal_operand(operands_[1]).then(
                    [this_](hpx::future<primitive_result_type> && cond)
                    {
                        return this_->body(std::move(cond));
                    });
            }

        private:
            std::vector<primitive_argument_type> operands_;
            hpx::promise<primitive_result_type> p_;
            primitive_result_type result_;
        };
    }

    // start iteration over given for statement
    hpx::future<primitive_result_type> for_operation::eval() const
    {
      return std::make_shared<detail::iteration_for>(operands_)->init();
    }
}}}
