#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 2021:
    Robot Control Software Ltd.
    Peter Barczikay
    Andrew Tantos

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace rOops
{

inline std::string insertSeparators(const std::string& src_str, char separator_='.')
{
    // Reserve space for separator characters and 'ns' at the end.
    auto length = src_str.length();
    if (0 == length)
        return "";  // This should never happen.
    length += ((length - 1) / 3);
    // Allocate destination string and fill it with the separator character.
    std::string dst_str(length, separator_);
    // Length is unsigned, but the indexes will be decremented over zero.
    int src_idx  = static_cast<int>(src_str.length()) - 1;
    int dst_idx = static_cast<int>(length) - 1;
    int cntr    = 0;  // separator counter.
    // Copy ts to dst_str starting at the end, and leaving a separator character at every 3rd position.
    while (0 <= src_idx)
    {
        if (0 > dst_idx) {
            throw std::runtime_error("This should never happen.");
        }
        // Copy the actual character.
        dst_str[dst_idx] = src_str[src_idx];
        --src_idx;
        --dst_idx;
        ++cntr;
        // Leave a separator character at every 3rd position.
        if (0 == cntr % 3) {
            --dst_idx;
        }
    }  //while
    return dst_str;
}

template <unsigned NrOfColumnsT>
class ExecDataMeas
{
public:
    using HeaderArray = std::array<std::string, NrOfColumnsT>;
    using ExecDataArray = std::array<std::uint64_t , NrOfColumnsT>;
    /**
      @brief Constructor.
      @param [in] aDropFirstSamples - Number of samples dropped at the beginning. The first measurements are generally not relevant.
      @param [in] aReserve - Number of samples to store. The buffer is reserved, and samples are dropped, when it is full. No reallocation of the buffer are allowed.
    */
    explicit ExecDataMeas(unsigned aDropFirstSamples = 2, std::size_t aReserve = 10000)
        : dropSamples_(aDropFirstSamples)
    {
        if (aReserve >= std::numeric_limits<std::uint64_t>::max()/100) {
            throw std::runtime_error("Exec data vector is too long. More than uint64::max / 100.");
        }
        for (auto& data_vector : stat_) {
            data_vector.reserve(aReserve);
        }
    }
    /**
      @brief Add header to the measured times.
      @param [in] aHeaders - array of column headers.
    */
    void addHeader(const HeaderArray& aHeaders)
    {
        columnTitles_ = aHeaders;
    }
    /**
      @brief Adds measured execution times for an iteration.
      @param [in] exec_data -  Array of execution data.
    */
    void addExecData(const ExecDataArray& exec_data)
    {
        // The first samples are not relevant because the parties are waiting for each other to get started.
        if (0 < dropSamples_) {
            --dropSamples_;
            return;
        }
        if (stat_[0].size() < stat_[0].capacity()) {
            for (std::size_t idx=0; idx<exec_data.size(); ++idx) {
                stat_[idx].push_back(exec_data[idx]);
            }
        }
    }

    /**
        @brief
            Prints statistics to a file and to std::cout.
            The file name is built from the title and the type.
        @param [in] aTitle - The title of the test.
        @param [in] aType - Number of messages sent in the test.
    */
    void printStat(const std::string& aTitle, const std::string& aType)
    {
        //static std::mutex mtx;
        //std::lock_guard<std::mutex> lck(mtx);
        if (stat_[0].empty()) {
            return;
        }
        // Save time samples.
        std::stringstream file_name;
        file_name << aTitle << aType << ".csv";
        save(file_name);

        sort();

        // Print header
        const int header_col_width = 13;
        const int time_col_width   = 15;
        std::cout << std::setw(header_col_width) << " | ";
        for (const auto& title : columnTitles_) {
            if (!title.empty()) {
                std::cout << std::setw(time_col_width) << title << " | ";
            }
        }
        std::cout << std::endl;
        std::cout.precision(3);
        std::cout << std::fixed;
        // Local function for printing statistic lines.
        auto printStatLine = [&](const char* line_title, std::uint64_t percentage)
        {
            std::cout << std::setw(header_col_width-4) << line_title << ": | ";
            auto stat_idx = percentage * stat_[0].size() / 100;
            if (stat_idx >= stat_[0].size()) {
                // handling 100 for maximum.
                stat_idx = stat_[0].size()-1;
            }
            for (std::size_t idx = 0; idx < stat_.size(); ++idx) {
                if (!columnTitles_[idx].empty()) {
                    std::cout << std::setw(time_col_width) << insertSeparators(std::to_string(stat_[idx][stat_idx])) << " | ";
                }
            }
            std::cout << std::endl;
        };
        printStatLine("minimum", 0);
        printStatLine("P50", 50);
        printStatLine("P98", 98);
        printStatLine("maximum", 100);
    }

    std::size_t size() const
    {
        return stat_[0].size();
    }
protected:
    void save(const std::stringstream& file_name)
    {
        std::ofstream smpl_file(file_name.str());
        if (smpl_file.good()) {
            for (const auto& title : columnTitles_) {
                if (!title.empty()) {
                    smpl_file << std::setw(15) << title << ",";
                }
            }
            smpl_file << '\n';
            smpl_file << std::fixed;
            smpl_file.precision(3);
            smpl_file << std::fixed;
            for (std::size_t tt=0; tt<stat_[0].size(); ++tt) {
                for (std::size_t idx = 0; idx < stat_.size(); ++idx) {
                    if (!columnTitles_[idx].empty()) {
                        // If loaded to spreadsheet, separators are confusing.
                        //smpl_file << std::setw(15) << insertSeparators(std::to_string(tt[idx])) << ",";
                        smpl_file << std::setw(15) << std::to_string(stat_[idx][tt]) << ",";
                    }
                }
                smpl_file << '\n';
            }  //for
        }
        else {
            std::cerr << "Cannot open " << file_name.str() << std::endl;
        }
    }
    void sort()
    {
        for (auto& vec : stat_) {
            std::sort(vec.begin(), vec.end());
        }
    }
private:
    HeaderArray columnTitles_{ {} }; ///< Titles for every column
    unsigned dropSamples_{ 2 }; ///< The first few samples are skipped, their values are not relevant due to transient effects.
    std::array<std::vector<std::uint64_t>, NrOfColumnsT> stat_{ {} }; /// Array of vectors for collecting data.
};

}  // namespace rOops
