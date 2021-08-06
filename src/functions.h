#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <iostream>
#include <string>
#include <regex>
#include <filesystem>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>

namespace fs = std::filesystem;
using namespace std;
using arrow::Status;
using arrow::Int32Builder;
using arrow::StringBuilder;


bool validate_variables(string s);
tuple<string, string> get_regex(string pattern);
map<char, int> parse_filename(string file_name,
                              string regex_pattern,
                              string variables);
void print_parsed_helper(map<char, int> r);
Status parse_directory_to_table(
    const std::string& directory,
    const std::string& regexp,
    const std::string& variables,
    std::shared_ptr<arrow::Table>* table
);

#endif  /* FUNCTIONS_H_ */