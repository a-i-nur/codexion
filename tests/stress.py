#!/usr/bin/env python3

import argparse
import random
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


LOG_RE = re.compile(
    r"^(\d+) (\d+) "
    r"(has taken a dongle|is compiling|is debugging|is refactoring|burned out)$"
)


@dataclass(frozen=True)
class Case:
    name: str
    coders: int
    burnout: int
    compile_ms: int
    debug_ms: int
    refactor_ms: int
    required: int
    cooldown: int
    scheduler: str
    expect: str
    timeout: float = 10.0
    require_parallel: bool = False

    def argv(self):
        return [
            str(self.coders),
            str(self.burnout),
            str(self.compile_ms),
            str(self.debug_ms),
            str(self.refactor_ms),
            str(self.required),
            str(self.cooldown),
            self.scheduler,
        ]


@dataclass(frozen=True)
class Event:
    timestamp: int
    coder: int
    message: str


class Failure(Exception):
    pass


def parse_output(case, output):
    events = []
    previous = -1
    for line_number, raw in enumerate(output.splitlines(), 1):
        match = LOG_RE.fullmatch(raw)
        if not match:
            raise Failure(f"line {line_number}: malformed log: {raw!r}")
        timestamp = int(match.group(1))
        coder = int(match.group(2))
        if not 1 <= coder <= case.coders:
            raise Failure(f"line {line_number}: coder id {coder} is out of range")
        if timestamp < previous:
            raise Failure(
                f"line {line_number}: timestamp moved backward "
                f"from {previous} to {timestamp}"
            )
        previous = timestamp
        events.append(Event(timestamp, coder, match.group(3)))
    if not events:
        raise Failure("program produced no logs")
    return events


def validate_state_order(case, events):
    phase = ["ready"] * (case.coders + 1)
    takes = [0] * (case.coders + 1)
    compiles = [0] * (case.coders + 1)
    compile_starts = [[] for _ in range(case.coders + 1)]
    phase_started = [0] * (case.coders + 1)
    ready_at = [0] * (case.coders + 1)
    burned = []
    tolerance = 3
    for index, event in enumerate(events):
        coder = event.coder
        message = event.message
        if burned:
            raise Failure(f"log exists after burnout: {events[index]}")
        if message == "has taken a dongle":
            if phase[coder] != "ready" or takes[coder] >= 2:
                raise Failure(f"coder {coder}: unexpected dongle log")
            if event.timestamp + tolerance < ready_at[coder]:
                raise Failure(
                    f"coder {coder}: requested at {event.timestamp}, "
                    f"before refactoring ends near {ready_at[coder]}"
                )
            takes[coder] += 1
        elif message == "is compiling":
            if phase[coder] != "ready" or takes[coder] != 2:
                raise Failure(f"coder {coder}: compile without two dongle logs")
            takes[coder] = 0
            phase[coder] = "compiling"
            phase_started[coder] = event.timestamp
            compiles[coder] += 1
            compile_starts[coder].append(event.timestamp)
            if compiles[coder] > case.required:
                raise Failure(f"coder {coder}: compiled too many times")
        elif message == "is debugging":
            if phase[coder] != "compiling":
                raise Failure(f"coder {coder}: debugging after {phase[coder]}")
            expected = phase_started[coder] + case.compile_ms
            if event.timestamp + tolerance < expected:
                raise Failure(
                    f"coder {coder}: debug starts at {event.timestamp}, "
                    f"compile ends near {expected}"
                )
            phase[coder] = "debugging"
            phase_started[coder] = event.timestamp
        elif message == "is refactoring":
            if phase[coder] != "debugging":
                raise Failure(f"coder {coder}: refactoring after {phase[coder]}")
            expected = phase_started[coder] + case.debug_ms
            if event.timestamp + tolerance < expected:
                raise Failure(
                    f"coder {coder}: refactor starts at {event.timestamp}, "
                    f"debug ends near {expected}"
                )
            phase[coder] = "ready"
            ready_at[coder] = event.timestamp + case.refactor_ms
        else:
            burned.append(event)
    return compiles, compile_starts, burned


def validate_timing(case, events, compiles, compile_starts, burned):
    tolerance = 3
    last_compile = [0] * (case.coders + 1)
    for event in events:
        if event.message == "is compiling":
            last_compile[event.coder] = event.timestamp
        elif event.message == "burned out":
            expected = last_compile[event.coder] + case.burnout
            delay = event.timestamp - expected
            if delay < 0 or delay > 10:
                raise Failure(
                    f"coder {event.coder}: burnout at {event.timestamp}, "
                    f"deadline {expected}, delay {delay} ms"
                )
    if case.expect == "complete":
        if burned:
            raise Failure(f"unexpected burnout: {burned[0]}")
        for coder in range(1, case.coders + 1):
            if compiles[coder] != case.required:
                raise Failure(
                    f"coder {coder}: {compiles[coder]} compiles, "
                    f"expected {case.required}"
                )
    elif len(burned) != 1:
        raise Failure(f"expected exactly one burnout, got {len(burned)}")
    intervals = [[] for _ in range(case.coders)]
    for coder in range(1, case.coders + 1):
        for start in compile_starts[coder]:
            end = start + case.compile_ms
            left = coder - 1
            right = coder % case.coders
            intervals[left].append((start, end, coder))
            if right != left:
                intervals[right].append((start, end, coder))
    for dongle, uses in enumerate(intervals, 1):
        uses.sort()
        for previous, current in zip(uses, uses[1:]):
            earliest = previous[1] + case.cooldown
            if current[0] + tolerance < earliest:
                raise Failure(
                    f"dongle {dongle}: coder {current[2]} starts at "
                    f"{current[0]}, before cooldown ends near {earliest}"
                )
    if case.require_parallel:
        starts = sorted(
            (start, coder)
            for coder in range(1, case.coders + 1)
            for start in compile_starts[coder]
        )
        parallel = False
        for first, second in zip(starts, starts[1:]):
            if second[0] - first[0] <= tolerance:
                distance = abs(second[1] - first[1])
                if distance not in (1, case.coders - 1):
                    parallel = True
                    break
        if not parallel:
            raise Failure("no parallel compile by non-neighboring coders")


def run_case(binary, case, verbose=False):
    started = time.monotonic()
    try:
        result = subprocess.run(
            [str(binary), *case.argv()],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=case.timeout,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        raise Failure(f"timeout after {case.timeout:.1f}s") from error
    elapsed = time.monotonic() - started
    if result.returncode != 0:
        raise Failure(
            f"exit status {result.returncode}; stderr={result.stderr!r}"
        )
    if result.stderr:
        raise Failure(f"unexpected stderr: {result.stderr!r}")
    events = parse_output(case, result.stdout)
    compiles, starts, burned = validate_state_order(case, events)
    validate_timing(case, events, compiles, starts, burned)
    if verbose:
        print(result.stdout, end="")
    return elapsed, len(events)


def validate_invalid_inputs(binary):
    invalid = [
        [],
        ["4", "800", "200", "200", "200", "3", "10"],
        ["4", "800", "200", "200", "200", "3", "10", "fifo", "extra"],
        ["4", "800", "200", "200", "200", "3", "10", "bad"],
        ["4", "800", "200", "200", "200", "3", "10", "FIFO"],
        ["4", "800", "200", "200", "200", "3", "10", "edf2"],
        ["4", "-800", "200", "200", "200", "3", "10", "fifo"],
        ["-4", "800", "200", "200", "200", "3", "10", "fifo"],
        ["4", "800x", "200", "200", "200", "3", "10", "fifo"],
        ["4", "800", "20.5", "200", "200", "3", "10", "fifo"],
        ["4", "800", "+200", "200", "200", "3", "10", "fifo"],
        ["4", "800", "", "200", "200", "3", "10", "fifo"],
        ["0", "800", "200", "200", "200", "3", "10", "fifo"],
        ["4", "0", "200", "200", "200", "3", "10", "fifo"],
        ["4", "800", "0", "200", "200", "3", "10", "fifo"],
        ["4", "800", "200", "0", "200", "3", "10", "fifo"],
        ["4", "800", "200", "200", "0", "3", "10", "fifo"],
        ["4", "800", "200", "200", "200", "0", "10", "fifo"],
        ["4", "800", "200", "200", "200", "3", "-10", "fifo"],
        ["2147483648", "800", "200", "200", "200", "3", "10", "fifo"],
    ]
    for argv in invalid:
        result = subprocess.run(
            [str(binary), *argv],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=2.0,
            check=False,
        )
        if result.returncode == 0:
            raise Failure(f"invalid input accepted: {argv}")
        if "Error:" not in result.stdout + result.stderr:
            raise Failure(f"invalid input has no error message: {argv}")


def deterministic_cases():
    return [
        Case("single coder burnout", 1, 60, 60, 60, 60, 1, 0, "fifo", "burnout"),
        Case("single coder 200 deadline", 1, 200, 100, 100, 100, 1, 0,
             "fifo", "burnout"),
        Case("single coder cooldown irrelevant", 1, 75, 60, 60, 60, 2, 999,
             "edf", "burnout"),
        Case("compile equals deadline", 2, 60, 60, 60, 60, 2, 0,
             "fifo", "burnout"),
        Case("compile equals 200 deadline", 2, 200, 200, 200, 200, 2, 0,
             "fifo", "burnout"),
        Case("two coders complete", 2, 1000, 60, 60, 60, 4, 0,
             "fifo", "complete"),
        Case("two coders 200 cycle", 2, 800, 200, 200, 200, 2, 0,
             "fifo", "complete"),
        Case("large cooldown", 2, 3000, 60, 60, 60, 3, 200,
             "fifo", "complete"),
        Case("four coder fifo", 4, 800, 200, 200, 200, 3, 10,
             "fifo", "complete", require_parallel=True),
        Case("four coder edf", 4, 800, 200, 200, 200, 3, 10,
             "edf", "complete", require_parallel=True),
        Case("parallel fifo", 4, 1500, 100, 60, 60, 3, 0,
             "fifo", "complete", require_parallel=True),
        Case("parallel edf cooldown", 4, 2000, 80, 60, 60, 3, 75,
             "edf", "complete", require_parallel=True),
        Case("odd ring fifo", 5, 2500, 70, 60, 60, 3, 30,
             "fifo", "complete", require_parallel=True),
        Case("odd ring edf", 7, 3000, 80, 60, 60, 3, 40,
             "edf", "complete", require_parallel=True),
        Case("tight feasible edf", 10, 500, 60, 60, 60, 3, 10,
             "edf", "complete", require_parallel=True),
        Case("tight 10 coder edf", 10, 350, 100, 100, 100, 5, 10,
             "edf", "complete", require_parallel=True),
        Case("200 coder edge", 200, 800, 60, 60, 60, 1, 0,
             "fifo", "complete", timeout=15.0, require_parallel=True),
        Case("200 coder load", 200, 3000, 60, 60, 60, 1, 0,
             "fifo", "complete", timeout=15.0, require_parallel=True),
    ]


def random_cases(seed, count):
    rng = random.Random(seed)
    cases = []
    for index in range(count):
        coders = rng.randint(2, 24)
        compile_ms = rng.randint(60, 130)
        debug_ms = rng.randint(60, 130)
        refactor_ms = rng.randint(60, 130)
        required = rng.randint(1, 4)
        cooldown = rng.randint(0, 120)
        scheduler = rng.choice(("fifo", "edf"))
        burnout = 5000
        cases.append(
            Case(
                f"random {index + 1:02d}",
                coders,
                burnout,
                compile_ms,
                debug_ms,
                refactor_ms,
                required,
                cooldown,
                scheduler,
                "complete",
                timeout=15.0,
                require_parallel=coders >= 4,
            )
        )
    return cases


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", default="./codexion")
    parser.add_argument("--random", type=int, default=30)
    parser.add_argument("--seed", type=int, default=42042)
    parser.add_argument("--repeat", type=int, default=3)
    parser.add_argument("--verbose", action="store_true")
    args = parser.parse_args()
    binary = Path(args.binary).resolve()
    if not binary.is_file():
        print(f"missing binary: {binary}", file=sys.stderr)
        return 2
    try:
        validate_invalid_inputs(binary)
        print("PASS invalid argument rejection")
    except Failure as error:
        print(f"FAIL invalid argument rejection: {error}")
        return 1
    cases = []
    for repetition in range(args.repeat):
        for case in deterministic_cases():
            cases.append((f"repeat {repetition + 1}", case))
    cases.extend(("random", case) for case in random_cases(args.seed, args.random))
    failures = []
    total_started = time.monotonic()
    for number, (group, case) in enumerate(cases, 1):
        label = f"[{number:03d}/{len(cases):03d}] {group}: {case.name}"
        try:
            elapsed, lines = run_case(binary, case, args.verbose)
            print(f"PASS {label} ({elapsed:.3f}s, {lines} logs)")
        except Failure as error:
            failures.append((case, str(error)))
            print(f"FAIL {label}: {error}")
    duration = time.monotonic() - total_started
    print(
        f"\n{len(cases) - len(failures)}/{len(cases)} passed "
        f"in {duration:.2f}s"
    )
    if failures:
        print("\nFailures:")
        for case, error in failures:
            print(f"- {case.name}: {error}")
            print(f"  command: {binary} {' '.join(case.argv())}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
