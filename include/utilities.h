#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include "splashkit.h"

using namespace std;


/**
 * @brief procedure to output a message and wait for the user to press enter to continue.
 * This makes sure the user does not miss the message output
 *
 * @return;
 */
void read_key(string message)
{
    string key;
    write_line(message);
    write("Press Enter to continue.");
    key = read_line();

    return;
}

/**
 * Read a string from the user
 *
 * @param prompt the message to show the user
 * @returns the string entered
 */
string read_string(string prompt)
{
    string user_entry;

    write(prompt);
    user_entry = read_line();

    return user_entry;
};


/**
 * Read an integer from the user
 *
 * @param prompt the message to show the user
 * @returns the integer entered
 */
int read_integer(string prompt){
    string user_entry;

    do{
        write(prompt);
        user_entry = read_line();
        if (!is_integer(user_entry)){
            write_line("Please enter an integer value.");
        }
    }while(!is_integer(user_entry));

    return stoi(user_entry);
}

/**
 * Read an integer from the user within a specified range
 *
 * @param prompt the message to show the user
 * @returns the integer entered
 */
int read_integer(string prompt, int lowerbound, int upperbound)
{
    string user_entry;
    bool flag = false;

    do
    {
        write(prompt);
        user_entry = read_line();
        if (!is_integer(user_entry))
        {
            write_line("Please enter an integer value.");
        }else if (stoi(user_entry) < lowerbound || stoi(user_entry) > upperbound){
            write_line("Please enter a valid integer.");
        }else{
            flag = true;
        }
    } while (!flag);

    return stoi(user_entry);
};


/**
 * @brief will take a prompt as parameter and output it for the user to enter a value of
 * type double. It will then return that value.
 * 
 * @param prompt will be the message to prompt the user to input the value of type
 * double
 * 
 * @return the double value entered
 */
double read_double(string prompt){
    string user_entry;

    do{
        write(prompt);
        user_entry = read_line();

        if (!is_double(user_entry)){
            write_line("Please enter a number.");
        }
    }while(!is_double(user_entry));
    
    return stod(user_entry);
}

/**
 * @brief will take a prompt as parameter and output it for the user to enter a value of
 * type double. It will then return that value.
 * 
 * @param prompt will be the message to prompt the user to input the value of type
 * double
 * 
 * @return the double value entered
 */
double read_double(string prompt, double lowerbound, double upperbound){
    string user_entry;
    bool flag = false;

    do{
        write(prompt);
        user_entry = read_line();

        if (!is_double(user_entry)){
            write_line("Please enter a number.");
        }else if (stod(user_entry) < lowerbound || stod(user_entry) > upperbound){
            write_line("Please enter a valid number.");
        }else{
            flag = true;
        }
    }while(!flag);

    return stod(user_entry);
}

#endif