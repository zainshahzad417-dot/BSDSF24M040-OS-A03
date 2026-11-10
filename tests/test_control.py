import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SHELL = ROOT / "bin" / "myshell"

def run_shell(commands, cwd=ROOT):
    result = subprocess.run(
        [str(SHELL)],
        input="\n".join(commands) + "\n",
        text=True,
        capture_output=True,
        cwd=cwd,
        timeout=15
    )
    assert result.returncode == 0, result.stderr
    return result.stdout + result.stderr

def executed(output, marker):
    return any(
        line.strip() == marker
        for line in output.splitlines()
    )

# Test 1: Successful condition
output = run_shell([
    "if true", "then",
    "echo BRANCH_SUCCESS",
    "else", "echo BRANCH_FAILURE",
    "fi", "exit"
])
assert executed(output, "BRANCH_SUCCESS")
assert not executed(output, "BRANCH_FAILURE")
print("PASS: Successful if condition")

# Test 2: Failed condition
output = run_shell([
    "if false", "then",
    "echo SHOULD_NOT_RUN",
    "else", "echo ELSE_EXECUTED",
    "fi", "exit"
])
assert executed(output, "ELSE_EXECUTED")
assert not executed(output, "SHOULD_NOT_RUN")
print("PASS: Failed if condition")

# Test 3: Without else
output = run_shell([
    "if true", "then",
    "echo NO_ELSE_SUCCESS",
    "fi", "exit"
])
assert executed(output, "NO_ELSE_SUCCESS")
print("PASS: If without else")

# Test 4: Failed condition without else
output = run_shell([
    "if false", "then",
    "echo SHOULD_NOT_APPEAR",
    "fi", "echo AFTER_IF", "exit"
])
assert not executed(output, "SHOULD_NOT_APPEAR")
assert executed(output, "AFTER_IF")
print("PASS: Failed if without else")

# Test 5: File existence
with tempfile.TemporaryDirectory() as directory:
    folder = Path(directory)
    (folder / "myfile.txt").write_text("Hello\n")

    output = run_shell([
        'if [ -f "myfile.txt" ]',
        "then", "echo FILE_EXISTS",
        "else", "echo FILE_MISSING",
        "fi", "exit"
    ], folder)

    assert executed(output, "FILE_EXISTS")
    assert not executed(output, "FILE_MISSING")
print("PASS: File existence")

# Test 6: Pipe and redirection
output = run_shell([
    "if echo hello | grep hello > /dev/null",
    "then", "echo PIPE_SUCCESS",
    "else", "echo PIPE_FAILURE",
    "fi", "exit"
])
assert executed(output, "PIPE_SUCCESS")
assert not executed(output, "PIPE_FAILURE")
print("PASS: Pipe and redirection")

# Test 7: Multiple commands
output = run_shell([
    "if true", "then",
    "echo FIRST_THEN",
    "echo SECOND_THEN",
    "else", "echo WRONG_BRANCH",
    "fi", "exit"
])
assert executed(output, "FIRST_THEN")
assert executed(output, "SECOND_THEN")
assert not executed(output, "WRONG_BRANCH")
print("PASS: Multiple commands")

# Test 8: Existing chaining
output = run_shell([
    "echo CHAIN_ONE ; echo CHAIN_TWO",
    "exit"
])
assert executed(output, "CHAIN_ONE")
assert executed(output, "CHAIN_TWO")
print("PASS: Previous command chaining")

print("\nALL FEATURE 7 TESTS PASSED")
