#ifndef _COMMANDPARSER_H
#define _COMMANDPARSER_H

#include "classes.h"
#include "protocol.h"
#include "clientclasses.h"
#include "Transactions.h"

/////////////////////////////////////////////////////////////

class yathCCommandParser: public yathCGeneric
{
protected:	
	yathCTransaction *Command_rls( const char *CommandTrailer);
	yathCTransaction *Command_lls( const char *CommandTrailer);
	yathCTransaction *Command_rcd( const char *CommandTrailer);
	yathCTransaction *Command_lcd( const char *CommandTrailer);
	yathCTransaction *Command_rrm( const char *CommandTrailer);
	yathCTransaction *Command_lrm( const char *CommandTrailer);
	yathCTransaction *Command_rrmdir( const char *CommandTrailer);
	yathCTransaction *Command_lrmdir( const char *CommandTrailer);
	yathCTransaction *Command_rmv( const char *CommandTrailer);
	yathCTransaction *Command_lmv( const char *CommandTrailer);
	yathCTransaction *Command_rmkdir( const char *CommandTrailer);
	yathCTransaction *Command_lmkdir( const char *CommandTrailer);
	
	yathCTransaction *Command_rtype( const char *CommandTrailer);
	
	yathCTransaction *Command_get( const char *CommandTrailer);
	yathCTransaction *Command_put( const char *CommandTrailer);
	yathCTransaction *Command_mget( const char *CommandTrailer);
	yathCTransaction *Command_mput( const char *CommandTrailer);
	
public:
	yathCCommandParser();
	virtual ~yathCCommandParser();
	yathCTransaction *Parse( const char *Command);
};

/////////////////////////////////////////////////////////////

#endif