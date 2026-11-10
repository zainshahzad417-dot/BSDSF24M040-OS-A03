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
        timeout=10
    )

    assert result.returncode == 0, (
        result.stdout + result.stderr
    )

    return result.stdout + result.stderr


with tempfile.TemporaryDirectory() as directory:
    folder = Path(directory)

    # Test 1: Output redirection
    output = run_shell([
        "echo Hello > output.txt",
        "exit"
    ], folder)

    assert (folder / "output.txt").read_text().strip() == "Hello"
    print("PASS: Output redirection >")

    # Test 2: Input redirection
    (folder / "names.txt").write_text(
        "zebra\napple\nmango\n"
    )

    run_shell([
        "sort < names.txt > sorted.txt",
        "exit"
    ], folder)

    assert (folder / "sorted.txt").read_text() == (
        "apple\nmango\nzebra\n"
    )
    print("PASS: Input redirection <")

    # Test 3: Pipe
    output = run_shell([
        "cat /etc/passwd | grep root",
        "exit"
    ], folder)

    assert "root:" in output
    print("PASS: Pipe |")

    # Test 4: Pipe with output redirection
    run_shell([
        "cat /etc/passwd | grep root > root.txt",
        "exit"
    ], folder)

    assert "root:" in (folder / "root.txt").read_text()
    print("PASS: Pipe with output redirection")

    # Test 5: Multiple pipes
    output = run_shell([
        "cat /etc/passwd | grep root | wc -l",
        "exit"
    ], folder)

    assert any(
        line.strip().isdigit()
        for line in output.splitlines()
    )
    print("PASS: Multiple pipes")

    # Test 6: No spaces around operators
    run_shell([
        "echo testing>compact.txt",
        "exit"
    ], folder)

    assert (folder / "compact.txt").read_text().strip() == "testing"
    print("PASS: Operators without spaces")

    # Test 7: Invalid redirection
    output = run_shell([
        "cat < nonexistent_input_123.txt",
        "ls >",
        "exit"
    ], folder)

    assert "nonexistent_input_123.txt" in output
    assert "redirection needs a filename" in output
    print("PASS: Redirection error handling")

    # Test 8: Previous commands
    output = run_shell([
        "whoami",
        "help",
        "jobs",
        "history",
        "exit"
    ], folder)

    assert "Job control not yet implemented" in output
    assert "history" in output
    print("PASS: Previous features preserved")


print("\nALL FEATURE 5 TESTS PASSED")
