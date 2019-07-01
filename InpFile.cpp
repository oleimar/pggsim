#include <InpFile.hpp>
#include <iostream>
#include <fstream>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

// These functions are used by the member function LoadSectionsFromFile

void Trim(std::string& s)
{
    if (s.size() == 0) return;
    std::size_t n = s.find_first_not_of(' ');
    if (n > 0) s.erase(0, n);
    if (s.size() == 0) return;
    // At this point s must contain at least one non-blank character
    n = s.find_last_not_of(' ');
    if (n < s.size() - 1) s.erase(n + 1, std::string::npos);
}


void RemoveComments(std::string& s)
{
    // Everything after // or ; or # is a comment
    std::size_t n = s.find("//");
    if (n < s.size()) s.erase(n, s.size());
    n = s.find(";");
    if (n < s.size()) s.erase(n, s.size());
    n = s.find("#");
    if (n < s.size()) s.erase(n, s.size());
}


// This function is used by the constructor of InpFile
std::string GetFilename2()
{
    char c;
    while (std::cin.rdbuf()->in_avail()) std::cin.get(c);
    std::cout << "Input file: ";
    std::string s;
    getline(std::cin, s);
    return s;
}


//********************** Class InpFile ****************************************

void InpFile::LoadSectionsFromFile()
{
    if (FileName.size() == 0) return;
    std::ifstream inp(FileName.c_str());
    if (!inp.is_open()) return;
    std::string SectionName; // Initially, the section name is empty
    std::string Name;
    std::string Value;
    std::string s;
    getline(inp,s);
    while (!inp.eof()) {
        RemoveComments(s);
        Trim(s);
        if (s.size() > 0) {
            if (s[0] == '[') { // A new section
                std::size_t n = s.find("]");
                if (n < s.size()) SectionName = s.substr(1, n - 1);
            } else {
                // Either a name = value line or a continuation of such a line
                std::size_t n = s.find("=");
                if (n < s.size()) { // name = value line
                    Name = s.substr(0, n - 1);
                    Trim(Name);
                    Value = s.substr(n + 1, s.size());
                    Trim(Value);
                    Sections[SectionName][Name] = Value;
                } else { // continuation of value
                    Sections[SectionName][Name] += " " + s;
                }
            }
        }
        getline(inp,s);
    }
    inp.close();
    LoadOK = true;
}

void InpFile::StoreSectionsInFile() const
{
    if (FileName.size() == 0) return;
    std::ofstream inp(FileName.c_str());
    if (!inp.is_open()) return;
    for (const  auto& Sit : Sections) {
        inp << "[" << Sit.first << "]\n";
        const map_type& Section = Sit.second;
        for (const auto& it : Section) {
            inp << it.first << " = " << it.second << "\n";
        }
    }
    inp.close();
}

InpFile::InpFile(const std::string& filename, bool warn) :
    FileName(filename), Warn(warn), LoadOK(false)
{
    LoadSectionsFromFile();
}

InpFile::InpFile(const char* filename, bool warn) :
    Warn(warn), LoadOK(false)
{
    // take into account that filename my be NULL
    if (filename) FileName = std::string(filename);
    else FileName = GetFilename2();
    LoadSectionsFromFile();
}

InpFile::InpFile(bool warn) : Warn(warn), LoadOK(false)
{
    FileName = GetFilename2();
    LoadSectionsFromFile();
}

void InpFile::SaveAs(const std::string& filename)
{
    FileName = filename;
    StoreSectionsInFile();
}

void InpFile::Save() const
{
    StoreSectionsInFile();
}

void InpFile::Warning(const std::string& Name,
                      const std::string& SectionName) const
{
    if (Warn) {
        std::cerr << "Read failed for identifier " << Name;
        if (SectionName.size() > 0) std::cerr << " in section " << SectionName;
        std::cerr << '\n';
    }
}

std::string InpFile::GetValueAsString(const std::string& Name,
                                      const std::string& SectionName) const
{
    auto Sit = Sections.find(SectionName);
    if (Sit == Sections.end()) { // Failed to find the section name
        Warning(Name, SectionName);
        return std::string();
    } else {
        const map_type& Section = (*Sit).second;
        const auto& it = Section.find(Name);
        if (it == Section.end()) { // Failed to find the name
            Warning(Name, SectionName);
            return std::string();
        } else { // Found the name
            return std::string(it->second);
        }
    }
}


//******************* Functions for reading from InpFile **********************

// This function can be used to read std::strings that may contain blanks
void ReadString(const InpFile& inp, std::string& Value,
                const std::string& Name, const std::string& SectionName)
{
   Value = inp.GetValueAsString(Name, SectionName);
}
