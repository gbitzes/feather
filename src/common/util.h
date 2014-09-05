#ifndef ___FEATHER_COMMON_UTIL_H
#define ___FEATHER_COMMON_UTIL_H

/*
 * General utilities
 */

#include <feather/types.h>
#include <feather/int-domain.h>
#include <sstream>

namespace feather {
    struct SimpleRange {
        Int a, b;
    };
    static IntRanges* deserializeDomain(std::string s) {
        s = s.substr(1, s.length()-2);
        std::vector<SimpleRange> ranges;
        
        std::stringstream ss(s);
        std::string chunk;
        while(ss >> chunk) {
            SimpleRange tmp;
            size_t pos = chunk.find("..");
            if(pos == std::string::npos) {
                Int num;
                std::istringstream(chunk) >> num;
                tmp.a = num;
                tmp.b = num;
            }
            else {
                std::string first = chunk.substr(0, pos);
                std::string last = chunk.substr(pos+2);
                Int num;
                std::istringstream(first) >> num;
                tmp.a = num;
                std::istringstream(last) >> num;
                tmp.b = num;
            }
            ranges.push_back(tmp);
        }

        IntRanges *domain = new IntRanges(ranges[0].a, ranges[ranges.size()-1].b);
        for(int i = 0; i < ranges.size()-1; i++)
            domain->removeRange(ranges[i].b+1, ranges[i+1].a-1, false);
        return domain;
    }
} //namespace feather


#endif
