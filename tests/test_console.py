"""End-to-end contracts, using only Python's standard library."""
import pathlib
import subprocess
import sys

app = str(pathlib.Path(sys.argv[1]).resolve())
data = pathlib.Path(sys.argv[2])


def run(text):
    completed = subprocess.run([app], input=text, text=True, capture_output=True, timeout=10)
    if completed.returncode != 0:
        raise AssertionError(f"Console failed: {completed.stderr}")
    return completed.stdout


demo = run((data / 'demo_week1.txt').read_text(encoding='utf-8'))
for token in ['Idle -> Selecting', 'Selecting -> Paying', 'Paying -> Idle',
              'Khe A2 da het hang', 'Khong tim thay khe: UNKNOWN',
              'Da them khe hang', 'Banh quy', 'Dispensing']:
    if token not in demo:
        raise AssertionError(f"Missing expected output: {token}")
print('[PASS] full menu selection cancel creation and polymorphism')

invalid = run((data / 'invalid_input.txt').read_text(encoding='utf-8'))
if invalid.count('[LOI]') != 4 or 'Selecting -> Idle' not in invalid:
    raise AssertionError('Bad menu input did not recover correctly')
print('[PASS] bad text overflow trailing junk and out-of-range menu recover')

for text in ['', '3\n', '5\nC1\n', '2\n3\nA1\n']:
    run(text)
print('[PASS] EOF at menu and inside prompts terminates normally')

invalid_product = run('5\nC1\nCOOKIE\nBanh quy\n-1000\n2\n10\n1\n0\n')
if 'Tien khong duoc am' not in invalid_product or 'Da them khe hang' in invalid_product:
    raise AssertionError('Invalid product was accepted')
print('[PASS] invalid product leaves inventory intact')

blocked = run('2\n3\nA1\n5\n4\n0\n')
if 'Chi them khe hang khi may o Idle' not in blocked or 'Paying -> Idle' not in blocked:
    raise AssertionError('Admin add is not guarded by Idle')
print('[PASS] administration is blocked during selection')
print('5/5 console scenarios passed')
