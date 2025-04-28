# Retail-Analytics CLI
A tiny, **header-only** C++17/20 command-line app that explores the
[Online Retail](https://archive.ics.uci.edu/dataset/352/online+retail) dataset
(~0.5 M rows, 2010-2011).
It answers two common questions:
1. **Transactions per country**
2. **Total sales amount** (optionally restricted to the **United Kingdom**)
It reads a CSV copy of `Online Retail.xlsx`, shows a live progress bar while
streaming the file, and finishes in **< 1 s** on a typical laptop.

---

## Table of Contents


- [Getting Started](#getting-started)
- [Building](#building)
- [Usage](#usage)
- [Dataset (Git LFS)](#dataset-git-lfs)
- [Testing](#testing)
- [License](#license)


---

## Getting Started

```bash
git clone --recursive https://github.com/<you>/retail-analytics.git
cd retail-analytics
git lfs pull          # fetch the CSV via Git LFS
```

---

## Building
```bash
make # release build
make clean # remove objects and binary
```


---

## Usage
```text
./retail [options]
Options:
 -f, --file <path> Path to CSV file (default: data/online_retail.csv)
 -c, --country <name> Show only that country
 --only-uk Accumulate amounts for UK only
 -h, --help Print usage
```
Examples:
```bash
# All countries (counts + total)
./retail
# Totals limited to UK
./retail --only-uk
# France only
./retail --country France
```
---

## Dataset (Git LFS)
`data/online_retail.csv` (~50 MB) is tracked via **Git LFS**.
After cloning run:
```bash
git lfs pull
```

---

## Testing
```bash
make test
```
Runs the CLI against `tests/sample.csv` and checks expected output.
---

## License
MIT License — see `LICENSE` for details.
