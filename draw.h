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
#include "multidraw.h"
#include "fileutil.h"

using json = nlohmann::json;

namespace RooUtil
{
    class DrawExprTool
    {
        public:
            typedef std::tuple<std::vector<TString>, std::vector<TString>, std::vector<TString>> tripleVecTStr;
            typedef std::tuple<std::vector<TString>, std::vector<TString>> pairVecTStr;
            typedef std::tuple<TString, TString> pairTStr;

            json _j;

            void setJson(json j) { _j = j; }
            tripleVecTStr getDrawExprTriple();
            pairVecTStr getDrawExprPairs();
            pairVecTStr getPairVecTStr(json& j);
            pairTStr getPairTStr(json& j);
            pairTStr getPairTStrFromRegion(json& j, TString region, std::vector<int> exclude=std::vector<int>());
            pairTStr getPairTStrFromRegionFromExpr(json& j, TString expr);
            TString getExprFromRegion(json& j, TString expr);
            std::vector<TString> getFullDrawSelExprs(json& j);
            pairVecTStr getFullDrawSelExprsAndWgts(json& j);
            std::vector<TString> getFullDrawCmdExprs(json& j);
    };

    namespace Draw
    {
        DrawExprTool::tripleVecTStr getDrawExprs(json& j);
        std::map<TString, TH1*> drawHistograms(TChain*,  json&, TString="", bool=false);
    }
}

#endif
