# Copyright (c) 2017-2018 Hartmut Kaiser
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_phylanx_primitive_plugin(constants_of_nature
  SOURCE_ROOT "${PROJECT_SOURCE_DIR}/src/plugins/constants_of_nature"
  HEADER_ROOT "${PROJECT_SOURCE_DIR}/phylanx/plugins/constants_of_nature"
  AUTOGLOB
  PLUGIN
  FOLDER "Core/Plugins"
  COMPONENT_DEPENDENCIES phylanx)

add_phylanx_pseudo_target(
  primitives.constants_of_nature_dir.constants_of_nature_plugin)
add_phylanx_pseudo_dependencies(
  primitives.constants_of_nature_dir
  primitives.constants_of_nature_dir.constants_of_nature_plugin)
add_phylanx_pseudo_dependencies(
  primitives.constants_of_nature_dir.constants_of_nature_plugin
  constants_of_nature_primitive)
