# cco - coroutine library for C
# Copyright (C) 2021-2022 Domenico Teodonio
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Set a default value for a variable if it is not already set
function(default_setting DEFINITION DEFAULT_VALUE)
    set(${DEFINITION}_DEFAULT ${DEFAULT_VALUE})

    if(NOT DEFINED ${DEFINITION})
        message(STATUS "${DEFINITION} set to default value of ${${DEFINITION}_DEFAULT}")
        set(${DEFINITION} ${${DEFINITION}_DEFAULT} PARENT_SCOPE)
    else()
        message(STATUS "${DEFINITION} set to ${${DEFINITION}}")
    endif()
endfunction()

macro(default_arch_setting TYPE DEFINITION DEFAULT_VALUE)
    default_setting(CCO_${TYPE}_${CMAKE_SYSTEM_PROCESSOR}_${DEFINITION} ${DEFAULT_VALUE})
endmacro()