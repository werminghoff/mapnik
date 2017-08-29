#! /bin/bash

# enabled VALUE
#   - if VALUE is empty or falsy, returns 1 (false)
#   - otherwise returns 0 (true)
# enabled VALUE COMMAND ...
#   - if VALUE is empty or falsy, returns 0 (true)
#   - otherwise runs COMMAND and returns its result
enabled () {
    local value="$1"; shift
    case $value in
        ''|'0'|[Ff]alse|[Nn]o) test $# -ne 0;;
        *) test $# -eq 0 || "$@";;
    esac
}

# on NAME
#   - if NAME == $TRAVIS_OS_NAME, returns 0 (true)
#   - otherwise returns 1 (false)
# on NAME COMMAND ...
#   - if NAME == $TRAVIS_OS_NAME, runs COMMAND and returns its result
#   - otherwise returns 0 (true)
on () {
    local name="$1"; shift
    case $name in
        $TRAVIS_OS_NAME) test $# -eq 0 || "$@";;
        *) test $# -ne 0;;
    esac
}

git_submodule_update () {
    git submodule update "$@" && return
    # failed, search branch and pull request heads for matching commit
    git submodule foreach \
        '
        test "$sha1" = "`git rev-parse HEAD`" ||
        git ls-remote origin "refs/heads/*" "refs/pull/*/head" |
        while read hash ref; do
            if test "$hash" = "$sha1"; then
                git config --add remote.origin.fetch "+$ref:$ref"
                break
            fi
        done
        '
    # try again with added fetch refs
    git submodule update "$@"
}

# commit_message_contains TEXT
#   - returns 0 (true) if TEXT is found in commit message
#   - case-insensitive, plain-text search, not regex
commit_message_contains () {
    git log -1 --pretty='%B' "$TRAVIS_COMMIT" | grep -qiFe "$*"
}

commit_message_parse () {
    if commit_message_contains '[skip tests]'; then
        config_override "CPP_TESTS = False"
    fi
    if commit_message_contains '[skip utils]'; then
        config_override "MAPNIK_INDEX = False"
        config_override "MAPNIK_RENDER = False"
        config_override "PGSQL2SQLITE = False"
        config_override "SHAPEINDEX = False"
        config_override "SVG2PNG = False"
    fi
}

config_override () {
    echo "Appending to config.py:" "$@"
    echo "$@" >> ./config.py
}

configure () {
    if enabled ${COVERAGE}; then
        ./configure "$@" PREFIX=${PREFIX} PGSQL2SQLITE=False SVG2PNG=False SVG_RENDERER=False \
            COVERAGE=True DEBUG=True
    else
        ./configure "$@" PREFIX=${PREFIX}
    fi
    # print final config values, sorted and indented
    sort -sk1,1 ./config.py | sed -e 's/^/	/'
}

coverage () {
    ./codecov -x "llvm-cov gcov" -Z
}

trigger_downstream() {
    body="{
        \"request\": {
          \"message\": \"Triggered build: Mapnik core commit ${TRAVIS_COMMIT}\",
          \"branch\":\"master\"
        }
    }
    "

    curl -s -X POST \
      -H "Content-Type: application/json" \
      -H "Accept: application/json" \
      -H "Travis-API-Version: 3" \
      -H "Authorization: token ${TRAVIS_TRIGGER_TOKEN}" \
      -d "$body" \
      https://api.travis-ci.org/repo/mapnik%2Fpython-mapnik/requests
}
