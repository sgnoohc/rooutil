//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "eventlist.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// Event List class
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//_________________________________________________________________________________________________
RooUtil::EventList::EventList( TString filename, TString delim )
{
    load( filename, delim );
}

//_________________________________________________________________________________________________
RooUtil::EventList::~EventList() {}

//_________________________________________________________________________________________________
void RooUtil::EventList::load( TString filename, TString delim )
{
    event_list.clear();
    ifstream ifile;
    ifile.open( filename.Data() );
    std::string line;

    while ( std::getline( ifile, line ) )
    {
//        std::stringstream ss( line );
//        ss >> evt >> run >> lumi;
//        std::cout << evt << ":" << run << ":" << lumi << ":" << std::endl;
        TString rawline = line;
        std::vector<TString> list = RooUtil::StringUtil::split(rawline, delim);
        if (list.size() != 3)
            RooUtil::error(Form("[RooUtil::EventList::load()] Found a line in %s that does not have exactly three items.", filename.Data()));
        std::vector<int> evtid;
        for (auto& item : RooUtil::StringUtil::split(rawline, delim))
            evtid.push_back( item.Atoi() );
        event_list.push_back( evtid );
    }
}

//_________________________________________________________________________________________________
bool RooUtil::EventList::has( int event, int run, int lumi )
{
    std::vector<int> evtid;
    evtid.push_back( event );
    evtid.push_back( run );
    evtid.push_back( lumi );

    if ( std::find( event_list.begin(), event_list.end(), evtid ) != event_list.end() )
        return true;
    else
        return false;
}
