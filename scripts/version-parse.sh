#!/usr/bin/bash

# This file is part of cco, coroutine library for C.
# Copyright (C) 2023, Domenico Teodonio

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA

# This script exports the following variables depending on the status of the repository
#   - VERSION_FULL
#   - VERSION
#   - VERSION_MAJOR
#   - VERSION_MINOR
#   - VERSION_PATCH
#   - VERSION_PRERELEASE
#   - VERSION_PRERELEASE_FULL
#   - VERSION_METADATA
#   - VERSION_METADATA_FULL
#   - VERSION_COMMIT_HASH
#   - VERSION_COMMITS_AHEAD
#   - VERSION_DIRTY

VERSION_COMMIT_HASH=`git rev-parse HEAD`
if [[ "`git describe 2>&1`" =~ .*fatal:.* ]]
then
    VERSION_FULL='v0.0.0'
    VERSION='v0.0.0'
    VERSION_MAJOR='0'
    VERSION_MINOR='0'
    VERSION_PATCH='0'
    VERSION_PRERELEASE=''
    VERSION_PRERELEASE_FULL='c0'
    VERSION_METADATA=''
    VERSION_METADATA_FULL=$VERSION_COMMIT_HASH
    VERSION_COMMITS_AHEAD='0'
    VERSION_DIRTY='0'
else
    __VERSION_TAGS=`git describe --abbrev=0`
    __PARSE_STRING="s_v\{0,1\}\([0-9]*\).*_\1_p"
    VERSION_MAJOR=`sed -n $__PARSE_STRING <<< "$__VERSION_TAGS"`
    __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.\([0-9]*\).*_\1_p"
    VERSION_MINOR=`sed -n $__PARSE_STRING <<< "$__VERSION_TAGS"`
    __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.\([0-9]*\).*_\1_p"
    VERSION_PATCH=`sed -n $__PARSE_STRING <<< "$__VERSION_TAGS"`
    __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-\([-\.a-zA-Z0-9]*\).*_\1_p"
    VERSION_PRERELEASE=`sed -n $__PARSE_STRING <<< "$__VERSION_TAGS"`
    VERSION_PRERELEASE_ESCAPED=${VERSION_PRERELEASE//\./\\.}
    if [[ -z $VERSION_PRERELEASE ]]
    then
        __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH+\([-\.a-zA-Z0-9]*\).*_\1_p"
    else
        __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-${VERSION_PRERELEASE//\./\\.}+\([-\.a-zA-Z0-9]*\).*_\1_p"
    fi
    VERSION_METADATA=`sed -n $__PARSE_STRING <<< "$__VERSION_TAGS"`
    VERSION_METADATA_ESCAPED=${VERSION_METADATA//\./\\.}
    __VERSION_TAGS=`git describe --long`

    if [[ -z $VERSION_METADATA ]]
    then
        if [[ -z $VERSION_PRERELEASE ]]
        then
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-\([0-9]*\).*_\1_p"
        else
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-${VERSION_PRERELEASE//\./\\.}-\([0-9]*\).*_\1_p"
        fi
    else
        if [[ -z $VERSION_PRERELEASE ]]
        then
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH+${VERSION_METADATA//\./\\.}-\([0-9]*\).*_\1_p"
        else
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-${VERSION_PRERELEASE//\./\\.}+${VERSION_METADATA//\./\\.}-\([0-9]*\).*_\1_p"
        fi
    fi
    VERSION_COMMITS_AHEAD=`sed -n $__PARSE_STRING <<< "$__VERSION_TAGS"`
    if [[ -z $VERSION_METADATA ]]
    then
        if [[ -z $VERSION_PRERELEASE ]]
        then
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-$VERSION_COMMITS_AHEAD-g\([a-zA-Z0-9]*\).*_\1_p"
        else
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-${VERSION_PRERELEASE//\./\\.}-$VERSION_COMMITS_AHEAD-g\([a-zA-Z0-9]*\).*_\1_p"
        fi
    else
        if [[ -z $VERSION_PRERELEASE ]]
        then
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH+${VERSION_METADATA//\./\\.}-$VERSION_COMMITS_AHEAD-g\([a-zA-Z0-9]*\).*_\1_p"
        else
            __PARSE_STRING="s_v\{0,1\}$VERSION_MAJOR\.$VERSION_MINOR\.$VERSION_PATCH-${VERSION_PRERELEASE//\./\\.}+${VERSION_METADATA//\./\\.}-$VERSION_COMMITS_AHEAD-g\([a-zA-Z0-9]*\).*_\1_p"
        fi
    fi
    [[ -z $VERSION_PRERELEASE ]] && VERSION_PRERELEASE_FULL=c${VERSION_COMMITS_AHEAD} || VERSION_PRERELEASE_FULL=$VERSION_PRERELEASE-c$VERSION_COMMITS_AHEAD
    [[ -z $VERSION_METADATA ]] && VERSION_METADATA_FULL=$VERSION_COMMIT_HASH || VERSION_METADATA_FULL=$VERSION_METADATA-$VERSION_COMMIT_HASH
    [[ "$(git describe --dirty)" =~ .*dirty.* ]] && VERSION_DIRTY=1 || VERSION_DIRTY=0
    [[ $VERSION_DIRTY == 1 ]] && VERSION_PRERELEASE_FULL=$VERSION_PRERELEASE_FULL-dirty
    VERSION_FULL="$VERSION_MAJOR.$VERSION_MINOR.$VERSION_PATCH"
    VERSION=$VERSION_FULL
    [[ ! -z $VERSION_PRERELEASE_FULL ]] && VERSION_FULL=$VERSION_FULL-$VERSION_PRERELEASE_FULL
    [[ ! -z $VERSION_PRERELEASE ]] && VERSION=$VERSION-$VERSION_PRERELEASE
    [[ ! -z $VERSION_METADATA_FULL ]] && VERSION_FULL=$VERSION_FULL+$VERSION_METADATA_FULL
    [[ ! -z $VERSION_METADATA ]] && VERSION=$VERSION+$VERSION_METADATA

    unset __PARSE_STRING
    unset __VERSION_TAGS
    unset VERSION_METADATA_ESCAPED
    unset VERSION_PRERELEASE_ESCAPED
fi

export VERSION_FULL
export VERSION
export VERSION_MAJOR
export VERSION_MINOR
export VERSION_PATCH
export VERSION_PRERELEASE
export VERSION_PRERELEASE_FULL
export VERSION_METADATA
export VERSION_METADATA_FULL
export VERSION_COMMIT_HASH
export VERSION_COMMITS_AHEAD
export VERSION_DIRTY