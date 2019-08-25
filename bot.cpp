#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#define invalid 1000
#define infinity 1000000
#define white 0
#define black 1

using namespace std;
typedef vector<vector<int>> Board;

// bot functions
void setBoard(Board b);
int calcCenterDistance(string pos);
int calcGrouping(string pos);
void generateMarbles();
int evaluate();
string findNeighbour(string pos, string dir);
vector<int> minimax(int depth, bool isMax);
string botMove();
void display();

class abalone
{
	friend class bot;

	// data members
	vector<vector<int>> board;
	int activePlayer; // assuming white starts
	int noOfMarbles[2];

	// member functions
	bool isStringValid(string);
	vector<int> getPair(string s); // gets the indices of an RD pair
	string getCoor(vector<int> xy); // gets string coordinates of vector indices
	vector<vector<vector<int>>> getCoordinates(string& r1d1, string& r2d2);//, string r3d3); // gets the coordinate list of a string
	int mod(string x1y1, string x2y2); // returns marbles selected
	string getDirection(string rdi, string rdf); // calculates the direction to go in
	void move(vector<vector<int>> initialCoor, vector<vector<int>> finalCoor); // moves pieces from initial to final position
	int getWhites();
	int getBlacks();
	bool cornerPush(vector<vector<int>>, string);
	bool properSelected(vector<vector<int>>, vector<vector<int>>, int); // checks if active player's marbles are selected
	bool inBoard(vector<vector<int>> v); //returns true if move is in board; pass vec in row,col
	int winner(); // returns winners number
	vector<vector<vector<int>>> isValid(string s); // returns initial and final coordinates if the move is valid
													// else will return an empty vector

	// bot functions
	friend void setBoard(abalone a);
	friend int calcCenterDistance(string pos);
	friend int calcGrouping(string pos);
	friend void generateMarbles();
	friend int evaluate();
	friend string findNeighbour(string pos, string dir);
	friend vector<int> minimax(int depth, bool isMax);
	friend string botMove();
	friend void display();


public:
	abalone(int player);
	void displayGame(); // display in game format
	void play(); // actual gameplay function
	void playWithBot(); // playing against bot
};

abalone::abalone(int player = white)
{
	activePlayer = player; // assuming white starts
	noOfMarbles[black] = 11;
	noOfMarbles[white] = 11;

	// white: 0/O, black: 1/@

	board = {
				  {  0,  0,  0,  0},

				{  0,  0,  0,  0,  0},

			  { -1, -1,  0,  0, -1, -1},

			{ -1, -1, -1, -1, -1, -1, -1},

			  { -1, -1,  1,  1, -1, -1},

				{  1,  1,  1,  1,  1},

				  {  1,  1,  1,  1}
	};
}

void abalone::displayGame()
{
	char row = 'A';
	int dia = 7;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 2 - i; j >= 0; j--)
		{
			cout << " ";
		}
		cout << row << " ";
		for (int j = 0; j < board[i].size(); j++)
		{
			if (board[i][j] == -1) cout << "+ ";
			else if (board[i][j] == 0) cout << "O ";
			else cout << "@ ";
		}
		cout << endl;
		row++;
	}
	for (int i = 4; i < 7; i++)
	{
		for (int j = 0; j < i - 3; j++)
		{
			cout << " ";
		}
		cout << row << " ";
		for (int j = 0; j < board[i].size(); j++)
		{
			if (board[i][j] == -1) cout << "+ ";
			else if (board[i][j] == white) cout << "O ";
			else cout << "@ ";
		}
		cout << dia << endl;
		row++;
		dia--;
	}
	for (int i = 0; i < 6; i++) cout << " ";
	for (int i = 0; i < 4; i++, dia--) cout << 5 - dia << " ";
	cout << endl;
}

bool abalone::isStringValid(string str)
{

	bool ret = (str.length() == 5); //A1 B1
	if (ret)
	{
		for (int i = 0; i < 4; i += 3)
			ret = (str[i] >= 'A' && str[i] <= 'G');
		for (int i = 1; i < 5; i += 3)
			ret = ret && (str[i] >= '1' && str[i] <= '7');

		ret = ret && (str[2] == ' ');
	}
	return ret;
}

int abalone::mod(string x1y1, string x2y2)
{
	if (x1y1[0] == x2y2[0]) // marbles are along same row
	{
		return abs(x1y1[1] - x2y2[1]) + 1;
	}
	else if (x1y1[1] == x2y2[1]) // marbles along same diagonal
	{
		return abs(x1y1[0] - x2y2[0]) + 1;
	}
	else // along different row and diagonal
	{
		if (x1y1[0] - x2y2[0] == x2y2[1] - x1y1[1]) // marbles along proper line
		{
			return abs(x1y1[0] - x2y2[0]) + 1;
		}
		else return invalid;
	}
}

vector<int> abalone::getPair(string s)
{
	vector<int> xy; // indices of respective input board coordinates
	xy.push_back(s[0] - 65);
	if (s[0] >= 'A' && s[0] <= 'C') xy.push_back(s[0] + s[1] - 117);
	else xy.push_back(s[1] - 49);
	return xy;
}

string abalone::getCoor(vector<int> xy)
{
	string coor = "++";
	coor[0] = xy[0] + 65;
	if(coor[0] >= 'D') coor[1] = xy[1] + 49;
	else coor[1] = 52 - xy[0] + xy[1];
	return coor;
}

vector<vector<vector<int>>> abalone::getCoordinates(string& r1d1, string& r2d2)//, string r3d3)
{
	// R: Row D: Diagonal
	vector<vector<int>> iniCoor;
	vector<vector<int>> finCoor;

	string dir = getDirection(r1d1, r2d2);
	int deltaX = 0, deltaY = 0;
	/*
		deltaX and deltaY will indicate what to add to the current position
		for west, decrease col; for east increase col....
	*/
	if (dir == "WW") { deltaX = 0;	deltaY = -1; }
	else if (dir == "EE") { deltaX = 0;	deltaY = 1; }
	else if (dir == "NW") { deltaX = -1;	deltaY = 0; }
	else if (dir == "SE") { deltaX = 1;	deltaY = 0; }
	else if (dir == "NE") { deltaX = -1;	deltaY = 1; }
	else if (dir == "SW") { deltaX = 1;	deltaY = -1; }

	string i = r2d2; //changed from r1d1 to r2d2
	string j = i;
	j[0] += deltaX;	j[1] += deltaY;

	string oppLast = j;
	vector<int> opp = getPair(oppLast); // assumed or probable opponent position

	if (inBoard({ opp })) // if assumed opponent position is out of the board
	{
		int x = activePlayer;
		if (board[opp[0]][opp[1]] == !activePlayer) // if opponent is found in the moving block
		{
			while (board[opp[0]][opp[1]] == !activePlayer) // loop until you find the final opponent block
			{
				oppLast[0] += deltaX;	oppLast[1] += deltaY;
				opp = getPair(oppLast);
				// index is goin out of bounds
				if (!inBoard({ opp })) break;

				// if our marble exists after the marble to be pushed
				else if (board[opp[0]][opp[1]] == activePlayer) return {};
			}
			oppLast[0] -= deltaX;	oppLast[1] -= deltaY;
		}
		else oppLast = "\0"; // opponent doesnt exist where the player is moving
	}
	else return {};

	// if opponent exists,  append it to the list to be moved
	if (oppLast != "\0")
	{
		// if opponent has more than one marble, push not possible
		if (mod(j, oppLast) > 1) return {};

		string oppI = oppLast, oppJ = "++";
		oppJ[0] = oppI[0] + deltaX;	oppJ[1] = oppI[1] + deltaY;
		iniCoor.push_back(getPair(oppI));
		finCoor.push_back(getPair(oppJ));
	}

	j = i;
	i = r1d1;
	for (; i != r2d2; i[0] += deltaX, i[1] += deltaY, j[0] += deltaX, j[1] += deltaY)
	{
		iniCoor.push_back(getPair(i));
		finCoor.push_back(getPair(j));
	}

	iniCoor.push_back(getPair(i));
	finCoor.push_back(getPair(j));

	return { iniCoor, finCoor };
}

string abalone::getDirection(string rdi, string rdf)
{
	if (rdi[0] == rdf[0]) // along same row
	{
		if (rdi[1] < rdf[1]) return "EE"; // going east
		else return "WW"; // going west
	}
	else if (rdi[1] == rdf[1]) // along same diagonal
	{
		if (rdi[0] < rdf[0]) return "SE";
		else return "NW";
	}
	else // along NE direction
	{
		if (rdi[0] < rdf[0]) return "SW";
		else return "NE";
	}
}

void abalone::move(vector<vector<int>> initialCoor, vector<vector<int>> finalCoor)
{
	int flag = 0;
	if (board[initialCoor[0][0]][initialCoor[0][1]] != activePlayer) // if we are pushing an opponent
	{
		// if opponent is being pushed out of the board
		if (finalCoor[0][0] < 0 || finalCoor[0][0] >= board.size() || finalCoor[0][1] < 0 || finalCoor[0][1] >= board[finalCoor[0][0]].size())
		{
			(activePlayer == white) ? noOfMarbles[black]-- : noOfMarbles[white]--;
			flag = 1;
		}
	}
	for (int i = flag; i < initialCoor.size(); i++)
	{
		// new coordinates will get the current pieces
		board[finalCoor[i][0]][finalCoor[i][1]] = board[initialCoor[i][0]][initialCoor[i][1]]; // commented

		// old coordinates will be declared empty
		if (i != initialCoor.size() - 1)
			board[initialCoor[i][0]][initialCoor[i][1]] = -1; //commented
	}
}

bool abalone::properSelected(vector<vector<int>> iniCoor, vector<vector<int>> finCoor, int n)
{
	for (int i = 1; i <= n; i++)
	{
		// if vector has opponents marbles appended, we must count from the end

		int x = iniCoor[iniCoor.size() - i][0], y = iniCoor[iniCoor.size() - i][1];
		// false if initial coordinates have wrong player on board
		int temp = activePlayer;
		if (board[x][y] != activePlayer) return false;

	}
	int x = finCoor[finCoor.size() - 1][0];
	int y = finCoor[finCoor.size() - 1][1];
	if (board[x][y] == activePlayer) return false;
	return true;
}

bool abalone::cornerPush(vector<vector<int>> inicoor, string dir)
{
	int y = inicoor[0][0];
	int x = inicoor[0][1];
	if (board[y][x] != activePlayer)
	{
		if (y == 0)
		{
			if (x == 0 && dir == "NW") return 1;
			else if (x == 3 && dir == "NE") return 1;
			else return 0;
		}
		else if (y == 3)
		{
			if (x == 0 && dir == "WW") return 1;
			else if (x == 6 && dir == "EE") return 1;
			else return 0;
		}
		else if (y == 6)
		{
			if (x == 0 && dir == "SW") return 1;
			else if (x == 3 && dir == "SE") return 1;
			else return 0;
		}
		else return 0;
	}
	else return 0;
}

bool abalone::inBoard(vector<vector<int>> finalPos) {
	bool flag = false;
	const int rowIndex = 6;
	int count = 0, i, index;
	for (i = 0; i < finalPos.size(); i++) {
		if (finalPos[i][0] <= rowIndex && finalPos[i][0] >= 0) {
			index = finalPos[i][0];
			if (finalPos[i][1] < board[index].size() && finalPos[i][1] >= 0) {
				count++;
			}
		}
	}
	if (count == i) {
		flag = true;
	}
	return flag;
}

int abalone::getBlacks()
{
	return noOfMarbles[black];
}

int abalone::getWhites()
{
	return noOfMarbles[white];
}

int abalone::winner()
{
	if (noOfMarbles[black] == 7) return white;
	else if (noOfMarbles[white] == 7) return black;
	else return -1;
}

vector<vector<vector<int>>> abalone::isValid(string s)
{
	// if invalid return falseVec

	vector<vector<vector<int>>> falseVec;
	vector<vector<vector<int>>> coordinates;

	if (!isStringValid(s)) return falseVec;

	string rd1 = s.substr(0, 2), rd2 = s.substr(3, 2);

	// checks if selected spots are in board
	if (!inBoard({ getPair(rd1), getPair(rd2) })) return falseVec;

	// checking if number of marbles selected is 1-3
	if (mod(rd1, rd2) != 2) return falseVec;

	int a = activePlayer;
	coordinates = getCoordinates(rd1, rd2);

	// if coordinates is empty
	if (coordinates.size() == 0) return falseVec;

	// checking if final coordinates lie in board provided we are not pushing an opponent
	int x = coordinates[0][0][0], y = coordinates[0][0][1];
	if (board[x][y] == activePlayer)
	{
		if (!inBoard(coordinates[1]))
		{
			return falseVec;
		}
	}

	// if proper marbles are selected
	if (!properSelected(coordinates[0], coordinates[1], 2)) return falseVec;

	string dir = getDirection(rd1, rd2);
	if (cornerPush(coordinates[0], dir))
		return falseVec;

	return coordinates;
}

void abalone::play()
{
	displayGame();
	string s;
	// input form : initial marble, final marble
	while (winner() == -1)
	{
		(activePlayer) ? cout << "Black's Move: " : cout << "White's Move: ";
		getline(cin, s);

		if (s == "exit") return;

		vector<vector<vector<int>>> coor = isValid(s); // returns an empty vector for a false move
														// if move is valid, gives us initial and final coordinates

		if (coor.size()) // move is valid
		{
			move(coor[0], coor[1]);
			activePlayer = !activePlayer; // switch players
		}

		else cout << "Invalid Move!" << endl;

		displayGame();
	}
	(winner()) ? cout << "Black Wins!" << endl : cout << "White Wins!" << endl;
	return;
}

void abalone::playWithBot()
{
	displayGame();
	string s;
	while(winner() == -1)
	{
		// assuming player starts
		cout << "Your Move: ";
		getline(cin, s);

		if (s == "exit") return;

		vector<vector<vector<int>>> coor = isValid(s); // returns an empty vector for a false move
														// if move is valid, gives us initial and final coordinates

		if (coor.size()) // move is valid
		{
			move(coor[0], coor[1]);
			activePlayer = !activePlayer; // switch players
		}
		else
		{
			cout << "Invalid Move!" << endl;
			continue;
		}

		if(winner() == white)
		{
			cout << "You Win!" << endl;
			return;
		}

		displayGame();

		setBoard(*this);
		generateMarbles();
		string compMove = botMove();
		cout << "Bot's Move: " << compMove << endl;
		coor = isValid(compMove);
		move(coor[0], coor[1]);
		displayGame();
		activePlayer = !activePlayer;

		if(winner() == black)
		{
			cout << "The Bot Wins!" << endl;
			return;
		}
	}
}


struct marble
{
	string position;
	int centerDistance;
	int grouping;
};

abalone comp(black);
vector<vector<marble>> marbles;
int botPlayer = black;
int user = white;

void setBoard(abalone a)
{
	comp.board = a.board;
	comp.noOfMarbles[white] = a.noOfMarbles[white];
	comp.noOfMarbles[black] = a.noOfMarbles[black];
}

int calcCenterDistance(string pos)
{
	int dist = comp.mod(pos, "D4");
	if(dist != invalid) return dist-1; // if it is inline with the center
	
	// center is always inline with a position one move away from the marble...
	// ... if the marble is not already inline
	string t1, t2, t3, t4;
	t1 = t2 = t3 = t4 = pos;
	t1[0]--; t2[0]++; t3[1]--; t4[1]++; // 4 possible positions for being inline
	int d1 = comp.mod(t1, "D4"), d2 = comp.mod(t2, "D4"), d3 = comp.mod(t3, "D4") ,d4 = comp.mod(t4, "D4");
	int m1 = min(d1, d2), m2 = min(d3, d4);
	return min(m1, m2); // one which is least away is the actual center distance
}

int calcGrouping(string pos)
{
	int count = 0; // total neighbours
	vector<int> temp = comp.getPair(pos);
	int active = comp.board[temp[0]][temp[1]];
	string n[6]; // string positions of 6 neighbours
	for(int i = 0; i < 6; i++) n[i] = pos;
	n[0][0]--; n[1][0]++; n[2][1]--; n[3][1]++; 
	n[4][0]--; n[4][1]++; n[5][0]++; n[5][1]--;
	for(int i = 0; i < 6; i++)
	{
		vector<int> xy = comp.getPair(n[i]);
		if(comp.inBoard({ xy })) // if neighbour is in the board
		{
			if(comp.board[xy[0]][xy[1]] == active) count++; // if neighbour is the same player's marble
		}
	}
	return count;
}

void display()
{
	comp.displayGame();
}

void generateMarbles()
{
	marbles.erase(marbles.begin(), marbles.end());
	if(marbles.size() == 0)
	{
		vector<marble> t1;
		vector<marble> t2;
		marbles.push_back(t1);
		marbles.push_back(t2);
	}
	for(int i = 0; i < comp.board.size(); i++)
	{
		for(int j = 0; j < comp.board[i].size(); j++)
		{
			// if white marble is found add it to white sparse matrix, if black then black sparse matrix
			if(comp.board[i][j] == white)
			{
				string coor = comp.getCoor({i, j});
				marbles[white].push_back({coor, calcCenterDistance(coor), calcGrouping(coor)});
			}

			else if(comp.board[i][j] == black)
			{
				string coor = comp.getCoor({i, j});
				marbles[black].push_back({coor, calcCenterDistance(coor), calcGrouping(coor)});
			}
		}
	}
	comp.noOfMarbles[white] = marbles[white].size();
	comp.noOfMarbles[black] = marbles[black].size();
}

int evaluate()
{
	if(comp.winner() == botPlayer)
	{
		return infinity;
	}
	else if(comp.winner() == !botPlayer)
	{
		return -infinity;
	}
	
	int value = 0;

	// +1000 for each marble bot has extra, -1000 for each marble player has extra
	int centerDistance = 0, grouping = 0, profit;
	if(botPlayer == white)
	{
		profit = comp.noOfMarbles[white] - comp.noOfMarbles[black];
		value += profit *1000;
		for(int i = 0; i < marbles[white].size(); i++)
		{
			centerDistance += marbles[white][i].centerDistance;
			grouping += marbles[white][i].grouping;
		}
	}
	else
	{
		profit = comp.noOfMarbles[black] - comp.noOfMarbles[white];
		value += profit *1000;
		for(int i = 0; i < marbles[black].size(); i++)
		{
			centerDistance += marbles[black][i].centerDistance;
			grouping += marbles[black][i].grouping;
		}
	}
	

	if(centerDistance < 18) value += 400;
	else if(centerDistance < 23) value += 300;
	else if(centerDistance < 27) value += 200;
	else if(centerDistance < 32) value += 100;

	if(grouping > 35) value += 320;
	else if(grouping > 30) value += 240;
	else if(grouping > 25) value += 160;
	else if(grouping > 20) value += 80;

	return value;
	
}

string findNeighbour(string pos, string dir)
{
	if(dir == "EE")
	{
		pos[1]++;
	}
	else if(dir == "WW")
	{
		pos[1]--;
	}
	else if(dir == "NE")
	{
		pos[0]--;	pos[1]++;
	}
	else if(dir == "SW")
	{
		pos[0]++;	pos[1]--;
	}
	else if(dir == "SE")
	{
		pos[0]++;
	}
	else if(dir == "NW")
	{
		pos[0]--;
	}
	return pos;
}

void showMarbles()
{
	for(int i = 0; i < marbles[black].size(); i++) cout << marbles[black][i].position << " ";
	cout << endl;
}

vector<int> minimax(int depth, bool isMax)
{
	int value = evaluate();

	if(value == infinity) return {infinity, depth};	// bot wins
	else if(value == -infinity) return {infinity, depth};	// player wins
	else if(depth == 2) return {value, depth};

	if(isMax) // maximizer's move
	{
		int maxVal = -infinity;
		int leastDepth = 100;
		for(int i = 0; i < marbles[botPlayer].size(); i++) // each available bot's marble
		{
			if(marbles[botPlayer][i].grouping) // marble must have neighbours
			{
				string dirs[] = {"EE", "WW", "NW", "SE", "SW", "NE"};
				int count = 0; // keeps count of number of neighbours visited
				for(int j = 0; j < 6; j++)
				{
					string marblePos = marbles[botPlayer][i].position;
					string neighbour = findNeighbour(marblePos, dirs[j]);
					vector<vector<int>> coors = {comp.getPair(marblePos), comp.getPair(neighbour)};
					// neighbour must exist
					if(comp.inBoard(coors))
					{
						vector<int> xy = comp.getPair(neighbour);
						// neighbour must be the same marble
						if(comp.board[xy[0]][xy[1]] == botPlayer)
						{
							// similar logic to play() in abalone

							Board temp = comp.board;
							string tempMove = marblePos + " " + neighbour;
							comp.activePlayer = black;
							vector<vector<vector<int>>> coordinates = comp.isValid(tempMove);
							if(coordinates.size())
							{
								// making the move
								comp.move(coordinates[0], coordinates[1]);
								generateMarbles();
								comp.activePlayer = !comp.activePlayer;

								vector<int> val = minimax(depth+1, false);
								if(val[0] > maxVal)
								{
									maxVal = val[0];
									leastDepth = val[1];
								}
								else  if(val[0] >= maxVal && val[1] < depth)
								{
									maxVal = val[0];
									leastDepth = val[1];
								}

								// undo the move
								comp.activePlayer = !comp.activePlayer;
								comp.board = temp;
								generateMarbles();
							}
							count++;
						}

						// if all neighbours have been visited, no need to look further
						if(count == marbles[botPlayer][i].grouping) break;
					}
				}
			}
		}

		return {maxVal, leastDepth};
	}
	else // minimizer's code
	{
		int minVal = infinity;
		int leastDepth = 100;
		for(int i = 0; i < marbles[user].size(); i++) // each available bot's marble
		{
			if(marbles[user][i].grouping) // marble must have neighbours
			{
				string dirs[] = {"EE", "WW", "NW", "SE", "SW", "NE"};
				int count = 0; // keeps count of number of neighbours visited
				for(int j = 0; j < 6; j++)
				{
					string marblePos = marbles[user][i].position;
					string neighbour = findNeighbour(marblePos, dirs[j]);
					vector<vector<int>> coors = {comp.getPair(marblePos), comp.getPair(neighbour)};
					// neighbour must exist
					if(comp.inBoard(coors))
					{
						vector<int> xy = comp.getPair(neighbour);
						// neighbour must be the same marble
						if(comp.board[xy[0]][xy[1]] == user)
						{
							// similar logic to play() in abalone

							Board temp = comp.board;
							string tempMove = marblePos + " " + neighbour;
							vector<vector<vector<int>>> coordinates = comp.isValid(tempMove);
							if(coordinates.size())
							{
								// making the move
								comp.move(coordinates[0], coordinates[1]);
								generateMarbles();
								comp.activePlayer = !comp.activePlayer;

								vector<int> val = minimax(depth+1, true);
								if(val[0] < minVal)
								{
									minVal = val[0];
									leastDepth = val[1];
								}
								else  if(val[0] <= minVal && val[1] < depth)
								{
									minVal = val[0];
									leastDepth = val[1];
								}

								// undo the move
								comp.activePlayer = !comp.activePlayer;
								comp.board = temp;
								generateMarbles();
							}
							count++;
						}

						// if all neighbours have been visited, no need to look further
						if(count == marbles[user][i].grouping) break;
					}
				}
			}
		}

		return {minVal, leastDepth};
	}
	
}

string botMove()
{
	int bestVal = -infinity, leastDepth = 100;
	string bestMove;
	for(int i = 0; i < marbles[botPlayer].size(); i++) // each available bot's marble
	{
		if(marbles[botPlayer][i].grouping) // marble must have neighbours
		{
			string dirs[] = {"EE", "WW", "NW", "SE", "SW", "NE"};
			int count = 0; // keeps count of number of neighbours visited
			for(int j = 0; j < 6; j++)
			{
				string marblePos = marbles[botPlayer][i].position;
				string neighbour = findNeighbour(marblePos, dirs[j]);
				vector<vector<int>> coors = {comp.getPair(marblePos), comp.getPair(neighbour)};

				// neighbour must exist
				if(comp.inBoard(coors))
				{
					vector<int> xy = comp.getPair(neighbour);
					// neighbour must be the same marble
					if(comp.board[xy[0]][xy[1]] == botPlayer)
					{
						// similar logic to play() in abalone

						Board temp = comp.board;
						string tempMove = marblePos + " " + neighbour;

						vector<vector<vector<int>>> coordinates = comp.isValid(tempMove);
						if(coordinates.size())
						{
							// making the move
							comp.move(coordinates[0], coordinates[1]);
							generateMarbles();
							comp.activePlayer = !comp.activePlayer;

							vector<int> val = minimax(0, false);
							if(val[0] > bestVal)
							{
								bestVal = val[0];
								leastDepth = val[1];
								bestMove = tempMove;
							}
							else if(val[0] == bestVal && val[1] < leastDepth)
							{
								bestVal = val[0];
								leastDepth = val[1];
								bestMove = tempMove;
							}

							// undo the move
							comp.activePlayer = !comp.activePlayer;
							comp.board = temp;
							generateMarbles();
						}
						count++;
					}

					// if all neighbours have been visited, no need to look further
					if(count == marbles[botPlayer][i].grouping) break;
				}
			}
		}
	}
	return bestMove;
}

int main()
{
	abalone a;
	a.playWithBot();
	cout << "Press any key to continue...";
	getchar();
}
