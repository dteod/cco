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

# Function to enable all warnings and treat them as errors for a target, compiler-agnostic
function(target_hard_compilation tgt scope)
    target_compile_options(${tgt} ${scope}
        $<$<C_COMPILER_ID:GNU>: -Wall -Wextra -Wpedantic --pedantic-errors -Werror>
        $<$<C_COMPILER_ID:Clang>: -Wall -Wextra -Wpedantic --pedantic-errors -Werror>
        $<$<C_COMPILER_ID:AppleClang>: -Wall -Wextra -Wpedantic --pedantic-errors -Werror>
        $<$<C_COMPILER_ID:Intel>: -Wall -Wextra -Wpedantic --pedantic-errors -Werror>
        $<$<C_COMPILER_ID:MSVC>: /W4 /WX>
    )
endfunction()