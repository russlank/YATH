#include "commandparser.h"

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCCommandParser::yathCCommandParser()
{
}

/////////////////////////////////////////////////////////////

yathCCommandParser::~yathCCommandParser()
{
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rls( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName[ MAX_PATH];
	const char *FileNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName[ I] = ParamToken[ I];

					FileName[ ParamTokenLength] = 0;
					FileNamePtr = FileName;
				}
			}
		}
	}

	NewTransaction = new yathCFileManagerTransaction();
	if ( NewTransaction != NULL) NewTransaction->SetType_rls( FileNamePtr, 0);

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_lls( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName[ MAX_PATH];
	const char *FileNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName[ I] = ParamToken[ I];

					FileName[ ParamTokenLength] = 0;
					FileNamePtr = FileName;
				}
			}
		}
	}

	NewTransaction = new yathCFileManagerTransaction();
	if ( NewTransaction != NULL) NewTransaction->SetType_lls( FileNamePtr, 0);

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rcd( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char DirPath[ MAX_PATH];
	const char *DirPathPtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( DirPathPtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) DirPath[ I] = ParamToken[ I];

					DirPath[ ParamTokenLength] = 0;
					DirPathPtr = DirPath;
				}
			}
		}
	}

	NewTransaction = new yathCFileManagerTransaction();
	if ( NewTransaction != NULL) NewTransaction->SetType_rcd( DirPathPtr, 0);

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_lcd( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char DirPath[ MAX_PATH];
	const char *DirPathPtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( DirPathPtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) DirPath[ I] = ParamToken[ I];

					DirPath[ ParamTokenLength] = 0;
					DirPathPtr = DirPath;
				}
			}
		}
	}

	NewTransaction = new yathCFileManagerTransaction();
	if ( NewTransaction != NULL) NewTransaction->SetType_lcd( DirPathPtr, 0);

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rrm( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName[ MAX_PATH];
	const char *FileNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName[ I] = ParamToken[ I];

					FileName[ ParamTokenLength] = 0;
					FileNamePtr = FileName;
				}
			}
		}
	}

	if ( FileNamePtr != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_rrmfile( FileNamePtr, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_lrm( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName[ MAX_PATH];
	const char *FileNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName[ I] = ParamToken[ I];

					FileName[ ParamTokenLength] = 0;
					FileNamePtr = FileName;
				}
			}
		}
	}

	if ( FileNamePtr != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_lrmfile( FileNamePtr, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_lmkdir( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char DirName[ MAX_PATH];
	const char *DirNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( DirNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) DirName[ I] = ParamToken[ I];

					DirName[ ParamTokenLength] = 0;
					DirNamePtr = DirName;
				}
			}
		}
	}

	if ( DirNamePtr != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_lmkdir( DirNamePtr, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rmkdir( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char DirName[ MAX_PATH];
	const char *DirNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( DirNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) DirName[ I] = ParamToken[ I];

					DirName[ ParamTokenLength] = 0;
					DirNamePtr = DirName;
				}
			}
		}
	}

	if ( DirNamePtr != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_rmkdir( DirNamePtr, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rrmdir( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char DirName[ MAX_PATH];
	const char *DirNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( DirNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) DirName[ I] = ParamToken[ I];

					DirName[ ParamTokenLength] = 0;
					DirNamePtr = DirName;
				}
			}
		}
	}

	if ( DirNamePtr != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_rrmdir( DirNamePtr, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_lrmdir( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char DirName[ MAX_PATH];
	const char *DirNamePtr = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( DirNamePtr == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) DirName[ I] = ParamToken[ I];

					DirName[ ParamTokenLength] = 0;
					DirNamePtr = DirName;
				}
			}
		}
	}

	if ( DirNamePtr != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_lrmdir( DirNamePtr, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rmv( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	char FileName2[ MAX_PATH];
	const char *FileNamePtr1 = NULL;
	const char *FileNamePtr2 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				} else if ( FileNamePtr2 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName2[ I] = ParamToken[ I];

					FileName2[ ParamTokenLength] = 0;
					FileNamePtr2 = FileName2;
				}
			}
		}
	}

	if (( FileNamePtr1 != NULL) && ( FileNamePtr2 != NULL)){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_rmove( FileNamePtr1, FileNamePtr2, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_lmv( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	char FileName2[ MAX_PATH];
	const char *FileNamePtr1 = NULL;
	const char *FileNamePtr2 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				} else if ( FileNamePtr2 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName2[ I] = ParamToken[ I];

					FileName2[ ParamTokenLength] = 0;
					FileNamePtr2 = FileName2;
				}
			}
		}
	}

	if (( FileNamePtr1 != NULL) && ( FileNamePtr2 != NULL)){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_lmove( FileNamePtr1, FileNamePtr2, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_rtype( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	const char *FileNamePtr1 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				}
			}
		}
	}

	if ( FileNamePtr1 != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_rtype( FileNamePtr1, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_get( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	char FileName2[ MAX_PATH];
	const char *FileNamePtr1 = NULL;
	const char *FileNamePtr2 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				} else if ( FileNamePtr2 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName2[ I] = ParamToken[ I];

					FileName2[ ParamTokenLength] = 0;
					FileNamePtr2 = FileName2;
				}
			}
		}
	}

	if ( FileNamePtr1 != NULL){
		if ( FileNamePtr2 == NULL) FileNamePtr2 = FileNamePtr1;
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_get( FileNamePtr1, FileNamePtr2, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_put( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	char FileName2[ MAX_PATH];
	const char *FileNamePtr1 = NULL;
	const char *FileNamePtr2 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				} else if ( FileNamePtr2 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName2[ I] = ParamToken[ I];

					FileName2[ ParamTokenLength] = 0;
					FileNamePtr2 = FileName2;
				}
			}
		}
	}

	if ( FileNamePtr1 != NULL){
		if ( FileNamePtr2 == NULL) FileNamePtr2 = FileNamePtr1;
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_put( FileNamePtr1, FileNamePtr2, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_mget( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	const char *FileNamePtr1 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				}
			}
		}
	}

	if ( FileNamePtr1 != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_mget( FileNamePtr1, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Command_mput( const char *CommandTrailer)
{
	yathCFileManagerTransaction *NewTransaction = NULL;

	char FileName1[ MAX_PATH];
	const char *FileNamePtr1 = NULL;

	while ( CommandTrailer != NULL){
		BOOL IsOption;
		const char *ParamToken;
		int ParamTokenLength;

		CommandTrailer = yathFindToken( CommandTrailer, ParamToken, ParamTokenLength, IsOption);

		if ( ParamToken != NULL){
			if ( !IsOption){
				if ( FileNamePtr1 == NULL){
					int I;

					if ( ParamTokenLength >= MAX_PATH) ParamTokenLength = MAX_PATH - 1;

					for ( I = 0; I < ParamTokenLength; I ++) FileName1[ I] = ParamToken[ I];

					FileName1[ ParamTokenLength] = 0;
					FileNamePtr1 = FileName1;
				}
			}
		}
	}

	if ( FileNamePtr1 != NULL){
		NewTransaction = new yathCFileManagerTransaction();
		if ( NewTransaction != NULL) NewTransaction->SetType_mput( FileNamePtr1, 0);
	}

	return NewTransaction;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCCommandParser::Parse( const char *Command)
{
	yathCTransaction *NewTransaction = NULL;

	if ( Command != NULL){
		const char *CommandToken;
		int CommandTokenLength;
		BOOL IsOption;
		const char *CommandTrailer = yathFindToken( Command, CommandToken, CommandTokenLength, IsOption);

		if (( !IsOption) && ( CommandToken != NULL) && ( CommandTokenLength > 0)){
			if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rls")) NewTransaction = Command_rls( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lls")) NewTransaction = Command_lls( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rdir")) NewTransaction = Command_rls( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "ldir")) NewTransaction = Command_lls( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rlist")) NewTransaction = Command_rls( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "llist")) NewTransaction = Command_lls( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rcd")) NewTransaction = Command_rcd( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lcd")) NewTransaction = Command_lcd( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rrm")) NewTransaction = Command_rrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lrm")) NewTransaction = Command_lrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rrmfile")) NewTransaction = Command_rrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lrmfile")) NewTransaction = Command_lrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rdel")) NewTransaction = Command_rrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "ldel")) NewTransaction = Command_lrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rdelete")) NewTransaction = Command_rrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "ldelete")) NewTransaction = Command_lrm( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rrmdir")) NewTransaction = Command_rrmdir( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lrmdir")) NewTransaction = Command_lrmdir( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rmkdir")) NewTransaction = Command_rmkdir( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lmkdir")) NewTransaction = Command_lmkdir( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rmd")) NewTransaction = Command_rmkdir( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lmd")) NewTransaction = Command_lmkdir( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rmv")) NewTransaction = Command_rmv( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lmv")) NewTransaction = Command_lmv( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rmove")) NewTransaction = Command_rmv( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "lmove")) NewTransaction = Command_lmv( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rtype")) NewTransaction = Command_rtype( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "rprint")) NewTransaction = Command_rtype( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "get")) NewTransaction = Command_get( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "mget")) NewTransaction = Command_mget( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "download")) NewTransaction = Command_get( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "put")) NewTransaction = Command_put( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "mput")) NewTransaction = Command_mput( CommandTrailer);
			else if ( yathCmpBufWStr( CommandToken, CommandTokenLength, "upload")) NewTransaction = Command_put( CommandTrailer);
			else ::Beep( 3000, 200);
		}
	}

	return NewTransaction;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/
