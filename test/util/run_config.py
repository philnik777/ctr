import pathlib
import subprocess


def run_tests(context):
  path = pathlib.Path(__file__).parent.parent

  tests = list(path.rglob("*.cpp"))

  for test in tests:
    extra_arguments = ""
    if (test.name.endswith(".compile.pass.cpp")):
      extra_arguments += " -fsyntax-only "
    result = subprocess.run(f"{context.cxx} -std=c++20 -I{context.include} {extra_arguments} {test}", shell=True)
    print(f"{test}: ", end="")
    if result.returncode == 0:
      print("PASS")
    else:
      print(f"FAIL\nTest {test} failed with output:\n{result.stdout}")
      exit(1)
