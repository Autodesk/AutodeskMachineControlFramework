#!/usr/bin/env bash
set -euo pipefail

usage() {
	echo "Usage: $0 [--unittest] [--testdefinition PATH]... [--build-target TARGET]..."
	echo "Example: $0 --unittest --testdefinition Tests/signalloadtest.xml --build-target libmctest_signalloadtest"
}

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build_coverage}"
RUN_UNITTEST=0
TESTDEFS=()
EXTRA_TARGETS=()

while [[ $# -gt 0 ]]; do
	case "$1" in
		--unittest)
			RUN_UNITTEST=1
			shift
			;;
		--testdefinition)
			if [[ $# -lt 2 ]]; then
				usage
				exit 1
			fi
			TESTDEFS+=("$2")
			shift 2
			;;
		--build-target)
			if [[ $# -lt 2 ]]; then
				usage
				exit 1
			fi
			EXTRA_TARGETS+=("$2")
			shift 2
			;;
		--help|-h)
			usage
			exit 0
			;;
		*)
			echo "Unknown argument: $1"
			usage
			exit 1
			;;
	esac
done

if [[ $RUN_UNITTEST -eq 0 && ${#TESTDEFS[@]} -eq 0 ]]; then
	usage
	exit 1
fi

if ! command -v lcov >/dev/null 2>&1; then
	echo "lcov not found. Please install lcov/genhtml before running coverage."
	exit 1
fi

mkdir -p "$BUILD_DIR"
if [[ ! -f "$BUILD_DIR/githash.txt" ]]; then
	GITHASH=""
	if command -v git >/dev/null 2>&1; then
		GITHASH="$(git -C "$ROOT_DIR" rev-parse --short=8 HEAD 2>/dev/null || true)"
	fi
	if [[ -z "$GITHASH" && -f "$ROOT_DIR/build_linux64/githash.txt" ]]; then
		GITHASH="$(cat "$ROOT_DIR/build_linux64/githash.txt" 2>/dev/null || true)"
	fi
	if [[ -z "$GITHASH" && -f "$ROOT_DIR/githash.txt" ]]; then
		GITHASH="$(cat "$ROOT_DIR/githash.txt" 2>/dev/null || true)"
	fi
	if [[ -z "$GITHASH" ]]; then
		echo "githash.txt not found and git hash unavailable. Run build_clean_linux64.sh or ensure githash.txt exists."
		exit 1
	fi
	printf "%s\n" "$GITHASH" > "$BUILD_DIR/githash.txt"
fi

if [[ ! -f "$BUILD_DIR/clientdirhash.txt" ]]; then
	CLIENTHASH=""
	if [[ -f "$ROOT_DIR/build_linux64/clientdirhash.txt" ]]; then
		CLIENTHASH="$(cat "$ROOT_DIR/build_linux64/clientdirhash.txt" 2>/dev/null || true)"
	fi
	if [[ -z "$CLIENTHASH" && -f "$ROOT_DIR/clientdirhash.txt" ]]; then
		CLIENTHASH="$(cat "$ROOT_DIR/clientdirhash.txt" 2>/dev/null || true)"
	fi
	if [[ -z "$CLIENTHASH" ]]; then
		echo "clientdirhash.txt not found. Run build_clean_linux64.sh or ensure clientdirhash.txt exists."
		exit 1
	fi
	printf "%s\n" "$CLIENTHASH" > "$BUILD_DIR/clientdirhash.txt"
fi

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DINCLUDE_TESTS=ON -DENABLE_COVERAGE=ON

TARGETS=()
if [[ $RUN_UNITTEST -eq 1 ]]; then
	TARGETS+=("amc_unittest")
fi
if [[ ${#TESTDEFS[@]} -gt 0 ]]; then
	TARGETS+=("amc_test")
fi
if [[ ${#EXTRA_TARGETS[@]} -gt 0 ]]; then
	TARGETS+=("${EXTRA_TARGETS[@]}")
fi

if [[ ${#TARGETS[@]} -gt 0 ]]; then
	cmake --build "$BUILD_DIR" --target "${TARGETS[@]}"
fi

pushd "$BUILD_DIR" >/dev/null
lcov --directory . --zerocounters

if [[ $RUN_UNITTEST -eq 1 ]]; then
	./Output/amc_unittest
fi

if [[ ${#TESTDEFS[@]} -gt 0 ]]; then
	CMD=(./Output/amc_test)
	for def in "${TESTDEFS[@]}"; do
		CMD+=(--testdefinition "$def")
	done
	"${CMD[@]}"
fi

lcov --capture --directory . --base-directory "$ROOT_DIR" --output-file coverage.info
lcov --ignore-errors unused --remove coverage.info \
	'/usr/*' \
	"$ROOT_DIR/3RD_PARTY/*" \
	"$ROOT_DIR/Framework/*" \
	"$ROOT_DIR/Libraries/*" \
	"$ROOT_DIR/SDK/*" \
	"$ROOT_DIR/submodules/*" \
	--output-file coverage.info
genhtml coverage.info --output-directory coverage_html
echo "Coverage report: $BUILD_DIR/coverage_html/index.html"
popd >/dev/null
