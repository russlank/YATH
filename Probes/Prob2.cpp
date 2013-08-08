#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <iostream.h>

/*
const char *FindToken( const char *StartFrom, const char *&ParamStart, int &ParamLength, BOOL &IsOption)
{
	const char *Result;
	
	ParamLength = 0;

	if ( StartFrom != NULL){

		while ( *StartFrom == ' ') StartFrom++;

		if ( *StartFrom == '"'){
			
			IsOption = FALSE;
			StartFrom++;
			ParamStart = StartFrom;
		
			while ( *StartFrom != 0){
				
				if ( *StartFrom != '"'){
					StartFrom++;
					ParamLength++;
				} else {
					StartFrom++;
					break;
				}

			}

		} else if ( *StartFrom != 0){

			if ( *StartFrom == '-') {
				IsOption = TRUE;
				StartFrom++;
			} else IsOption = FALSE;
			
			ParamStart = StartFrom;
			
			while ( *StartFrom != 0) {

				if (( *StartFrom != ' ') && ( *StartFrom != '"')){
					StartFrom++;
					ParamLength++;
				} else break;

			} 

		} else ParamStart = NULL;

		if ( *StartFrom != 0) Result = StartFrom;
		else Result = NULL;

	} else {
		IsOption = FALSE;
		ParamStart = NULL;
		Result = NULL;
	}

	return Result;
}
*/

/*
int main(int argc, char *argv[])
{
	// char String[] = "dir -abc zico.com \"jasem\" -dido \"\"\"hazem";
	char String[] = "--";
	const char *Scanner = String;
	const char *ParamStart;
	int ParamLength;
	BOOL Option;

	do {
		Scanner = FindToken( Scanner, ParamStart, ParamLength, Option);
		if ( ParamStart != NULL){
			printf( "Length = %d, Option = %d, '%s'\n", ParamLength, Option, ParamStart);
		}
	} while ( Scanner != NULL);

	char c = getchar();

	return (0);
}
*/

/*
int main(int argc, char *argv[])
{
	printf("MAX_PATH = %d\n", MAX_PATH);
	char c = getchar();

	return (0);
}
*/

/////////////////////////////////////////////////////////////

char *yathPathFindFileName( char *Path)
{
	char *Result;
	if ( Path != NULL){
		char *ScanPtr = Path;
		Result = ScanPtr;
		while ( *ScanPtr != '\0'){
			if ( *ScanPtr == '\\') {
				ScanPtr++;
				Result = ScanPtr;
			} if ( *ScanPtr == ':'){
				ScanPtr++;
				if ( *ScanPtr != '\0') Result = ScanPtr;
			} else ScanPtr++;
		}
	} else Result = NULL;
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathPathAppend( char *Path, const char *Name, int MaxSize)
{
	BOOL Result = FALSE;

	if (( Path != NULL) && ( Name != NULL) && (MaxSize > 1)){
		int Remain = MaxSize - 1;
		char *ScanPtr = Path;
		BOOL NeedBkSlash = FALSE;
		
		while ( *ScanPtr != '\0'){
			if ( Remain > 0) {
				NeedBkSlash = (( *ScanPtr != '\\') && ( *ScanPtr != ':'));
				ScanPtr++;
				Remain--;
			} else goto NoRoom;
		}
		
		if ( NeedBkSlash) {
			if ( Remain > 0) {
				*ScanPtr = '\\';
				ScanPtr++;
				Remain--;
			} else goto NoRoom;
		}

		while ( *Name != '\0') {
			if ( Remain > 0) {
				*ScanPtr = *Name;
				ScanPtr++;
				Name++;
				Remain--;
			} else goto NoRoom;
		}

		Result = TRUE;

NoRoom:;
	   *ScanPtr = '\0';
	}

	return Result;
}

/////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	char FullPath[23];

	FullPath[0] = '\0';

	cout << FullPath << "\n";

	if ( !::yathPathAppend( FullPath, "C:", sizeof( FullPath))) cout << "ERROR\n";

	cout << FullPath << " - FN: " << ::yathPathFindFileName( FullPath) << "\n";
	
	if ( !::yathPathAppend( FullPath, "JASEM", sizeof( FullPath))) cout << "ERROR\n";

	cout << FullPath << " - FN: " << ::yathPathFindFileName( FullPath) << "\n";

	if ( !::yathPathAppend( FullPath, "HASAN\\", sizeof( FullPath))) cout << "ERROR\n";

	cout << FullPath << " - FN: " << ::yathPathFindFileName( FullPath) << "\n";

	if ( !::yathPathAppend( FullPath, "ZICO", sizeof( FullPath))) cout << "ERROR\n";

	cout << FullPath << " - FN: " << ::yathPathFindFileName( FullPath) << "\n";

	if ( !::yathPathAppend( FullPath, "*.*", sizeof( FullPath))) cout << "ERROR\n";

	cout << FullPath << " - FN: " << ::yathPathFindFileName( FullPath) << "\n";

	char c;
	cin >> c;
}