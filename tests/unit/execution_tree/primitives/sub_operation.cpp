//   Copyright (c) 2017 Bibek Wagle
//
//   Distributed under the Boost Software License, Version 1.0. (See accompanying
//   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/phylanx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <Eigen/Dense>

#include <iostream>
#include <utility>
#include <vector>

void test_sub_operation_0d()
{
    phylanx::execution_tree::primitive lhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(47.0));

    phylanx::execution_tree::primitive rhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(5.0));

    phylanx::execution_tree::primitive sub =
        hpx::new_<phylanx::execution_tree::primitives::sub_operation>(
            hpx::find_here(),
            std::vector<phylanx::execution_tree::primitive_argument_type>{
                std::move(lhs), std::move(rhs)});

    hpx::future<phylanx::execution_tree::primitive_result_type> f =
        sub.eval();
    HPX_TEST_EQ(
        42.0, phylanx::execution_tree::extract_numeric_value(f.get())[0]);
}

void test_sub_operation_0d_lit()
{
    phylanx::ir::node_data<double> lhs(47.0);

    phylanx::execution_tree::primitive rhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(5.0));

    phylanx::execution_tree::primitive sub =
        hpx::new_<phylanx::execution_tree::primitives::sub_operation>(
            hpx::find_here(),
            std::vector<phylanx::execution_tree::primitive_argument_type>{
                std::move(lhs), std::move(rhs)});

    hpx::future<phylanx::execution_tree::primitive_result_type> f =
        sub.eval();
    HPX_TEST_EQ(
        42.0, phylanx::execution_tree::extract_numeric_value(f.get())[0]);
}

void test_sub_operation_1d()
{
    Eigen::VectorXd v1 = Eigen::VectorXd::Random(1007);
    Eigen::VectorXd v2 = Eigen::VectorXd::Random(1007);

    phylanx::execution_tree::primitive lhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(v1));

    phylanx::execution_tree::primitive rhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(v2));

    phylanx::execution_tree::primitive sub =
        hpx::new_<phylanx::execution_tree::primitives::sub_operation>(
            hpx::find_here(),
            std::vector<phylanx::execution_tree::primitive_argument_type>{
                std::move(lhs), std::move(rhs)});

    hpx::future<phylanx::execution_tree::primitive_result_type> f =
        sub.eval();

    Eigen::VectorXd expected = v1 - v2;
    HPX_TEST_EQ(
        phylanx::ir::node_data<double>(std::move(expected)),
        phylanx::execution_tree::extract_numeric_value(f.get()));
}

void test_sub_operation_1d_lit()
{
    Eigen::VectorXd v1 = Eigen::VectorXd::Random(1007);
    Eigen::VectorXd v2 = Eigen::VectorXd::Random(1007);

    phylanx::ir::node_data<double> lhs(v1);

    phylanx::execution_tree::primitive rhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(v2));

    phylanx::execution_tree::primitive sub =
        hpx::new_<phylanx::execution_tree::primitives::sub_operation>(
            hpx::find_here(),
            std::vector<phylanx::execution_tree::primitive_argument_type>{
                std::move(lhs), std::move(rhs)});

    hpx::future<phylanx::execution_tree::primitive_result_type> f =
        sub.eval();

    Eigen::VectorXd expected = v1 - v2;
    HPX_TEST_EQ(
        phylanx::ir::node_data<double>(std::move(expected)),
        phylanx::execution_tree::extract_numeric_value(f.get()));
}

void test_sub_operation_2d()
{
    Eigen::MatrixXd m1 = Eigen::MatrixXd::Random(101, 101);
    Eigen::MatrixXd m2 = Eigen::MatrixXd::Random(101, 101);

    phylanx::execution_tree::primitive lhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(m1));

    phylanx::execution_tree::primitive rhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(m2));

    phylanx::execution_tree::primitive sub =
        hpx::new_<phylanx::execution_tree::primitives::sub_operation>(
            hpx::find_here(),
            std::vector<phylanx::execution_tree::primitive_argument_type>{
                std::move(lhs), std::move(rhs)});

    hpx::future<phylanx::execution_tree::primitive_result_type> f =
        sub.eval();

    Eigen::MatrixXd expected = m1.array() - m2.array();
    HPX_TEST_EQ(
        phylanx::ir::node_data<double>(std::move(expected)),
        phylanx::execution_tree::extract_numeric_value(f.get()));
}

void test_sub_operation_2d_lit()
{
    Eigen::MatrixXd m1 = Eigen::MatrixXd::Random(101, 101);
    Eigen::MatrixXd m2 = Eigen::MatrixXd::Random(101, 101);

    phylanx::ir::node_data<double> lhs(m1);

    phylanx::execution_tree::primitive rhs =
        hpx::new_<phylanx::execution_tree::primitives::variable>(
            hpx::find_here(), phylanx::ir::node_data<double>(m2));

    phylanx::execution_tree::primitive sub =
        hpx::new_<phylanx::execution_tree::primitives::sub_operation>(
            hpx::find_here(),
            std::vector<phylanx::execution_tree::primitive_argument_type>{
                std::move(lhs), std::move(rhs)});

    hpx::future<phylanx::execution_tree::primitive_result_type> f =
        sub.eval();

    Eigen::MatrixXd expected = m1.array() - m2.array();
    HPX_TEST_EQ(
        phylanx::ir::node_data<double>(std::move(expected)),
        phylanx::execution_tree::extract_numeric_value(f.get()));
}

int main(int argc, char* argv[])
{
    test_sub_operation_0d();
    test_sub_operation_0d_lit();

    test_sub_operation_1d();
    test_sub_operation_1d_lit();

    test_sub_operation_2d();
    test_sub_operation_2d_lit();

    return hpx::util::report_errors();
}
