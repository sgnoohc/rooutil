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
RooUtil::EventList::EventList( TString filename )
{
    load( filename );
}

//_________________________________________________________________________________________________
RooUtil::EventList::~EventList() {}

//_________________________________________________________________________________________________
void RooUtil::EventList::load( TString filename )
{
    event_list.clear();
    ifstream ifile;
    ifile.open( filename.Data() );
    std::string line;
    int evt, run, lumi;

    while ( std::getline( ifile, line ) )
    {
        std::stringstream ss( line );
        ss >> evt >> run >> lumi;
        std::cout << evt << ":" << run << ":" << lumi << ":" << std::endl;
        std::vector<int> evtid;
        evtid.push_back( evt );
        evtid.push_back( run );
        evtid.push_back( lumi );
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
