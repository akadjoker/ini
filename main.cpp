#include <string>
#include <vector>
#include <iostream>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <cstring>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct IniSection
{
    string name;
    vector<string> keys;
    vector<string> values;
};


struct ConfigINI
{

    ConfigINI(const std::string &filename)
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cout << "Erro ao abrir o arquivo.\n";
            return;
        }

        file.seekg(0, std::ios::end);
        size = file.tellg();
        file.seekg(0, std::ios::beg);
        code = new char[size + 1];
        file.read(code, size);
        code[size] = '\0';
        file.close();
    }
    ~ConfigINI()
    {
         for (auto& entry : m_sections)
        {
            delete entry.second;
        }
        delete[] code;
    }
    bool IsAtEnd()
    {
        return current >= size;
    }
    char advance()
    {
        current++;
        return code[current - 1];
    }
    bool match(char c)
    {
        if (IsAtEnd())
            return false;
        if (code[current] != c)
        {
            cout<<"Error: Expected character: ("<<c<< ") get:  ("<<code[current]<<")"<<endl;
            return false;
        }
        current++;
        return true;
   }

    char peekNext()
    {
         if (current + 1 >= size) 
            return '\0';
        return code[current + 1];
    }
    char peek()
    {
        if (IsAtEnd())
            return '\0';
        return code[current];
    }
    int Size()
    {
        return size;
    }

    bool skip_spaces()
    {
        while (code[current] == ' ' || code[current] == '\t')
            current++;
        return true;
    }

    bool Parse()
    {
        if (size == 0)
            return false;
        bool state = true;
        while (!IsAtEnd())
        {
            start = current;
            state =scanToken();
        }
        return state;
    }

    bool scanToken()
    {
        char currentChar = advance();
        if (isPanic)
            return false;
        
        
      

        switch (currentChar)
        {
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;

        case '\n':
            line++;
            break;
        case ';': // ignore comments
        {
                string comment;
                while (peek() != '\n' && !IsAtEnd())
                {
                    comment+=advance();
                }   
                cout << "Comment: " << comment << endl;
      
            break;
        }
        case '[':
        {
            lastSection="";
            while (peek() != ']' &&  !IsAtEnd() && peek() != ' ')
            {
               if (peek() == '\n')
               {
                 cout<<"Error: Expected ']' at line: "<<line<<endl;
                 isPanic = true;
                 return false;
               }

                lastSection += advance();
            }
            advance(); // consume ']'
           // cout << "Section: " << section << endl;
            IniSection *tmp_section= new IniSection();
            m_sections[lastSection] = tmp_section;
            break;
        }
        case '=':
        {
            skip_spaces();
            string value;
            while (peek() != '\n' && !IsAtEnd())
            {
                char c = advance();
                if (c != ' ' )
                    value += c;
            }
            if (value.size() == 0)
            {
                cout<<"Error: Expected key value at line: "<<line<<endl;
                isPanic = true;
                return false;
            } 
            cout << "Value: " << value << endl;
            if (!lastSection.empty())
            {
                IniSection *tmp_section = GetSection(lastSection);
                if(tmp_section)
                    tmp_section->values.push_back(value);
            }
            
            break;
        }
        default:
        {
            if (std::isalpha(currentChar))
            {
                string key;
                key += currentChar;
                while ( std::isalpha(peek()) || peek()=='_')
                {
                    char c = advance();
                    if (c !=' ')
                        key += c;
                }
                if (key.size() == 0)
                {
                    cout<<"Error: Expected value for key at line: "<<line<<endl;
                    isPanic = true;
                    return false;
                }
                cout << "Key: " << key << endl;
                 if (!lastSection.empty())
                {
                    IniSection *tmp_section = GetSection(lastSection);
                    if (tmp_section)
                        tmp_section->keys.push_back(key);
                }
               // IniSection *tmp_section = m_sections[lastSection];
               // tmp_section->keys.push_back(key);
                
            } 
            break;
        }
        }
 
    

// cout << currentChar << " : " << next << endl;
        return true;
    }

    IniSection *GetSection(const std::string &section)
    {
         if (m_sections.count(section) > 0 && !isPanic)
        {
            return m_sections[section];
        }
        cout<<"Error: Section not found: "<<section<<endl;
        return nullptr;
    }

  std::unordered_map<std::string, IniSection*> m_sections;
private:
    char *code{nullptr};
    int size{0};
    int current{0};
    int start = {0};
    int line = {1};
    int state = {0};
    std::string lastSection;
    bool isPanic = {false};
  
};

int main()
{
    ConfigINI config("/media/djoker/data/code/projectos/compilers/ini/config.ini");
    config.Parse();

     IniSection *section = config.GetSection("player");
     if (section)
     { 
        if (section->keys.size()!= section->values.size())
        {
            cout<<"Error: Invalid section: "<<section->keys.size()<<":"<< section->values.size()<<endl;
            return 0;
        }
            int count = section->keys.size();
            cout << "Section: :" << count<< endl;
             for (int i=0;i<count;i++)
             {
                 cout << section->keys[i] << " = " << section->values[i] << endl;
             }
            
     }

    cout << "BY!" << endl;
    return 0;
}