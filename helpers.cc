
#include "helpers.h"

using namespace std;

int str_to_integer(const string& convert_me){

    int return_value = 0;

    try{

        return_value = stoi(convert_me);

    } catch(out_of_range& range_error){

        string msg = get_str_conversion_error(convert_me,"integer","The resulting int is too large");
        error_logger.push_error(msg);

        return_value = numeric_limits<int>::min();

    } catch(invalid_argument& bad_arg_error){

        string msg = get_str_conversion_error(convert_me,"integer","It contains invalid characters.");
        error_logger.push_error(msg);

        return_value = numeric_limits<int>::min();
    }

    return return_value;
}

double str_to_double(const string& convert_me){

    double return_value = 0.0;

    try{

        return_value = stod(convert_me);

    } catch(out_of_range& range_error){

        string msg = get_str_conversion_error(convert_me,"double","The resulting double is too large.");
        error_logger.push_error(msg);
        return_value = numeric_limits<double>::min();

    } catch(invalid_argument& bad_arg_error){

        string msg = get_str_conversion_error(convert_me,"double","It contains invalid characters.");
        error_logger.push_error(msg);
        return_value = numeric_limits<double>::min();
    }

    return return_value;
}

string get_str_conversion_error(const string& bad_str,const string& type,
                                   const string& explanation){

    string return_msg = "Failure to convert string:"+bad_str;
    return_msg       += " to type:"+type+". "+explanation;

    return return_msg;
}
