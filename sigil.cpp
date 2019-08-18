// Solve a tetramino board : place all the tetraminos pieces so that they don't overlap and fill the board exactly.
// Jerome Marhic
// 2015

#include <iostream>
using namespace std;
#include <vector>
#include <assert.h>
#include <list>
#include <string>
#include <map>

// width (columns) and height (rows) of the board
const int W = 8;
const int H = 6;

template<class T>
void copyBoard( const T board[W][H], T newBoard[W][H] )
{
	for( int i=0; i<W; i++ )
	{
		for( int j=0; j<H; j++ )
		{
			newBoard[i][j] = board[i][j];
		}
	}
}

template<class T>
void displayBoard( const T board[W][H] )
{
	for( int i=0; i<W; i++ )
	{
		for( int j=0; j<H; j++ )
		{
			cout<< (int)board[i][j];
		}
		cout<<endl;
	}
	cout<<endl;
}

void displayPiece( const bool piece[4][4] )
{
	for( int i=0; i<4; i++ )
	{
		for( int j=0; j<4; j++ )
		{
			cout<< piece[i][j];
		}
		cout<<endl;
	}
	cout<<endl;
}

// rotate by 90 degrees to the right
// possible to use with source == dest
void rotate( const bool source[4][4], bool dest[4][4] )
{
	bool temp[4][4];
	for( int i=0; i<4; i++ )
	{
		for(int j=0; j<4; j++ )
		{
			temp[i][j] = false;
		}
	}
	for( int i=0; i < 4; i++ )
	{
		for( int j=0; j<4; j++ )
		{
			temp[j][3-i] = source[i][j];
		}
	}
	for( int i=0; i<4; i++ )
	{
		for(int j=0; j<4; j++ )
		{
			dest[i][j] = temp[i][j];
		}
	}
	return;
}

// return false if overlap or some part is outside
bool copyInto( const bool board[W][H], const bool piece[4][4], int x, int y, bool newBoard[W][H] )
{
	for( int i=0; i<W; i++ )
	{
		for( int j=0; j<H; j++ )
		{
			newBoard[i][j] = board[i][j];
		}
	}

	for( int i=0; i<4; i++ )
	{
		for( int j=0; j<4; j++ )
		{
			if( piece[i][j] )
			{
				if( x + i >= W || y + j >= H )
					return false;
				if( board[x+i][y+j] != false )// overlap
					return false;

				newBoard[x+i][y+j] = true;
			}
		}
	}
	return true;
}

void placePiece( const bool pieces[W*H/4][4][4], bool board[W][H], int pieceIndex, int answer[W][H] )
{
	//cout<<"piece "<<pieceIndex<<endl;
	//displayBoard(answer);

	if( pieceIndex >= (W*H/4) )
	{
		cout<<"SUCCESS"<<endl;
		displayBoard(answer);
		system("pause");
		return;
	}

	// try the 4 different possible rotations of the piece
	for( int nbrot = 0; nbrot < 4; nbrot++ )
	{
		bool p[4][4];
		for( int i=0; i<4; i++ )
		{
			for( int j=0; j<4; j++ )
			{
				p[i][j] = pieces[pieceIndex][i][j];
			}
		}
		for( int i=0; i<nbrot; i++ )
		{
			rotate(p,p);
		}
		//cout<<"piece "<<pieceIndex<<endl;
		//displayPiece(p);
		//system("pause");

		for( int i=0; i<W; i++ )
		{
			for( int j=0; j<H; j++ )
			{
				bool newBoard[W][H];
				bool r = copyInto( board, p, i, j, newBoard );
				if( r )
				{
					int newAnswer[W][H];
					copyBoard(answer,newAnswer);
					for(int x=0; x<4; x++ )
					{
						for( int y=0; y<4; y++ )
						{
							if( p[x][y] )
							{
								newAnswer[i+x][j+y] = pieceIndex;
							}
						}
					}

					//displayBoard(newBoard);
					//system("pause");
					placePiece(pieces,newBoard,pieceIndex+1,newAnswer);
				}
			}
		}
	}
}

void listPositions( int pieceIndex, int nbpieces, const vector< pair<int,int> >& piece, vector< vector< int > >& boardPos )
{
	// list all the positions where we can place this piece on the board
	for( int row=0; row<H; row++ )
	{
		for( int col=0; col<W; col++ )
		{
			vector<int> newPos(nbpieces + W*H, 0); // initialized to 0
			newPos[pieceIndex] = 1; // this indicates the L shape

			bool ok = true;
			for( int i=0; i<piece.size(); i++ )
			{
				pair<int,int> p = piece[i];
				if( row+p.first >= H || col+p.second >= W )
				{
					ok = false;
					break;
				}
				else
				{
					int posIndex = nbpieces + (row+p.first)*W+(col+p.second);
					assert( posIndex < newPos.size() );
					newPos[posIndex] = 1;
				}
			}

			if( ok ) // add this position to the list:
			{
				boardPos.push_back( newPos );
			}
		}
	}
}

struct DataObject
{
	DataObject()
	{
		L = R = U = D = C = 0;
		N = "";
		isColumn = false;
		row = -1;
		col = -1;
	}
	// left, right, up, down and column header
	DataObject* L;
	DataObject* R;
	DataObject* U;
	DataObject* D;
	DataObject* C;
	string N;
	// debug info
	bool isColumn;
	int row;
	int col;
};


DataObject* createData( const vector< vector< int > >& boardPos )
{
	DataObject* header = new DataObject();
	DataObject* previous = header;
	// create and link each column and the header :
	vector<DataObject*> columnHeader( boardPos[0].size() );
	for( int col=0; col< boardPos[0].size(); col++ )
	{
		columnHeader[col] = new DataObject();
		columnHeader[col]->L = previous;
		columnHeader[col]->D = columnHeader[col];
		columnHeader[col]->U = columnHeader[col];
		columnHeader[col]->isColumn = true;
		int nbpieces = W*H/4;
		if( col < nbpieces )
		{
			columnHeader[col]->N = '0' + col;
		}
		else
		{
			columnHeader[col]->N = '0' + (int)( (col-nbpieces) / W );
			columnHeader[col]->N += '0' + (col-nbpieces) % W;
		}
		cout<<columnHeader[col]->N<<endl;
		previous->R = columnHeader[col];
		previous = columnHeader[col];
	}
	header->L = columnHeader[boardPos[0].size()-1];
	columnHeader[boardPos[0].size()-1]->R = header;


	// for each "1" in each column, create a data object
	vector< DataObject* > objects( boardPos.size() * boardPos[0].size(), NULL );
	for( int i=0; i< boardPos.size(); i++ ) // rows
	{
		for( int j=0; j< boardPos[0].size(); j++ ) // columns
		{
			if( boardPos[i][j] == 1 )
			{
				DataObject* newObject = new DataObject();
				newObject->C = columnHeader[j];
				newObject->row = i;
				newObject->col = j;
				assert( newObject->C != NULL );
				objects[ i * boardPos[0].size() + j ] = newObject;
				// link the object with the column header
				newObject->U = columnHeader[j]->U;
				newObject->D = columnHeader[j];
				newObject->U->D = newObject;
				columnHeader[j]->U = newObject;

				newObject->R = newObject;
				newObject->L = newObject;
			}
		}
	}
	// now create the left-right links
	for( int o=0; o< objects.size(); o++ )
	{
		DataObject* object = objects[o];
		if( object != NULL )
		{
			assert( object->U != NULL);
			assert( object->D != NULL);
			assert( object->C != NULL );

			// Find the next non-null object on the same row
			int rowIndex = (int)( o / boardPos[0].size() );
			int colIndex = o % boardPos[0].size();

			int index = colIndex+1;
			while( index != colIndex )
			{
				if( index >= boardPos[0].size() ) // loop to the 'left' of the row
					index = 0;

				int objectIndex = rowIndex * boardPos[0].size() + index;
				if( objects[objectIndex] != NULL )
				{
					object->R = objects[objectIndex];
					objects[objectIndex]->L = object;
					break;
				}

				index++;
			}

		}
	}

	return header;
}

// this is a subset of the dancing link algorithm below
void coverColumn( DataObject* c )
{
	assert( c != NULL );
	assert( c->isColumn );
	c->R->L = c->L;
	c->L->R = c->R;
	DataObject* i = c->D;
	while( i != c )
	{
		DataObject* j = i->R;
		while( j != i )
		{
			// remove this object from his column :
			assert( j->D != NULL );
			assert( j->U != NULL );
			j->D->U = j->U;
			j->U->D = j->D;
			j = j->R;
		}
		i = i->D;
	}
}

void uncoverColumn( DataObject* c )
{
	assert( c != NULL );
	assert( c->isColumn );
	DataObject* i = c->U;
	while( i != c )
	{
		DataObject* j = i->L;
		while( j != i )
		{
			j->D->U = j;
			j->U->D = j;
			j = j->L;
		}
		i = i->U;
	}
	c->R->L = c;
	c->L->R = c;
}

void printSolution(const vector<DataObject*>& rows)
{
	for( int i=0; i< rows.size(); i++ )
	{
		DataObject* o = rows[i];
		cout<<"piece "<<o->C->N<<" : ";
		DataObject* r = o->R;
		while( r != o )
		{
			cout<<r->C->N;
			r = r->R;
			if( r!= o )
			{
				cout<<" ";
			}
		}
		cout<<endl;
	}
}

// Use Knuth "dancing link" algorithm to solve the exact cover problem on this set
// http://arxiv.org/pdf/cs/0011047v1.pdf
// Dancing Links
void solveCoverProblem( vector<DataObject*>& rows, int k, DataObject* header )
{
	assert( header != NULL );
	assert( header->R != NULL );
	if( header->R == header )
	{
		cout<<"SOLVED"<<endl;
		printSolution(rows);
		return; // problem solved
	}

	// choose a column object :
	DataObject* c = header->R;
	// "cover" the column (see paper) :
	coverColumn(c);

	DataObject* r = c->D;
	while( r != c )
	{
		rows[k] = r;
		DataObject* j = r->R;
		while( j != r )
		{
			coverColumn(j->C);
			j = j->R;
		}
		solveCoverProblem(rows, k+1, header);

		j = r->L;
		while( j != r )
		{
			uncoverColumn(j->C);
			j = j->L;
		}
		r = r->D;
	}
	uncoverColumn(c);
	return;
}

int main()
{
	bool L[4][4] = {
	{ 1, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 1, 1, 0, 0 },
	{ 0, 0, 0, 0 }};

	bool L2[4][4] = {
	{ 1, 0, 0, 0 },
	{ 1, 1, 1, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }};

	bool T[4][4] = {
	{ 1, 1, 1, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }};

	bool I[4][4] = {
	{ 1, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 1, 0, 0, 0 }};

	bool S[4][4] = {
	{ 1, 0, 0, 0 },
	{ 1, 1, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 0, 0 }};

	bool S2[4][4] = {
	{ 1, 1, 0, 0 },
	{ 0, 1, 1, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }};

	// C like 'Carre' (square in french)
	bool C[4][4] = {
	{ 1, 1, 0, 0 },
	{ 1, 1, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }};

	// Store the number of different possible rotations
	// For example the 'S' piece has 2, and the square only 1.
	// That will effectively avoid doubles in the matrice later.
	map<int,int> numRotations; // piece -> num different rotations
	numRotations.insert( pair<int,int>( (int)&L,4) );
	numRotations.insert( pair<int,int>( (int)&L2,4) );
	numRotations.insert( pair<int,int>( (int)&T,4) );
	numRotations.insert( pair<int,int>( (int)&I,2) );
	numRotations.insert( pair<int,int>( (int)&S,2) );
	numRotations.insert( pair<int,int>( (int)&S2,2) );
	numRotations.insert( pair<int,int>( (int)&C,1) );

	vector<int> pieceList;
	// This is the pieces we want to use for our puzzle:
	pieceList.push_back( (int)&S2 );
	pieceList.push_back( (int)&S2 );
	pieceList.push_back( (int)&S2 );
	pieceList.push_back( (int)&S );
	pieceList.push_back( (int)&C );
	pieceList.push_back( (int)&C );
	pieceList.push_back( (int)&T );
	pieceList.push_back( (int)&T );
	pieceList.push_back( (int)&L2 );
	pieceList.push_back( (int)&L2 );
	pieceList.push_back( (int)&L2 );
	pieceList.push_back( (int)&I );

	assert( pieceList.size() == W*H/4 );
	int nbpieces = pieceList.size();

	// Prepare the board (the 0 and 1 matrix), the first nbpieces columns
	// indicate the piece index that this row uses, and the next W*H columns
	// have a 1 to indicate the piece presence in that cell. See paper.
	vector< vector< int > > boardPos;
	for( int p=0; p< pieceList.size(); p++ )
	{
		int nbRotations = numRotations[ pieceList[p] ];
		for( int rot=0; rot<nbRotations; rot++ )
		{
			bool pieceCopy[4][4];
			for( int i=0; i<4; i++ )
			{
				for( int j=0; j<4; j++ )
				{
					//pieceCopy[i][j] = pieces[p][i][j];
					pieceCopy[i][j] = (bool) (*((bool(*)[4][4])pieceList[p]))[i][j];
				}
			}

			for( int i=0; i<rot; i++ )
			{
				rotate(pieceCopy,pieceCopy);
			}

			// The piece configuration
			vector<pair<int,int>> pos;
			// keep track of the min value (when we rotate the
			// piece, it can go to the 'right' of the matrices)
			int minRow = 4;
			int minCol = 4;
			for( int i=0; i<4; i++ )
			{
				for( int j=0; j<4; j++ )
				{
					if( pieceCopy[i][j] == 1 )
					{
						minRow = std::min( i, minRow );
						minCol = std::min( j, minCol );
						pos.push_back( pair<int,int>( i, j ) );
					}
				}
			}
			for(int i=0; i< pos.size(); i++ )
			{
				pos[i].first -= minRow;
				pos[i].second -= minCol;
			}

			// list all the possible board positions for this piece configuration :
			listPositions( p, nbpieces, pos,  boardPos );
		}
	}

	// The board is ready, we need to find an exact cover set
	// we use dancing link algorithm
	DataObject* header = createData( boardPos );
	vector<DataObject*> rows( nbpieces );
	solveCoverProblem(rows,0,header);


	system("pause");
	return 0;
}
