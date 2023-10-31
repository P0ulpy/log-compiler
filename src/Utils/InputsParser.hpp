#pragma once

#include <vector>
#include <string>
#include <algorithm>

class InputParser
{
    public:
        
        InputParser (int argc, const char **argv)
        {
            m_rootPath = argv[0];

            for (int i = 1; i < argc; ++i)
            {
                m_tokens.push_back(std::string(argv[i]));
            }
        }

        const std::string& operator[] (const std::string& option) const
        {
            return GetCmdOption(option);
        }

        const std::string& operator[] (size_t index) const
        {
            return GetCmdIndex(index);
        }

        template <typename First = std::string, typename ...Args>
        requires std::is_convertible_v<First, std::string>
        const std::string& GetCmdOptions(const First& option, Args... args) const
        {
            const auto& optionValue = GetCmdOption(option);
            if(!optionValue.empty())
                return optionValue;

            if constexpr (sizeof...(Args) > 0)
                return GetCmdOptions(args...);
            else
            {
                static const std::string empty_string;
                return empty_string;    
            }
        }

        const std::string& GetCmdOption(const std::string& option) const
        {
            auto itr = std::find(m_tokens.begin(), m_tokens.end(), option);

            if (itr != m_tokens.end() && ++itr != m_tokens.end())
            {
                return *itr;
            }

            static const std::string empty_string;
            return empty_string;
        }

        const std::string& GetCmdIndex(size_t index) const
        {
            if(CmdIndexExists(index))
            {
                return m_tokens[index];
            }

            static const std::string empty_string;
            return empty_string;
        }
        
        bool CmdOptionExists(const std::string &option) const
        {
            return std::find(this->m_tokens.begin(), this->m_tokens.end(), option)
                   != this->m_tokens.end();
        }

        bool CmdIndexExists(size_t index) const
        {
            return m_tokens.size() > 0 && m_tokens.size() > index;
        }

        const std::vector<std::string> GetTokens() const    { return m_tokens; }
        const std::string GetRootPath() const               { return m_rootPath; } 

    private:
        std::string m_rootPath;
        std::vector <std::string> m_tokens;
};