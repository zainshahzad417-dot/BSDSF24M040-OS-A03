import subprocess

def run_shell(commands):
    result = subprocess.run(
        ["./bin/myshell"],
        input="\n".join(commands) + "\n",
        text=True,
        capture_output=True,
        timeout=10
    )
    assert result.returncode == 0, (
        f"Shell failed: {result.stderr}"
    )
    return result.stdout + result.stderr


# Test 1: History and re-execution
output = run_shell([
    "pwd",
    "whoami",
    "history",
    "!2",
    "history",
    "exit"
])

assert "1  pwd" in output
assert "2  whoami" in output
assert "3  history" in output
assert "4  whoami" in output
print("PASS: History storage and !n")


# Test 2: Invalid history commands
output = run_shell([
    "!99",
    "!abc",
    "!0",
    "exit"
])

assert "event 99 not found" in output
assert output.count("invalid event number") >= 2
print("PASS: Invalid history commands")


# Test 3: Last 20 commands
output = run_shell(
    ["pwd"] * 24 +
    ["history", "!1", "!6", "exit"]
)

assert "6  pwd" in output
assert "25  history" in output
assert "event 1 not found" in output
print("PASS: Last 20 commands retained")


# Test 4: Previous built-ins
output = run_shell([
    "cd /tmp",
    "pwd",
    "jobs",
    "help",
    "exit"
])

assert "/tmp" in output
assert "Job control not yet implemented" in output
assert "Custom Shell Help" in output
assert "Exiting custom shell" in output
print("PASS: Feature 2 built-in commands")

print("\nALL FEATURE 3 TESTS PASSED")
