//  Copyright (c) 2017-2018 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/config.hpp>
#include <phylanx/execution_tree/primitives/greater_equal.hpp>
#include <phylanx/ir/node_data.hpp>

#include <hpx/include/lcos.hpp>
#include <hpx/include/naming.hpp>
#include <hpx/include/util.hpp>
#include <hpx/throw_exception.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace phylanx { namespace execution_tree { namespace primitives
{
    ///////////////////////////////////////////////////////////////////////////
    primitive create_greater_equal(hpx::id_type const& locality,
        std::vector<primitive_argument_type>&& operands,
        std::string const& name, std::string const& codename)
    {
        static std::string type("__ge");
        return create_primitive_component(
            locality, type, std::move(operands), name, codename);
    }

    match_pattern_type const greater_equal::match_data =
    {
        hpx::util::make_tuple("__ge",
            std::vector<std::string>{"_1 >= _2"},
            &create_greater_equal, &create_primitive<greater_equal>)
    };

    ///////////////////////////////////////////////////////////////////////////
    greater_equal::greater_equal(std::vector<primitive_argument_type>&& operands,
            std::string const& name, std::string const& codename)
      : primitive_component_base(std::move(operands), name, codename)
    {}

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        struct greater_equal : std::enable_shared_from_this<greater_equal>
        {
            greater_equal(std::string const& name, std::string const& codename)
              : name_(name)
              , codename_(codename)
            {
            }

        protected:
            std::string name_;
            std::string codename_;

        protected:
            using operand_type = ir::node_data<double>;
            using operands_type = std::vector<primitive_argument_type>;

            primitive_argument_type greater_equal0d1d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                rhs.vector() = blaze::map(rhs.vector(),
                    [&](double x) { return (x >= lhs.scalar()); });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(rhs)});
            }

            primitive_argument_type greater_equal0d2d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                rhs.matrix() = blaze::map(rhs.matrix(),
                    [&](double x) { return (x >= lhs.scalar()); });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(rhs)});
            }

            primitive_argument_type greater_equal0d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t rhs_dims = rhs.num_dimensions();
                switch(rhs_dims)
                {
                case 0:
                    return primitive_argument_type(
                        ir::node_data<bool>{lhs.scalar() >= rhs.scalar()});

                case 1:
                    return greater_equal0d1d(std::move(lhs), std::move(rhs));

                case 2:
                    return greater_equal0d2d(std::move(lhs), std::move(rhs));

                default:
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal0d",
                        generate_error_message(
                            "the operands have incompatible number of "
                                "dimensions",
                            name_, codename_));
                }
            }

            primitive_argument_type greater_equal1d0d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                lhs.vector() = blaze::map(lhs.vector(),
                    [&](double x) { return (x >= rhs.scalar()); });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(lhs)});
            }

            primitive_argument_type greater_equal1d1d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t lhs_size = lhs.dimension(0);
                std::size_t rhs_size = rhs.dimension(0);

                if (lhs_size != rhs_size)
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal1d1d",
                        generate_error_message(
                            "the dimensions of the operands do not match",
                            name_, codename_));
                }

                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                lhs.vector() = blaze::map(lhs.vector(), rhs.vector(),
                    [&](double x, double y) { return (x >= y); });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(lhs)});
            }

            primitive_argument_type greater_equal1d2d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t lhs_size = lhs.dimension(0);
                auto rhs_size = rhs.dimensions();

                if (lhs_size != rhs_size[1])
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal1d2d",
                        generate_error_message(
                            "the dimensions of the operands do not match",
                            name_, codename_));
                }

                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                for (size_t i = 0UL; i < rhs.matrix().rows(); i++)
                    blaze::row(rhs.matrix(), i) =
                        blaze::map(blaze::row(rhs.matrix(), i),
                            blaze::trans(lhs.vector()),
                            [](double x, double y) { return x >= y; });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(rhs)});
            }

            primitive_argument_type greater_equal1d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t rhs_dims = rhs.num_dimensions();
                switch(rhs_dims)
                {
                case 0:
                    return greater_equal1d0d(std::move(lhs), std::move(rhs));

                case 1:
                    return greater_equal1d1d(std::move(lhs), std::move(rhs));

                case 2:
                    return greater_equal1d2d(std::move(lhs), std::move(rhs));

                default:
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal1d",
                        generate_error_message(
                            "the operands have incompatible number of "
                                "dimensions",
                            name_, codename_));
                }
            }

            primitive_argument_type greater_equal2d0d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t lhs_size = lhs.dimension(0);
                std::size_t rhs_size = rhs.dimension(0);

                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                lhs.matrix() = blaze::map(
                    lhs.matrix(), [&](double x) { return (x >= rhs.scalar()); });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(lhs)});
            }

            primitive_argument_type greater_equal2d1d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t rhs_size = rhs.dimension(0);
                auto lhs_size = lhs.dimensions();

                if (rhs_size != lhs_size[1])
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal2d1d",
                        generate_error_message(
                            "the dimensions of the operands do not match",
                            name_, codename_));
                }

                // TODO: SIMD functionality should be added, blaze implementation
                // is not currently available
                for (size_t i = 0UL; i < lhs.matrix().rows(); i++)
                    blaze::row(lhs.matrix(), i) =
                        blaze::map(blaze::row(lhs.matrix(), i),
                            blaze::trans(rhs.vector()),
                            [](double x, double y) { return x >= y; });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(lhs)});
            }

            primitive_argument_type greater_equal2d2d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                auto lhs_size = lhs.dimensions();
                auto rhs_size = rhs.dimensions();

                if (lhs_size != rhs_size)
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal2d2d",
                        generate_error_message(
                            "the dimensions of the operands do not match",
                            name_, codename_));
                }

                // TODO: SIMD functionality should be added, blaze implementation
                //       is not currently available
                lhs.matrix() = blaze::map(lhs.matrix(), rhs.matrix(),
                    [&](double x, double y) { return (x >= y); });

                return primitive_argument_type(
                    ir::node_data<bool>{std::move(lhs)});
            }

            primitive_argument_type greater_equal2d(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t rhs_dims = rhs.num_dimensions();
                switch(rhs_dims)
                {
                case 0:
                    return greater_equal2d0d(std::move(lhs), std::move(rhs));

                case 1:
                    return greater_equal2d1d(std::move(lhs), std::move(rhs));

                case 2:
                    return greater_equal2d2d(std::move(lhs), std::move(rhs));

                default:
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal2d",
                        generate_error_message(
                            "the operands have incompatible number of "
                                "dimensions",
                            name_, codename_));
                }
            }

        public:
            primitive_argument_type greater_equal_all(
                operand_type&& lhs, operand_type&& rhs) const
            {
                std::size_t lhs_dims = lhs.num_dimensions();
                switch (lhs_dims)
                {
                case 0:
                    return greater_equal0d(std::move(lhs), std::move(rhs));

                case 1:
                    return greater_equal1d(std::move(lhs), std::move(rhs));

                case 2:
                    return greater_equal2d(std::move(lhs), std::move(rhs));

                default:
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::greater_equal_all",
                        generate_error_message(
                            "left hand side operand has unsupported number of "
                                "dimensions",
                            name_, codename_));
                }
            }

        protected:
            struct visit_greater_equal
            {
                template <typename T1, typename T2>
                primitive_argument_type operator()(T1, T2) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side are incompatible "
                                "and can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                primitive_argument_type operator()(
                    ir::node_data<primitive_argument_type>&&,
                    ir::node_data<primitive_argument_type>&&) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side are incompatible "
                                "and can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                primitive_argument_type operator()(std::vector<ast::expression>&&,
                    std::vector<ast::expression>&&) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side are incompatible "
                                "and can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                primitive_argument_type operator()(
                    ast::expression&&, ast::expression&&) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side are incompatible "
                                "and can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                primitive_argument_type operator()(primitive&&, primitive&&) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side are incompatible "
                                "and can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                template <typename T>
                primitive_argument_type operator()(T && lhs, T && rhs) const
                {
                    return primitive_argument_type(
                            ir::node_data<bool>{lhs >= rhs});
                }

                primitive_argument_type operator()(
                    util::recursive_wrapper<
                        std::vector<primitive_argument_type>>&&,
                    util::recursive_wrapper<
                        std::vector<primitive_argument_type>>&&) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side are incompatible "
                                "and can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                primitive_argument_type operator()(
                    ir::node_data<double>&& lhs, std::int64_t&& rhs) const
                {
                    if (lhs.num_dimensions() != 0)
                    {
                        return greater_equal_.greater_equal_all(
                            std::move(lhs), operand_type(std::move(rhs)));
                    }
                    return primitive_argument_type(
                        ir::node_data<bool>{lhs[0] >= rhs});
                }

                primitive_argument_type operator()(
                    std::int64_t&& lhs, ir::node_data<double>&& rhs) const
                {
                    if (rhs.num_dimensions() != 0)
                    {
                        return greater_equal_.greater_equal_all(
                            operand_type(std::move(lhs)), std::move(rhs));
                    }
                    return primitive_argument_type(
                        ir::node_data<bool>{lhs >= rhs[0]});
                }

                primitive_argument_type operator()(
                    operand_type&& lhs, operand_type&& rhs) const
                {
                    return greater_equal_.greater_equal_all(
                        std::move(lhs), std::move(rhs));
                }

                primitive_argument_type operator()(
                    ir::node_data<bool>&& lhs, ir::node_data<bool>&& rhs) const
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "left hand side and right hand side can't be compared",
                            greater_equal_.name_, greater_equal_.codename_));
                }

                greater_equal const& greater_equal_;
            };

        public:
            hpx::future<primitive_argument_type> eval(
                std::vector<primitive_argument_type> const& operands,
                std::vector<primitive_argument_type> const& args) const
            {
                if (operands.size() != 2)
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "the greater_equal primitive requires exactly two "
                                "operands",
                            name_, codename_));
                }

                if (!valid(operands[0]) || !valid(operands[1]))
                {
                    HPX_THROW_EXCEPTION(hpx::bad_parameter,
                        "greater_equal::eval",
                        generate_error_message(
                            "the greater_equal primitive requires that the "
                                "arguments given by the operands array are valid",
                            name_, codename_));
                }

                auto this_ = this->shared_from_this();
                return hpx::dataflow(hpx::util::unwrapping(
                    [this_](operands_type && ops) -> primitive_argument_type
                    {
                        return primitive_argument_type(
                            util::visit(visit_greater_equal{*this_},
                                std::move(ops[0].variant()),
                                std::move(ops[1].variant())));
                    }),
                    detail::map_operands(
                        operands, functional::literal_operand{}, args,
                        name_, codename_));
            }
        };
    }

    // implement '>=' for all possible combinations of lhs and rhs
    hpx::future<primitive_argument_type> greater_equal::eval(
        std::vector<primitive_argument_type> const& args) const
    {
        if (operands_.empty())
        {
            return std::make_shared<detail::greater_equal>(name_, codename_)
                ->eval(args, noargs);
        }
        return std::make_shared<detail::greater_equal>(name_, codename_)
            ->eval(operands_, args);
    }
}}}
