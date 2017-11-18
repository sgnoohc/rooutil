#include "draw.h"

using namespace RooUtil::StringUtil;
using namespace RooUtil;


RooUtil::DrawExprTool::pairVecTStr RooUtil::DrawExprTool::getDrawExprPairs()
{
    std::vector<TString> draw_cmd;
    std::vector<TString> draw_sel;
    for (json::iterator it_reg = _j.begin(); it_reg != _j.end(); ++it_reg)
    {
        json g(_j[it_reg.key()]);
        TString reg = it_reg.key().c_str();
        if (!g.count("cuts"))
        {
            print("ERROR - Did not find any cuts field for this json");
            std::cout << std::setw(4) << g << std::endl;
        }
        if (!g.count("histograms"))
        {
            warning("This json has no histograms defined. Seems unusual. Is this correct?");
            std::cout << std::setw(4) << g << std::endl;
        }
        std::vector<TString> this_reg_draw_cmd = getFullDrawCmdExprs(g["histograms"]);
        std::vector<TString> this_reg_draw_sel = getFullDrawSelExprs(g["cuts"]);
        for (auto& cmd : this_reg_draw_cmd)
        {
            for (size_t isel = 0; isel < this_reg_draw_sel.size(); ++isel)
            {
                TString cmd_w_name = Form(cmd.Data(), (TString("%s_") + Form("%s_cut%d_", reg.Data(), isel)).Data());
                TString sel = this_reg_draw_sel[isel];
                draw_cmd.push_back(cmd_w_name);
                draw_sel.push_back(sel);
            }
        }
    }
    return std::make_tuple(draw_cmd, draw_sel);
}

RooUtil::DrawExprTool::pairVecTStr RooUtil::DrawExprTool::getPairVecTStr(json& j)
{
    std::vector<TString> cuts;
    std::vector<TString> wgts;
    std::vector<TString> ps_strs = j;
    for (auto& str : ps_strs)
    {
        TString tmpstr = str;
        if (str.Contains("#"))
            tmpstr = getExprFromRegion(_j, str);
        std::vector<TString> str_items = split(tmpstr, "^");
        if (str_items.size() != 1 && str_items.size() != 2)
            error(Form("failed to parse selection string = %s", tmpstr.Data()));
        TString cut = str_items[0];
        TString wgt = str_items.size() > 1 ? str_items[1] : "1";
        cuts.push_back(cut);
        wgts.push_back(wgt);
    }
    return std::make_tuple(cuts, wgts);
}

RooUtil::DrawExprTool::pairTStr RooUtil::DrawExprTool::getPairTStr(json& j)
{
    std::vector<TString> cuts;
    std::vector<TString> wgts;
    std::tie(cuts, wgts) = getPairVecTStr(j);
    return std::make_tuple(formexpr(cuts), formexpr(wgts));
}

RooUtil::DrawExprTool::pairTStr RooUtil::DrawExprTool::getPairTStrFromRegion(json& j, TString region, std::vector<int> exclude)
{
    json& regj = j[region.Data()];
    if (!regj.count("cuts"))
    {
        print("ERROR - Did not find any cuts field for this json");
        std::cout << std::setw(4) << regj << std::endl;
    }
    json& cutj = regj["cuts"];
    std::vector<TString> tmp_all_selections;
    if (cutj.count("preselections"))
    {
        std::vector<TString> v = cutj["preselections"];
        tmp_all_selections.insert(tmp_all_selections.end(), v.begin(), v.end());
    }
    if (cutj.count("selections"))
    {
        std::vector<TString> v = cutj["selections"];
        tmp_all_selections.insert(tmp_all_selections.end(), v.begin(), v.end());
    }

    std::vector<TString> all_selections;
    for (size_t idx = 0; idx < tmp_all_selections.size(); ++idx)
    {
        if (std::find(exclude.begin(), exclude.end(), idx) != exclude.end())
            continue;
        all_selections.push_back(tmp_all_selections.at(idx));
    }

    json tmp(all_selections);
    return getPairTStr(tmp);
}

RooUtil::DrawExprTool::pairTStr RooUtil::DrawExprTool::getPairTStrFromRegionFromExpr(json& j, TString expr)
{
    expr.ReplaceAll("#", "");
    std::vector<TString> expr_tokens = split(expr, "%");
    if (expr_tokens.size() != 1 && expr_tokens.size() != 2)
        error(Form("failed to parse selection string = %s", expr.Data()));
    TString region = expr_tokens[0];
    TString exclude_str = expr_tokens.size() > 1 ? expr_tokens[1] : "";
    std::vector<int> exclude;
    for (auto& elem : split(exclude_str, ","))
        exclude.push_back(elem.Atoi());
    return getPairTStrFromRegion(j, region, exclude);
}

TString RooUtil::DrawExprTool::getExprFromRegion(json& j, TString expr)
{
    pairTStr p = getPairTStrFromRegionFromExpr(j, expr);
    return Form("%s ^ %s", std::get<0>(p).Data(), std::get<1>(p).Data());
}

std::vector<TString> RooUtil::DrawExprTool::getFullDrawSelExprs(json& j)
{
    std::vector<TString> draw_sel;

    // Parse the "selections" data in a given region json.
    std::vector<TString> individ_selections;
    std::vector<TString> individ_selections_weights;
    if (j.count("selections"))
        std::tie(individ_selections, individ_selections_weights) = getPairVecTStr(j["selections"]);

    // Parse the "preselections" data in a given region json.
    TString preselection = "1";
    TString preselection_weight = "1";
    if (j.count("preselections"))
        std::tie(preselection, preselection_weight) = getPairTStr(j["preselections"]);

    // Pre-pend the preselections to the selections
    individ_selections.insert(individ_selections.begin(), preselection);
    individ_selections_weights.insert(individ_selections_weights.begin(), preselection_weight);

    // Now collapse the selections into a list of selections
    std::vector<TString> selections;
    std::vector<TString> selections_weights;
    for (size_t isel = 0; isel < individ_selections.size(); ++isel)
    {
        std::vector<TString> cutexpr(individ_selections.begin(), individ_selections.begin() + isel + 1);
        std::vector<TString> wgtexpr(individ_selections_weights.begin(), individ_selections_weights.begin() + isel + 1);
        TString full_draw_sel_expr = Form("(%s)*(%s)", formexpr(cutexpr).Data(), formexpr(wgtexpr).Data());
        full_draw_sel_expr = cleanparantheses(full_draw_sel_expr);
        draw_sel.push_back(full_draw_sel_expr);
    }

    return draw_sel;
}

std::vector<TString> RooUtil::DrawExprTool::getFullDrawCmdExprs(json& j)
{
    std::vector<TString> draw_cmd;
    for (json::iterator it_hist = j.begin(); it_hist != j.end(); ++it_hist)
    {
        json& histj = it_hist.value();
        if (!histj.count("var"))
        {
            print("ERROR - Did not find 'var' field for this histogram definition");
            std::cout << std::setw(4) << histj << std::endl;
        }
        if (!histj.count("bin"))
        {
            print("ERROR - Did not find 'bin' field for this histogram definition");
            std::cout << std::setw(4) << histj << std::endl;
        }
        TString name = it_hist.key();
        TString var = it_hist.value()["var"];
        TString bin = it_hist.value()["bin"];
        TString cmd = Form("%s>>", var.Data()) + TString("%s") + Form("%s", name.Data()) + Form("%s", bin.Data());
        cmd = sjoin(cmd, " ", "");
        draw_cmd.push_back(cmd);
    }
    return draw_cmd;
}
