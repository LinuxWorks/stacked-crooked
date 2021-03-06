#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <map>


template<typename T>
std::string convert_to_string(const T & value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}


template<typename T>
std::string convert_to_string(const std::vector<T> & list)
{
    std::string result;
    for (typename std::vector<T>::const_iterator it  = list.begin(),
                                                 end = list.end();
         it != end; ++it)
    {
        if (!result.empty())
            result += ", ";
        
        result += convert_to_string(*it);
    }
    return "[ " + result + " ]";
}


template<typename T, typename U>
std::string convert_to_string(const std::map<T, U> & map)
{
    std::string result;
    for (typename std::map<T, U>::const_iterator it  = map.begin(),
                                                 end = map.end();
         it != end; ++it)
    {
        if (!result.empty())
            result += ", ";
        
        result += convert_to_string(it->first) + ": " + convert_to_string(it->second);
    }
    return "{ " + result + " }";
}


std::vector<int> generate_vector(std::size_t a, std::size_t b)
{
    std::vector<int> result;
    for (std::size_t idx = a; idx <= b; ++idx)
    {
        result.push_back(idx);
    }
    return result;
}


std::vector< std::vector<int> > generate_nested_vector(std::size_t a, std::size_t b,
                                                       std::size_t c, std::size_t d,
                                                       std::size_t e, std::size_t f)
{
    std::vector< std::vector<int> > result;
    result.push_back(generate_vector(a, b));
    result.push_back(generate_vector(c, d));
    result.push_back(generate_vector(e, f));
    return result;
}


int main()
{
    std::cout << "Vector: " << convert_to_string(generate_vector(1, 9)) << std::endl;
    
    std::vector< std::vector<int> > nested_vector;
    nested_vector.push_back(generate_vector(1, 3));
    nested_vector.push_back(generate_vector(4, 6));
    nested_vector.push_back(generate_vector(7, 9));
    std::cout << "Nested vector: " << convert_to_string(nested_vector) << std::endl;
    
    std::vector< std::vector< std::vector<int> > > nested_nested_vector;
    nested_nested_vector.push_back(generate_nested_vector(1, 3, 4, 6, 7, 9));
    nested_nested_vector.push_back(generate_nested_vector(1, 3, 4, 6, 7, 9));
    nested_nested_vector.push_back(generate_nested_vector(1, 3, 4, 6, 7, 9));
    std::cout << "Nested nested vector: " << convert_to_string(nested_nested_vector) << std::endl;
    
    std::map<std::string, int> word_count;
    word_count["one"] = 1;
    word_count["two"] = 2;
    word_count["three"] = 3;
    std::cout << "Map: " << convert_to_string(word_count) << std::endl;
    
    std::map<std::string, std::vector<int> > nested_map;
    nested_map["do"] = generate_vector(3, 4);
    nested_map["re"] = generate_vector(5, 6);
    nested_map["mi"] = generate_vector(7, 8);
    std::cout << "Nested map: " << convert_to_string(nested_map) << std::endl;
    
    return 0;
}
