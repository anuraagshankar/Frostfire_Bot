import time
import random
import math
import copy
white = 0
black = 1

b = {
    'A4': white,'A5': white,'A6': white,'A7': white,
    'B3': white,'B4': white,'B5': white,'B6': white,'B7': white,
    'C4': white,'C5': white,

    'E3': black,'E4': black,
    'F1': black,'F2': black,'F3': black,'F4': black,'F5': black,
    'G1': black,'G2': black,'G3': black,'G4': black
    }

limits = [['A4', 'A7'],['B3', 'B7'],['C2', 'C7'],['D1', 'D7'],['E1', 'E6'],['F1', 'F5'],['G1', 'G4']]

marbles = [0,0]

def getDirection(rdi, rdf):
    if rdi[0] == rdf[0]:
        if rdi[1] < rdf[1]: return 'EE'
        else: return 'WW'
    elif rdi[1] == rdf[1]:
        if rdi[0] < rdf[0]: return 'SE'
        else: return 'NW'
    else:
        if rdi[0] < rdf[0]: return 'SW'
        else: return 'NE'

def inLimit(rd):
    i = ord(rd[0]) - 65
    if i < 0 or i > 6: return False
    elif rd >= limits[i][0] and rd <= limits[i][1]: return True
    return False

def getNeighbour(rd, dir):
    x = rd
    if dir == "EE":
        x = x[0] + chr(ord(x[1]) + 1)
    elif dir == "WW":
        x = x[0] + chr(ord(x[1]) - 1)
    elif dir == "NE":
        x = chr(ord(x[0]) - 1) + chr(ord(x[1]) + 1)
    elif dir == "SW":
        x = chr(ord(x[0]) + 1) + chr(ord(x[1]) - 1)
    elif dir == "NW":
        x = chr(ord(x[0]) - 1) + x[1]
    elif dir == "SE":
        x = chr(ord(x[0]) + 1) + x[1]

    return x

class GameState:
    def __init__(self):
        self.whosemove = active # active player
        self.board = b
        self.marbles = marbles
        self.goFlag = 0 # true if move is valid

    def Clone(self):
        temp = GameState()
        temp.whosemove = self.whosemove
        temp.board = copy.deepcopy(self.board)
        temp.marbles = copy.deepcopy(self.marbles)
        temp.goFlag = self.goFlag
        return temp

    def display(self):
        row = 'A'
        dia = 7
        for i in range(0, 4):
            for j in range(2-i, -1, -1):
                print(" ", end = '')
            print(row + ". ", end = '')
            m = limits[i][0]
            for j in range(ord(limits[i][1][1]) - ord(limits[i][0][1])+1):
                if m in self.board:
                    if self.board[m] == white:
                        print('W ', end = '')
                    else: print('B ', end = '')
                else: print('_ ', end = '')
                m = m[0] + chr(ord(m[1]) + 1)
            print("")
            row = chr(ord(row) + 1)
        for i in range(4, 7):
            for j in range(i-3):
                print(" ", end = '')
            print(row + '. ', end='')
            m = limits[i][0]
            for j in range(ord(limits[i][1][1]) - ord(limits[i][0][1])+1):
                if m in self.board:
                    if self.board[m] == white:
                        print('W ', end = '')
                    else: print('B ', end = '')
                else: print('_ ', end = '')
                m = m[0] + chr(ord(m[1]) + 1)
            print(str(dia))
            row = chr(ord(row) + 1)
            dia -= 1
        print("       1 2 3 4")

    def mValidation(self, move):
        m1 = move[0:2]
        m2 = move[3:5]
        dir = getDirection(m1, m2)
        deltaX = ord(m2[0]) - ord(m1[0])
        deltaY = ord(m2[1]) - ord(m1[1])
	    
        i = ord(m1[0]) + deltaX
        j = ord(m1[1]) + deltaY
        x = chr(i) + chr(j)

        if m2 != x: return False # two non-adjacent marbles selected
        elif m1 not in self.board or m2 not in self.board: return False # blank spaces selected
        elif self.board[m1] != self.board[m2] or self.board[m1] == 1 - self.whosemove: return False # opp is selected or two different marbles are selected

        i += deltaX
        j += deltaY
        m3 = chr(i) + chr(j)

        i += deltaX
        j += deltaY
        m4 = chr(i) + chr(j)

        if not inLimit(m3): return False # new position not in board

        if m3 in self.board:
            if self.board[m3] == self.whosemove or m4 in self.board: return False

        if(m3 == "A4" and dir == "NW") or (m3 == "A7" and dir == "NE") or (m3 == "D1" and dir == "WW") or (m3 == "D7" and dir == "EE") or (m3 == "G1" and dir == "SW") or (m3 == "G4" and dir == "SE"): return False # corner push check

        return True

    def DoMove(self, move):
        if self.mValidation(move):
            m1 = move[0:2]
            m2 = move[3:5]
            deltaX = ord(m2[0]) - ord(m1[0])
            deltaY = ord(m2[1]) - ord(m1[1])
            
            i = ord(m2[0]) + deltaX
            j = ord(m2[1]) + deltaY
            m3 = chr(i) + chr(j)

            i += deltaX
            j += deltaY
            m4 = chr(i) + chr(j)

            if m3 in self.board: # being pushed
                if not inLimit(m4): # being KOed
                    self.board[m3] = self.whosemove
                    if self.whosemove == white: self.marbles[black] += 1
                    else: self.marbles[white] += 1
                self.board[m4] = 1 - self.whosemove
                self.board[m3] = self.whosemove
                self.board[m2] = self.whosemove
            else: # only moving around
                self.board[m3] = self.whosemove
            
            self.board.pop(m1)
            self.whosemove = 1 - self.whosemove

    def GetMoves(self):
        if self.marbles[black] == 4 or self.marbles[white] == 4: return []
        moves = []
        active = self.whosemove
        for i in self.board.keys():
            if self.board[i] == active:
                for dir in ["EE", "WW", "NE", "NW", "SE", "SW"]:
                    n = getNeighbour(i, dir)
                    if n in self.board:
                        if self.board[i] == active:
                            if self.mValidation(i + " " + n): moves.append(i + " " + n)
        
        return moves

    def isWin(self):
        if self.marbles[white] == 4:
            return black
        elif self.marbles[black] == 4:
            return white
        else:
            return -1
    
    def GetResult(self):
        result = self.isWin()
        
        if result == 1 and self.whosemove == 0 :
            return 0.0
        elif result == 0 and self.whosemove == 1:
            return 1.0
        else:
            return 0.5

    def isGameOver(self):
        if self.marbles[black] == 4 or self.marbles[white] == 4: return True
        else: return False

class Node:
    """ A node in the game tree. Note wins is always from the viewpoint of playerJustMoved.
        Crashes if state not specified.
    """

    def __init__(self, move=None, parent=None, state=None):
        self.move = move  # the move that got us to this node - "None" for the root node
        self.parentNode = parent  # "None" for the root node
        self.childNodes = []
        self.wins = 0
        self.visits = 0
        self.untriedMoves = state.GetMoves()  # future child nodes
        self.whosemove = state.whosemove  # the only part of the state that the Node needs later

    def UCTSelectChild(self):
        """ Use the UCB1 formula to select a child node. Often a constant UCTK is applied so we have
            lambda c: c.wins/c.visits + UCTK * sqrt(2*log(self.visits)/c.visits to vary the amount of
            exploration versus exploitation.
        """
        s = sorted(self.childNodes, key=lambda c: c.wins / c.visits + math.sqrt(2 * math.log(self.visits) / c.visits))[-1]
        return s

    def AddChild(self, m, s):
        """ Remove m from untriedMoves and add a new child node for this move.
            Return the added child node
        """
        n = Node(move=m, parent=self, state=s)
        self.untriedMoves.remove(m)
        self.childNodes.append(n)
        return n

    def Update(self, result):
        """ Update this node - one additional visit and result additional wins. result must be from the viewpoint of playerJustmoved.
        """
        self.visits += 1
        self.wins += result

    def __repr__(self):
        return "[M:" + str(self.move) + " W/V:" + str(self.wins) + "/" + str(self.visits) + " U:" + str(
            self.untriedMoves) + "]"

def UCT(rootstate, itermax, verbose = False):
    """ Conduct a UCT search for itermax iterations starting from rootstate.
        Return the best move from the rootstate.
        Assumes 2 alternating players (player 1 starts), with game results in the range [0.0, 1.0]."""

    rootnode = Node(state=rootstate)

    for i in range(itermax):
        node = rootnode
        state = rootstate.Clone()

        # Select
        while node.untriedMoves == [] and node.childNodes != []:  # node is fully expanded and non-terminal

            node = node.UCTSelectChild()
            state.DoMove(node.move)


        # Expand
        if node.untriedMoves != []:  # if we can expand (i.e. state/node is non-terminal)
            m = random.choice(node.untriedMoves)
            state.DoMove(m)
            node = node.AddChild(m, state)  # add child and descend tree

        # Rollout - this can often be made orders of magnitude quicker using a state.GetRandomMove() function
        while state.GetMoves() != []:  # while state is non-terminal
            state.DoMove(random.choice(state.GetMoves()))

        # Backpropagate
        while node != None:  # backpropagate from the expanded node and work back to the root node
            node.Update(state.GetResult())
            node = node.parentNode

    return sorted(rootnode.childNodes, key=lambda c: c.visits)[-1].move  # return the move that was most visited

def UCTPlayGame():
    """ Play a sample game between two UCT players where each player gets a different number
        of UCT iterations (= simulations = tree nodes).
    """
    start_time = time.time()
    state = GameState()
    m = UCT(rootstate=state, itermax=500, verbose=False)  # play with values for itermax and verbose = True
    print("Bot's Move: " + m)
    state.DoMove(m)
    print("--- %s seconds ---" % (time.time() - start_time))

def checkWin():
    if marbles[black] == 4:
        return white
    elif marbles[white] == 4:
        return black
    else: return -1

active = 0
g = GameState()
g.display()
while g.isGameOver() == False:
    UCTPlayGame()
    g.display()
    active = 1-active
    if checkWin() == white:
        print("White Wins!")
        break
    UCTPlayGame()
    g.display()
    active = 1-active
    if checkWin() == black:
        print("Black Wins!")
        break
