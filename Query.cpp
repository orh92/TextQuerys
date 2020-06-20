#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <regex>
using namespace std;


std::regex word("\\s*[a-zA-Z]\\s");
std::regex word1ORword2("\\s*[a-zA-Z]\\s[O][R]\\s*[a-zA-Z]\\s");
std::regex word1ANDword2("\\s*[a-zA-Z]\\s[A][N][D]\\s*[a-zA-Z]\\s");
std::regex word1BESIDEword2("\\s*[a-zA-Z]\\s[B][E][S][I][D][E]\\s*[a-zA-Z]\\s ");

////////////////////////////////////////////////////////////////////////////////
void mySplit(const std::string& str, vector<string>& cont)
{
    std::istringstream iss(str);
    std::copy(std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>(),
        std::back_inserter(cont));
}
std::shared_ptr<QueryBase> QueryBase::factory(const string& l)
{

    if (regex_match(l, word))
    {
        return std::shared_ptr<QueryBase>(new WordQuery(l));
    }
    else if (regex_match(l, word1ORword2))
    {
        vector<string> vec;
        mySplit(l, vec);
        return std::shared_ptr<QueryBase>(new OrQuery(vec.at(0), vec.at(1)));
    }
    else if (regex_match(l, word1ANDword2))
    {
        vector<string> vec;
        mySplit(l, vec);
        return std::shared_ptr<QueryBase>(new AndQuery(vec.at(0), vec.at(1)));
    }
    else if (regex_match(l, word1BESIDEword2))
    {
        vector<string> vec;
        mySplit(l, vec);
        return std::shared_ptr<QueryBase>(new BesideQuery(vec.at(0), vec.at(1)));
    }
    else
    {
        throw std::invalid_argument("Unrecognized search");
    }

}
////////////////////////////////////////////////////////////////////////////////
QueryResult AndQuery::eval (const TextQuery& text) const
{
   QueryResult left_result = text.query(lhs);
   QueryResult right_result = text.query(rhs);
   auto ret_lines = std::make_shared<std::set<line_no>>();
   std::set_intersection(left_result.begin(), left_result.end(),
      right_result.begin(), right_result.end(), 
      std::inserter(*ret_lines, ret_lines->begin()));
   return QueryResult(rep(), ret_lines, left_result.get_file());
}

QueryResult OrQuery::eval(const TextQuery &text) const
{
   QueryResult left_result = text.query(lhs);
   QueryResult right_result = text.query(rhs);
   auto ret_lines = 
      std::make_shared<std::set<line_no>>(left_result.begin(), left_result.end());
   ret_lines->insert(right_result.begin(), right_result.end());
   return QueryResult(rep(), ret_lines, left_result.get_file());
}
/////////////////////////////////////////////////////////
QueryResult BesideQuery::eval (const TextQuery& text) const
{
 QueryResult left_result = text.query(lhs);
   QueryResult right_result = text.query(rhs);
   auto ret_lines = 
      std::make_shared<std::set<line_no>>(left_result.begin(), left_result.end());
   ret_lines->insert(right_result.begin(), right_result.end());
   return QueryResult(rep(), ret_lines, left_result.get_file());
}
/////////////////////////////////////////////////////////
