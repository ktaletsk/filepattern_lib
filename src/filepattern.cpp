#include <iostream>
#include "functions.h"

int main(int argc, char** argv) {
  if (argc != 3)
  {
    std::cout << "Usage: filepattern <pattern> <path>";
    return 1;
  }
  string reg = argv[1];
  string pat = argv[2];

  tuple<string, string> x;
  try
  {
      x = get_regex(reg);
  }
  catch (const std::exception &e)
  {
      std::cerr << e.what() << '\n';
  }

  cout << "* Parse filepattern into regexp";
  cout << "* Parsed regexp: " << get<0>(x) << endl;
  cout << "* Variables: " << get<1>(x) << endl;

  // Run Arrow table conversion
  std::shared_ptr<arrow::Table> table;
  Status st = parse_directory_to_table(pat, get<0>(x), get<1>(x), &table);
  if (!st.ok()) {
    std::cerr << st << std::endl;
    return 1;
  }

  std::cerr << "* Read parsed Arrow table:" << std::endl;
  arrow::PrettyPrint(*table, {}, &std::cerr);
  return 0;
}
