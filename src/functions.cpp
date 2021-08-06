// regex_match example

#include "functions.h"

bool validate_variables(string s) {
  return true;
}

/**
 * Check for know invalid patterns present in the filepattern
 */
bool is_invalid_filepattern(string s) {
  // Check two neighboring groups with variable number of variables
  string regex_string = R"(.*\{[rtczyxp]+\+\}\{[rtczyxp]+\+\}.*)"; 
  return regex_match(s, regex(regex_string));
}

/**
 * Sum numbers in a vector.
 *
 * @param pattern Filename pattern.
 * @return regex used to parse filenames, Variables found in the filename pattern
 * Parse a filename pattern into a regular expression
 * 
 * The filename pattern used here mimics that used by MIST, where variables and
 * positions are encoded into the string. For example, file_c000.ome.tif that
 * indicates channel using the _c, the filename pattern would be
 * ``file_c{ccc}.ome.tif``. The only possible variables that can be passed into
 * the filename pattern are p, x, y, z, c, t, and r. In the case of p, x, and
 * y, both x&y must be specified or p must be specified, but if all three are
 * specified then an error is thrown.
 * 
 * If no filepattern is provided, then a universal expression is returned.    
 */
tuple<string, string> get_regex(string pattern) {
  // Check for known invalid patterns
  if (is_invalid_filepattern(pattern)) {
    throw invalid_argument("Invalid filepattern");
  }

  string regexp(pattern);
  string variables;
  ostringstream output;

  string regex_string = R"(\{([rtczyxp]+)(\+?)\})"; 
  regex var_regex(regex_string);

  auto iterator_begin = sregex_iterator(pattern.begin(), pattern.end(), var_regex);
  auto iterator_end = sregex_iterator();

  for (; iterator_begin != iterator_end; iterator_begin++)
  {
    smatch match = *iterator_begin;
    string match_str = match.str();
    string variable_grp_str = match[1].str();
    int variable_grp_len = variable_grp_str.length();
    string group_suffix = "";
    bool plus_present = match[2].str().length() == 0 ? false : true;
    char variable = variable_grp_str[0];

    // Check if variables in the group are the same
    if (variable_grp_str.find_first_not_of(variable) != string::npos)
    {
      // Variables are not the same
      throw invalid_argument("Variables in the filepattern are not the same");
    }

    // Check that variable was not already used
    if (variables.find(variable) != string::npos)
    {
      // Variable already used
      throw invalid_argument("Variable already used in the filepattern");
    }

    // Check that plus is not added to the group of more than one variable
    if (plus_present && variable_grp_len > 1) 
    {
      // Plus is added to more than one variable
      throw invalid_argument("Plus is added to more than one variable in the filepattern");
    }

    // Add variable to string of variables
    variables += variable;

    if (plus_present){
      group_suffix = "+";
    }
    else {
      if (variable_grp_len > 1) {
        group_suffix = "{" + to_string(variable_grp_len) + "}";
      }
    }

    output << iterator_begin->prefix() << "([0-9]" + group_suffix + ")";
  }
  output << pattern.substr(pattern.size() - iterator_begin->position());

  return {output.str(), variables};
}

/**
 * Get the variable indices from a file name.
 *
 * @param file_name: List of values parsed from a filename using a filepattern.
 * @param regex: A regular expression used to parse the filename.
 * @param variables: String of variables associated with the regex.
 * @return A dictionary if the filename matches the pattern/regex, otherwise empty dictionary.
 * Extract the variable values from a file name. Return as a dictionary.
 * 
 * For example, if a file name and file pattern are:
 *    file_x000_y000_c000.ome.tif
 *    file_x{xxx}_y{yyy}_c{ccc}.ome.tif
 * 
 * This function will return:
 *    {'x': 0,
 *     'y': 0,
 *     'c': 0}
 */
map<char, int> parse_filename(string file_name,
                              string regex_pattern,
                              string variables) {
  // Initialize the output
  map<char, int> r;

  regex var_regex(regex_pattern);
  smatch match;

  regex_search(file_name, match, var_regex);
  cout << "match: " << match.str() << endl;

  // Check that the number of variables is the same as (the number of matches - 1)
  if (match.size() - 1 != variables.size()) {
    throw invalid_argument("Number of variables in the filepattern is not the same as the number of variables in the filename");
  }

  for (auto v = 0; v < variables.size(); v++) {
    r[variables[v]] = stoi(match[v + 1].str());
  }
  return r;
}

void print_parsed_helper(map<char, int> r)
{
    for (auto it = r.cbegin(); it != r.cend(); ++it)
    {
        std::cout << it->first << " " << it->second << "\n";
    }

    cout << endl;
}

Status parse_directory_to_table(
  const std::string& directory,
  const std::string& regexp,
  const std::string& variables,
  std::shared_ptr<arrow::Table>* table
) {
  // Create table schema

  // Declare the (type) describing matched filenames:
  // one utf8-encoded string column "name"
  // and N int32 columns with names corresponding to chars in "variables"
  std::vector<std::shared_ptr<arrow::Field>> fields;

  std::shared_ptr<arrow::Field> field_matched_filename;
  field_matched_filename = arrow::field("Filename", arrow::utf8());
  fields.push_back(field_matched_filename);
  
  for (int i = 0; i < variables.size(); ++i) {
    std::shared_ptr<arrow::Field> field_variable;
    std::string var_name(1, variables[i]);
    field_variable = arrow::field(var_name, arrow::int32());
    fields.push_back(field_variable);
  }
  
  auto schema = arrow::schema(fields);

  // Find number of files in directory
  int num_files = std::distance(fs::directory_iterator(directory), fs::directory_iterator{});
  cout << endl << "Found " << num_files << " files in directory";

  arrow::MemoryPool* pool = arrow::default_memory_pool();

  arrow::StringBuilder filename_builder(pool);
  std::vector<Int32Builder> variable_builders;
  for (int i = 0; i < variables.size(); ++i) {
    variable_builders.push_back(arrow::Int32Builder(pool));
  }

  // Parse filenames in the directory and insert them into the builders
  for (auto &p : fs::directory_iterator(directory))
  {
      std::cout << p.path() << '\n';
      auto r = parse_filename(p.path().filename().string(), regexp, variables);
      print_parsed_helper(r);

      ARROW_RETURN_NOT_OK(filename_builder.Append(p.path().string()));
      for (int i = 0; i < variables.size(); ++i) {
        ARROW_RETURN_NOT_OK(variable_builders[i].Append(r[variables[i]]));
      }
  }

  // Finalise the arrays
  std::vector<std::shared_ptr<arrow::Array>> arrays(variables.size()+1);
  filename_builder.Finish(&arrays[0]);
  for (int i = 0; i < variables.size(); ++i) {
    variable_builders[i].Finish(&arrays[i+1]);
  }

  // Create arrow table
  *table = arrow::Table::Make(schema, arrays);
  return Status::OK();
}