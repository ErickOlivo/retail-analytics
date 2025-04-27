# Retail-Analytics CLI ■
A tiny, **header-only** C++17/20 command-line app that explores the
[Online Retail](https://archive.ics.uci.edu/dataset/352/online+retail) dataset
(~0.5 M rows, 2010-2011).
It answers two common questions:
1. **Transactions per country**
2. **Total sales amount** (optionally restricted to the **United Kingdom**)
It reads a CSV copy of `Online Retail.xlsx`, shows a live progress bar while
streaming the file, and finishes in **< 1 s** on a typical laptop.
---
