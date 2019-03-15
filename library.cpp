/*
####################################################################
# TIE-02207 Programming 2: Basics, S2019                           #
#                                                                  #
# Project 2: Library                                               #
# Program description: Program read a csv-file and after that user #
# print informations conserining libraries, authors and book with  #
# various commands                                                 #
#                                                                  #
# Author: Anttoni Tukia, 267137., anttoni.tukia@tuni.fi            #
####################################################################
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

//Information about each book is stored here
struct Book
{
    std::string title;
    std::string author;
    int reservations;
};

//The datastructure
using Books = std::vector<Book>;
using Authors = std::map<std::string, Books>;
using Libraries = std::map<std::string, Authors>;

const int ON_THE_SHELF = -1;
const int MAX_RESERVATIONS = 100;
/*
Split given sting
s: string to be spitted
delimeter: the delimeter (obviously)
return: a vector that contains string parts
*/
std::vector<std::string> split(const std::string& s, const char delimiter)
{
    std::vector<std::string> result;
    std::string tmp = s;

    //Loop until delimeter is not the greatest possible value
    while(tmp.find(delimiter) != std::string::npos)
    {
        //Substract until delimeter is found
        std::string new_part = tmp.substr(0, tmp.find(delimiter));
        tmp = tmp.substr(tmp.find(delimiter)+1, tmp.size());
        if(not new_part.empty())
        {
            result.push_back(new_part);
        }
    }

    //Add to vector is tmp is not an empty string
    if(not tmp.empty())
    {
        result.push_back(tmp);
    }
    return result;
}

/*
Inserts book to datastruture
libs: datastructure
*/
void insert_book(Libraries& libs, const std::string& library, const std::string& author,
                 const std::string& book, int reservations)
{
    //Insert library if isn't already in datastructure
    if (libs.find(library) == libs.end())
    {
        libs.insert({library, {}});
    }

     //Insert author if isn't already in datastructure
    if (libs.at(library).find(library) == libs.at(library).end())
    {
        libs.at(library).insert({author, {}});
    }

    //Creates a new "book"
    Books& books = libs.at(library).at(author);
    for (Book& old_book : books)
    {
        //If the same book is twice in the file, update reservations
        if (old_book.title == book && old_book.author == author)
        {
            old_book.reservations = reservations;
            return;
        }
    }
    books.push_back({book, author, reservations});
}

/*
Asks user to give the name of the file to open and reads it.
return true if successful, false if not
libs: datastucture
*/
bool readInputFile(Libraries& libs)
{
    std::string file_name;
    std::cout << "Input file: ";
    std::getline(std::cin, file_name);

    std::ifstream file(file_name);

    //Return false if file cannot be opened
    if (!file)
    {
        std::cout << "Error: the input file cannot be opened" << std::endl;
        return false;
    }

    //Getline returns true while there are any unlooped lines left in file
    std::string line;
    while (getline(file, line))
    {
        //Uses split-funtion to split the line from file
        std::vector<std::string> parts = (split(line, ';'));
        if (parts.size() != 4)
        {
            std::cout << "Error: empty field" << std::endl;
            file.close();
            return false;
        }

        //Assingn names to each part of vector
        std::string library = parts.at(0);
        std::string author = parts.at(1);
        std::string book = parts.at(2);
        std::string reservations_str = parts.at(3);

        //Insert -1 to reservations if the book is on the shelf
        int reservations;
        if (reservations_str == "on-the-shelf")
        {
            reservations = ON_THE_SHELF;
        }

        //If the book is not on the shelf insert the lenght of the queue
        else
        {
            reservations = std::stoi(reservations_str);
        }
        insert_book(libs, library, author, book, reservations);
    }

    //Close the file and return true when successful
    file.close();
    return true;
}

/*
Prints the shortes queue to wanted book and prints libraries where the book is found in alphabetical order
libs: datastructure
fields: vector that contain given parameters
*/
void reservables(const Libraries& libs, const std::vector<std::string>& fields)
{
    //If user gives to few parameters
    if (fields.size() < 2)
    {
        std::cout << "Error in command reservable" << std::endl;
        return;
    }

    //Re-arrange the name of the book into a string
    std::string book_name;    
    for (unsigned i = 1; i < fields.size() - 1; i++)
    {
        book_name += fields.at(i);
        book_name += " ";
    }
    book_name += fields.back();

    if (book_name.front() == '\"' && book_name.back() == '\"')
    {
        book_name = book_name.substr(1, book_name.size() - 2);

        //Stop if re-arranged string is empty
        if (book_name.empty())
        {
            std::cout << "Error in command reservable" << std::endl;
            return;
        }
    }

    int shortest = MAX_RESERVATIONS;
    std::set<std::string> shortest_libs;

    //Loop through the entire datastucture
    for (const auto& library : libs)
    {
        for (const auto& authors : library.second)
        {
            for (const auto& book : authors.second)
            {
                //If the correct book is found
                if (book.title == book_name)
                {                   
                    if (book.reservations < shortest)
                    {
                        //Replace the shortes queue and set with library with the shortest queue
                        shortest_libs.clear();
                        shortest = book.reservations;
                        shortest_libs.insert(library.first);
                    }

                    //Insert library to set if the queue is equal to the shortest
                    else if (book.reservations == shortest)
                    {
                        shortest_libs.insert(library.first);
                    }
                }
            }
        }
    }

    //The book is not a library book if it's not found
    if (shortest_libs.empty())
    {
        std::cout << "Book is not a library book." << std::endl;
    }

    //The book is not reservable is there are too mane reservations
    else if (shortest == MAX_RESERVATIONS)
    {
        std::cout << "The book is not reservable from any library." << std::endl;
    }
    else
    {
        //Print this if the book is loanable
        if (shortest == ON_THE_SHELF)
        {
            std::cout << "on the shelf" << std::endl;
        }

        //Print this if there's any queue
        else
        {
            std::cout << shortest << " reservations" << std::endl;
        }

        //Print all the libraires with shorest queues
        for (const std::string& lib : shortest_libs)
        {
            std::cout << "--- " << lib << std::endl;
        }
    }
}

/*
Prints the names of all libraries
*/
void print_libraries(Libraries& libs)
{
    //Using iterator to "point" the names
    Libraries::iterator iter = libs.begin();
    while (iter != libs.end())
    {
        std::cout << iter->first << std::endl;
        iter++;
    }
}

/*
Prints all books from the wanted library
libs: datastructure
fields: vector that contains given parameters
*/
void print_material(const Libraries& libs, const std::vector<std::string>& fields)
{
    //Stop if there are too few or too many parameters
    if (fields.size() > 2 || fields.size() < 2)
    {
        std::cout << "Error in command material" << std::endl;
        return;
    }

    //Stop if given library name is not found
    else if (libs.find(fields.at(1)) == libs.end())
    {
        std::cout << "Error: unknown library name" << std::endl;
        return;
    }

    //Create a temporary vector and insert stuff there, because it's easy to sort
    std::vector<std::string> temp_vector;
    std::string temp_string;

        //Loop through the datastructure
        for (const auto& author : libs.at(fields.at(1)))
        {
            for (const auto& book : author.second)
            {
                //Asseble the string before inserting it to the vector and then clear
                //the string so it can be used next time
                temp_string += author.first;
                temp_string += ": ";
                temp_string += book.title;
                temp_vector.push_back(temp_string);
                temp_string.clear();
            }
        }

    //Sort the vector so it can be printed in an alphabetical order
    std::sort(temp_vector.begin(), temp_vector.end());
    for (unsigned long int i = 0; i < temp_vector.size(); i++)
    {
        std::cout << temp_vector.at(i) << std::endl;
    }
}

/*
Print the books for the wanted author from the wanted library
libs: datastructure
field: vector that contains given parameters
*/
void print_books(const Libraries& libs, const std::vector<std::string>& fields)
{

    //Stop if there are too few or too many parameters
    if (fields.size() < 3 || fields.size() > 3)
    {
        std::cout << "Error: error in command books" << std::endl;
        return;
    }

    //Stop if given library's name is not found
    else if (libs.find(fields.at(1)) == libs.end())
    {
        std::cout << "Error: unknown library name" << std::endl;
        return;
    }

    //Stop if given author's name is not found
    else if (libs.at(fields.at(1)).find(fields.at(2)) == libs.at(fields.at(1)).end())
    {
        std::cout << "Error: unknown author" << std::endl;
        return;
    }

    //Create a temporary vector and insert stuff there, because it's easy to sort
    std::vector<std::string> temp_vector;
    std::string temp_string;

    //Loop through the datastructure
    for (const auto& book : libs.at(fields.at(1)).at(fields.at(2)))
    {

        //Asseble the string
        temp_string += book.title;
        temp_string += " --- ";

        //Print the length of the queue if the book isn't loanable right away
        if (book.reservations != ON_THE_SHELF)
        {
            std::string res_str = std::to_string(book.reservations);
            temp_string += res_str;
            temp_string += " reservations";
        }

        //Print this if the book is loanable
        else
        {
            temp_string += "on the shelf";
        }

        //Add the strong to the temporary vector and clear the string so it can used next time
        temp_vector.push_back(temp_string);
        temp_string.clear();
    }

    //Sort the vector so it can be printed in an alphabetical order
    std::sort(temp_vector.begin(), temp_vector.end());
    for (unsigned long int i = 0; i < temp_vector.size(); i++)
    {
        std::cout << temp_vector.at(i) << std::endl;
    }
}

/*
Prints all the books that are loanable
libs: datastructure
*/
void print_loanable(const Libraries& libs)
{
    //Create a temporary vector and insert stuff there, because it's easy to sort
    std::vector<std::string> temp_vector;
    std::string temp_string;

    //Loop through the datastructure
    for (const auto& library : libs)
    {
        for (const auto& author : library.second)
        {
            for (const auto& book : author.second)
            {
                //Add this to temporary string if the book is loanable
                if (book.reservations == ON_THE_SHELF)
                {
                    temp_string += author.first + ": " + book.title;

                    //Add temporary string to vector if it isn't there already
                    if (std::find(temp_vector.begin(), temp_vector.end(), temp_string) == temp_vector.end())
                    {
                        temp_vector.push_back(temp_string);
                    }

                    //Clear the string for later use
                    temp_string.clear();
                }
            }
        }
    }

    //Sort the vector so it can be printed in an alphabetical order
    std::sort(temp_vector.begin(), temp_vector.end());
    for (unsigned long int i = 0; i < temp_vector.size(); i++)
    {
        std::cout << temp_vector.at(i) << std::endl;
    }
}

int main()
{
    Libraries libs;

    //End program if opening the file was unsuccessful
    if (!readInputFile(libs))
    {
        return EXIT_FAILURE;
    }

    //Loops until the user gives the command to quit
    while (true)
    {
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "quit")
        {
            break;
        }

        //Split the line using this function and save the pieces to a vector
        std::vector<std::string> parts = split(command, ' ');

        //Check the command
        if (parts.at(0) == "reservable")
        {
            reservables(libs, parts);
        }
        else if (parts.at(0) == "libraries")
        {
            print_libraries(libs);
        }
        else if (parts.at(0) == "material")
        {
            print_material(libs, parts);
        }
        else if (parts.at(0) == "books")
        {
            print_books(libs, parts);
        }
        else if (parts.at(0) == "loanable")
        {
            print_loanable(libs);
        }
        else
        {
            std::cout << "Error: Unknown command: " << parts.at(0) << std::endl;
        }

    }
    return EXIT_SUCCESS;
}
