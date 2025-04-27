#include <atomic>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "CLI/CLI.hpp"            // extern/CLI11/include
#include "barkeep/barkeep.h"      // extern/barkeep

/* ---------------------------------------------------------------------------
 *  RFC-4180 compliant CSV reader.
 *  Handles:
 *    • quoted fields          • doubled quotes (“""” → “"”)
 *    • commas/newlines inside quotes
 * ------------------------------------------------------------------------- */
static std::vector<std::string> csv_read_row(std::istream &in)
{
    std::vector<std::string> row;
    std::string cell;
    bool in_quotes = false;

    for (char ch; ; ) {
        int c = in.get();                      // -1 = EOF
        if (c == -1) {                         // EOF
            row.push_back(cell);
            break;
        }
        ch = static_cast<char>(c);

        if (ch == '"') {
            if (in_quotes && in.peek() == '"') {  // doubled quote
                cell.push_back('"');
                in.get();
            } else {
                in_quotes = !in_quotes;           // toggle state
            }
        }
        else if (ch == ',' && !in_quotes) {
            row.push_back(cell);
            cell.clear();
        }
        else if ((ch == '\n' || ch == '\r') && !in_quotes) {
            if (ch == '\r' && in.peek() == '\n') in.get(); // swallow LF
            row.push_back(cell);
            break;                          // end-of-row
        }
        else {
            cell.push_back(ch);
        }
    }
    return row;
}

/* ------------------------- aggregation struct --------------------------- */
struct Stats {
    std::unordered_map<std::string, std::size_t> tx_per_country;
    double total_amount = 0.0;
};

/* ------------------------------- core ----------------------------------- */
Stats process_csv(const std::string& path,
                  const std::string& country_filter,
                  bool only_uk,
                  std::atomic<std::size_t>& processed)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open CSV: " + path);

    csv_read_row(file);                       // skip header row

    Stats st;
    while (file.peek() != EOF) {
        ++processed;
        const auto cols = csv_read_row(file);
        if (cols.size() < 8) continue;        // malformed row

        const std::string& country = cols[7];
        if (!country_filter.empty() && country != country_filter) continue;

        long   qty;
        double price;
        try {
            qty   = std::stol (cols[3]);
            price = std::stod(cols[5]);
        } catch (...) { continue; }

        st.tx_per_country[country]++;
        if (!only_uk || country == "United Kingdom")
            st.total_amount += qty * price;
    }
    return st;
}

/* ------------------------------ main ------------------------------------ */
int main(int argc, char** argv)
{
    CLI::App app{"Retail-Analytics CLI"};

    std::string csv_path = "data/online_retail.csv";
    std::string country;
    bool only_uk = false;

    app.add_option("-f,--file",    csv_path, "Path to CSV data file");
    app.add_option("-c,--country", country,  "Show only that country");
    app.add_flag  ("--only-uk",    only_uk,  "Sum amounts for UK only");
    CLI11_PARSE(app, argc, argv);

    /* progress bar ------------------------------------------------------- */
    namespace bk = barkeep;
    constexpr std::size_t TOTAL = 541'909;        // approx. rows
    std::atomic<std::size_t> processed{0};

    auto bar = bk::ProgressBar(
        &processed,
        bk::ProgressBarConfig{
            .total  = TOTAL,
            .format = "{6}{1}  {7}{2:3.0f}%{11}  {0}/{3} lines",
            .speed  = 0.1
        });

    /* run ---------------------------------------------------------------- */
    Stats s = process_csv(csv_path, country, only_uk, processed);
    bar->done();

    /* output ------------------------------------------------------------- */
    if (country.empty()) {
        std::cout << "\nTransactions per country:\n";
        for (const auto& [c, n] : s.tx_per_country)
            std::cout << std::setw(25) << std::left << c << n << '\n';
    } else {
        std::cout << "\n" << country << ": "
                  << s.tx_per_country[country] << " transactions\n";
    }

    std::cout << "\nTotal amount"
              << (only_uk ? " (UK)" : " (all countries)")
              << ": £" << std::fixed << std::setprecision(2)
              << s.total_amount << '\n';
}

