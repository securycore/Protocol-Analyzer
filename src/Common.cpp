// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <fstream>  // std::ifstream.
#include <stdexcept>  // std::exception, std::out_of_range.
#include <algorithm>  // std::find_if, std::count.

#include "../include/analyzer/Common.hpp"


namespace analyzer::common
{
    std::string clockToString (const std::chrono::system_clock::time_point& time) noexcept
    {
        time_t currTime = std::chrono::system_clock::to_time_t(time);
        try {
            return std::string(ctime(&currTime)).erase(24, 1).erase(0, 4);
        }
        catch (const std::out_of_range& /*err*/) {
            return std::string();
        }
    }


    namespace text
    {
        std::string & trimLeft (std::string& str) noexcept
        {
            str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int32_t, int32_t>(std::isspace))));
            return str;
        }

        std::string & trimRight (std::string& str) noexcept
        {
            str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int32_t, int32_t>(std::isspace))).base(), str.end());
            return str;
        }

        std::string & trim (std::string& str) noexcept
        {
            return trimLeft(trimRight(str));
        }


        std::vector<std::string> split (const std::string& str, char delimiter) noexcept
        {
            std::vector<std::string> result;
            split(str, delimiter, std::back_inserter(result));
            return result;
        }

        std::vector<std::string_view> splitInPlace (std::string_view str, char delimiter) noexcept
        {
            std::vector<std::string_view> result;
            std::string_view::size_type start = 0;
            auto pos = str.find_first_of(delimiter, start);
            while (pos != std::string_view::npos)
            {
                if (pos != start) {
                    result.push_back(str.substr(start, pos - start));
                }
                start = pos + 1;
                pos = str.find_first_of(delimiter, start);
            }
            if (start < str.size()) {
                result.push_back(str.substr(start, str.size() - start));
            }
            return result;
        }


        unsigned char charToUChar (const char symbol) noexcept
        {
            union {
                char in;
                unsigned char out;
            } u { symbol };
            return u.out;
        }

        void replaceNonPrintableToSymbol (void* data, const std::size_t size, const char symbol) noexcept
        {
            auto current = reinterpret_cast<unsigned char*>(data);
            auto end = reinterpret_cast<unsigned char*>(data) + size;
            while (current != end) {
                if (isPrintable(symbol) == true) {
                    *current = charToUChar(symbol);
                }
                current++;
            }
        }

    }  // namespace text.


    namespace file
    {
        bool checkFileExistence (std::string_view path) noexcept
        {
            std::ifstream file(path.data(), std::ios_base::binary);
            if (file.is_open() == true && file.good() == true)
            {
                file.close();
                return true;
            }
            return false;
        }

        std::size_t getFileSize (std::string_view path) noexcept
        {
            std::ifstream file(path.data(), std::ios_base::binary);
            if (file.is_open() == true && file.good() == true)
            {
                const auto size = file.seekg(0, std::ios_base::end).tellg();
                file.close();
                return static_cast<std::size_t>(size);
            }
            return ErrorState;
        }

        bool readFileToEnd (std::string_view path, std::string& data) noexcept
        {
            try {
                const std::size_t size = getFileSize(path);
                if (size == ErrorState) {
                    return false;
                }
                data.reserve(size);

                std::ifstream file(path.data(), std::ios_base::in);
                if (file.is_open() == true && file.good() == true)
                {
                    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
                    return true;
                }
            }
            catch (const std::exception& /*err*/) { }
            return false;
        }

        std::size_t getFileLines (std::string_view path) noexcept
        {
            try {
                std::ifstream file(path.data(), std::ios_base::in);
                if (file.is_open() == true && file.good() == true)
                {
                    const auto count = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
                    file.close();
                    return static_cast<std::size_t>(count);
                }
            }
            catch (const std::exception& /*err*/) { }
            return ErrorState;
        }

    }  // namespace file.

}  // namespace common.
