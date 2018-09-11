#  Copyright (c) 2018 Hartmut Kaiser
#
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

from phylanx import Phylanx


@Phylanx
def constants_of_nature(a):
    print('e: ', con('e'))
    print('pi: ', con('pi'))
    print('ua: ', con('ua'))

    print('e: ', e())
    print('pi: ', pi())
    print('ua: ', ua())

    print(map(con, ['e', 'pi', 'ua']))


constants_of_nature()
