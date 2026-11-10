import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SHELL = ROOT / "bin" / "myshell"

def run_shell(commands):
    result = subprocess.run(
        [str(SHELL)],
        input="\n".join(commands) + "\n",
        text=True,
        capture_output=True,
        cwd=ROOT,
        timeout=15
    )

    assert result.returncode == 0, (
        result.stdout + result.stderr
    )

    return result.stdout + result.stderr

def has_output(output, expected):
    return any(
        line.strip() == expected
        for line in output.splitlines()
    )

# Test 1: Assignment and set
output = run_shell([
    'MSG="hello"',
    "set",
    "exit"
])

assert has_output(output, "MSG=hello")
print("PASS: Variable assignment and set")

# Test 2: Expansion
output = run_shell([
    'MSG="hello"',
    "echo $MSG",
    "exit"
])

assert has_output(output, "hello")
print("PASS: Variable expansion")

# Test 3: Reassignment
output = run_shell([
    'MSG="hello"',
    'MSG="goodbye"',
    "echo $MSG",
    "set",
    "exit"
])

assert has_output(output, "goodbye")
assert has_output(output, "MSG=goodbye")
assert not has_output(output, "MSG=hello")
print("PASS: Variable reassignment")

# Test 4: Quoted values
output = run_shell([
    'MESSAGE="Hello, World!"',
    "echo $MESSAGE",
    "exit"
])

assert has_output(output, "Hello, World!")
print("PASS: Quoted values with spaces")

# Test 5: Multiple variables
output = run_shell([
    "AGE=22",
    "CITY=Lahore",
    "set",
    "exit"
])

assert has_output(output, "AGE=22")
assert has_output(output, "CITY=Lahore")
print("PASS: Multiple variables")

# Test 6: Command chaining
output = run_shell([
    "CITY=Lahore ; echo $CITY",
    "exit"
])

assert has_output(output, "Lahore")
print("PASS: Variables with command chaining")

# Test 7: Variable condition
output = run_shell([
    "FLAG=yes",
    'if [ "$FLAG" = "yes" ]',
    "then",
    "echo CONDITION_PASSED",
    "else",
    "echo CONDITION_FAILED",
    "fi",
    "exit"
])

assert has_output(output, "CONDITION_PASSED")
assert not has_output(output, "CONDITION_FAILED")
print("PASS: Variables inside if conditions")

# Test 8: Variables with pipes
output = run_shell([
    "MSG=hello",
    "echo $MSG | tr a-z A-Z",
    "exit"
])

assert has_output(output, "HELLO")
print("PASS: Variables with pipes")

# Test 9: Invalid variable name
output = run_shell([
    "1MSG=hello",
    "exit"
])

assert "variable: invalid name" in output
print("PASS: Invalid variable name")

# Test 10: Single-quote preservation
output = run_shell([
    "MSG=hello",
    "echo '$MSG'",
    "exit"
])

assert has_output(output, "$MSG")
print("PASS: Single-quoted literal")

print("\nALL FEATURE 8 TESTS PASSED")
