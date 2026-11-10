#!/bin/bash
set -e

echo "Testing GNU Readline integration..."

test -x bin/myshell
echo "PASS: Shell executable exists"

ldd bin/myshell | grep -q libreadline
echo "PASS: GNU Readline linked"

grep -q 'readline(PROMPT)' src/main.c
echo "PASS: readline() integrated"

grep -q 'add_history(input)' src/main.c
echo "PASS: Readline history integrated"

grep -q 'completion_init()' src/main.c
echo "PASS: Completion initialized"

grep -q 'rl_completion_matches' src/completion.c
echo "PASS: Custom command completion implemented"

echo ""
echo "ALL READLINE INTEGRATION CHECKS PASSED"
