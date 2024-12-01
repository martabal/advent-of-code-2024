run-perf:
	poetry --directory perf run python3 perf/run-perf.py

check-perf:
	poetry --directory perf run ruff check perf

format-perf:
	poetry --directory perf run ruff format perf