import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SHELL = ROOT / "bin" / "myshell"


def run_shell(commands, cwd):
    result = subprocess.run(
        [str(SHELL)],
        input="\n".join(commands) + "\n",
        text=True,
        capture_output=True,
        cwd=cwd,
        timeout=15
    )

    assert result.returncode == 0, (
        result.stdout + result.stderr
    )

    return result.stdout + result.stderr


with tempfile.TemporaryDirectory() as directory:
    folder = Path(directory)

    # Test 1: Sequential chaining
    output = run_shell([
        "echo First ; echo Second ; echo Third",
        "exit"
    ], folder)

    assert "First\n" in output
    assert "Second\n" in output
    assert "Third\n" in output
    print("PASS: Sequential command chaining")

    # Test 2: Semicolon inside quotes
    output = run_shell([
        'echo "Hello; World"',
        'echo "Hello & World"',
        "exit"
    ], folder)

    assert "Hello; World" in output
    assert "Hello & World" in output
    print("PASS: Quoted separators")

    # Test 3: Background job tracking and reaping
    output = run_shell([
        "sleep 1 &",
        "jobs",
        "sleep 2",
        "jobs",
        "exit"
    ], folder)

    assert "started: sleep 1" in output
    assert "Running sleep 1" in output
    assert "No active background jobs." in output
    print("PASS: Background jobs and zombie reaping")

    # Test 4: Background command followed by another
    output = run_shell([
        "sleep 1 & echo Ready",
        "sleep 2",
        "exit"
    ], folder)

    assert "started: sleep 1" in output
    assert "Ready" in output
    print("PASS: Background and chained commands")

    # Test 5: Existing pipes still work
    output = run_shell([
        "echo hello | grep hello",
        "exit"
    ], folder)

    assert "hello" in output
    print("PASS: Previous pipe functionality")

    # Test 6: Output redirection remains working
    run_shell([
        "echo multitasking > test.txt",
        "exit"
    ], folder)

    assert (folder / "test.txt").read_text().strip() == (
        "multitasking"
    )
    print("PASS: Previous output redirection")

    # Test 7: Previous built-ins and history
    output = run_shell([
        "whoami",
        "history",
        "help",
        "exit"
    ], folder)

    assert "history" in output
    assert "Custom Shell Help" in output
    print("PASS: Previous built-ins and history")


print("\nALL FEATURE 6 TESTS PASSED")
