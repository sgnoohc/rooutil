#ifndef draw_h
#define draw_h

#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "TString.h"

#include "json.h"
#include "stringutil.h"
#include "printutil.h"

using json = nlohmann::json;

namespace RooUtil
{
    class DrawExprTool
    {
        public:
            typedef std::tuple<std::vector<TString>, std::vector<TString>> pairVecTStr;
            typedef std::tuple<TString, TString> pairTStr;

            json _j;

            void setJson(json j) { _j = j; }
            pairVecTStr getDrawExprPairs();
            pairVecTStr getPairVecTStr(json& j);
            pairTStr getPairTStr(json& j);
            pairTStr getPairTStrFromRegion(json& j, TString region, std::vector<int> exclude=std::vector<int>());
            pairTStr getPairTStrFromRegionFromExpr(json& j, TString expr);
            TString getExprFromRegion(json& j, TString expr);
            std::vector<TString> getFullDrawSelExprs(json& j);
    };
}

#endif
