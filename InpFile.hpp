/*
 * InpFile.hpp
 *
 * This header file provides the class InpFile, which can be used to handle
 * input from a text file in a manner that is somewhat similar to the way .ini
 * files are handled in Microsoft Windows. The default file extension for these
 * input files is .inp. The format of the file is basically a number of lines
 * of the type Name = Value, where Value is an appropriate representation of a
 * value to be assigned to a variable using lookup based on Name. It is also
 * possible to have several values following Name = and these can continue on
 * additional lines. In this way, arrays or lists of data can be read in. The
 * file can be divided into sections, using lines of the kind [SectionName],
 * and the section name is then also used to look up a value. It is not
 * necessary to use section names, since the first part of the file, before any
 * lines of the kind [SectionName] occur, is regarded as a section with the
 * empty string as section name.
 *
 * For efficiency reasons, the input file is read into memory by the InpFile
 * constructor. Reads from the file cannot fail, in the sense that default
 * values will be assigned to variables. However, it is possible to pass a flag
 * to the constructor so that a warning is displayed on std::cerr whenever a
 * lookup fails, e.g. because of failure to open the file or because the name
 * or section name did not exist (or perhaps was misspelled).
 *
 * Here is an example of the contents of an InpFile file:
 *
 * ; Either a semicolon, two slashes, or a # character can be used for comments
 * NumDays = 7        // This could be read using Read(inp, Num, "NumDays"),
 *                    // since the first part is the default section
 * Weight = 7.54
 * PopSizes = 103 203  59  87 142   // List of values separated by white space,
 *            178  48  95 223  99   // which may continue over several lines
 * OutputName = output.txt
 * [Pars]
 * Max = 300          // This could be read using Read(inp, max, "Max", "Pars")
 *
 */

#ifndef INPFILE_HPP
#define INPFILE_HPP

#include <string>
#include <map>
#include <sstream>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

//************************ Class InpFile *********************************

class InpFile {
public:
    using map_type = std::map<std::string, std::string>;
    using sec_map_type = std::map<std::string, map_type>;
    // using map_iter = map_type::const_iterator;
    // using sec_map_iter = sec_map_type::const_iterator;
    // The file name can either be passed to the constructor, or the user will
    // be prompted for it (on std::cout and std::cin). If the warn parameter
    // is passed as true, a warning will be printed on std::cerr every time
    // a lookup, i.e. read, fails.
    explicit InpFile(const std::string& filename, bool warn = true);
    explicit InpFile(const char* filename, bool warn = true);
    explicit InpFile(bool warn = true);
    const char* GetFileName() const { return FileName.c_str(); }
    void SaveAs(const std::string& filename);
    void Save() const;
    // This operator can be used to check if the input file was successfully
    // opened and read in by the constructor: if ( !inp ) ...
    bool operator!() const { return !LoadOK; }
    void Warning(const std::string& Name,
                 const std::string& SectionName) const;
    std::string GetValueAsString(const std::string& Name,
                                 const std::string& SectionName =
                                 std::string()) const;
private:
    void LoadSectionsFromFile();
    void StoreSectionsInFile() const;
    std::string FileName;
    sec_map_type Sections;
    bool Warn;
    bool LoadOK;
};


//***************** Functions for reading from InpFile *******************

// This template function can be used for any type of single value
template<typename T>
void Read(const InpFile& inp, T& Value, const std::string& Name,
          const std::string& SectionName = std::string())
{
    std::string s = inp.GetValueAsString(Name, SectionName);
    std::istringstream ist(s);
    ist >> Value;
    if (!ist) { // Representation of value was wrong in some way
        Value = T();
        inp.Warning(Name, SectionName);
    }
}

// This template function can be used for reading into a built-in array,
// defined by pointers Beg and End to the first and one-beyond-last elements
template<typename T>
void Read(const InpFile& inp, T* Beg, T* End, const std::string& Name,
          const std::string& SectionName = std::string())
{
    std::string s = inp.GetValueAsString(Name, SectionName);
    std::istringstream ist(s);
    bool OK = true;
    for (T* it = Beg; it!=End; ++it) {
        ist >> *it;
        if (!ist) {
            *it = T();
            OK = false;
        }
    }
    if (!OK) inp.Warning(Name, SectionName);
}

// The template function below is for the case of reading into
// standard library arrays and similar containers; the requirement
// on the array type V is that it should define V::value_type, have
// the method size() and operator[]
template<typename V>
void ReadArr(const InpFile& inp, V& Vec, const std::string& Name,
             const std::string& SectionName = std::string())
{
    using value_type = typename V::value_type;
    std::string s = inp.GetValueAsString(Name, SectionName);
    std::istringstream ist(s);
    bool OK = true;
    for (std::size_t i = 0; i < Vec.size(); ++i) {
        ist >> Vec[i];
        if (!ist) {
            Vec[i] = value_type();
            OK = false;
        }
    }
    if (!OK) inp.Warning(Name, SectionName);
}

// This function can be used to read strings that may contain blanks
void ReadString(const InpFile& inp, std::string& Value,
                const std::string& Name,
                const std::string& SectionName = std::string());

#endif // INPFILE_HPP
