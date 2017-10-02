import random
import time
import threading
import termcolor
import re
import msvcrt


def letterScore(N):
    """returns Scrabble value of a letter"""
    if N == 'A' or N =='E' or N =='I' or N == 'O' or N == 'N' or N == 'R' or N == 'T' or N == 'L' or N =='S' or N == 'U':
        score = 1
    elif N == 'D' or N == 'G':
        score = 2
    elif N == 'B' or N =='C' or N =='M' or N =='P':
        score = 3
    elif N == 'F' or N == 'H' or N == 'V' or N == 'W' or N =='Y':
        score = 4
    elif N == 'K':
        score = 5
    elif N ==  'X' or N =='J':
        score = 8
    else:
        score = 10
        
    return score

def scrabbleScore(S):
        """returns the value of a word based on its Scrabble value"""
        score = 0
        for i in range(len(S)):
            score += letterScore(S[i])
                
        return score

def lengthScore(word):
    """returns the value of a word based on its length"""
    score = 0
    if len(word) <= 3:
        return 0
    if len(word) >= 4:
        score += 2
    if len(word) >= 5:
        score += 3
        
        for i in range(5, len(word)):
            score += 5
    return score
        


def isAWord(S):
    f = open('Dictionary.txt')
    text = f.read()
    f.close()

    LoW = text.split()
    if S in LoW:
        return True
    else:
        return False


def count(e,L):
    """counts elements e in list L"""
    count = 0
    for i in range(len(L)):
        if L[i] == e:
            count += 1
    return count

def unlist(A):
    """turns a list of lists with same number of elements into one list"""
    L = []
    for x in range(len(A)):
        for y in range(len(A[x])):
            L += [A[x][y]]
            
    return L

def listToLoL(L):
    """Turns a list into a list of lists or a string into a list of strings"""
    LoL = []
    LoL.append(L)    
    return LoL

def duplicateInList(L):
    """Returns True if there is two occurrences of an element in the list"""
    for i in range(len(L)):
        for j in range(len(L)):
            if i != j and L[i] == L[j]:
                return True
    return False

def escape_ansi(line):
    ansi_escape = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')
    return ansi_escape.sub('', line)

def removeAll(e, L):
    """remove all occurrences of element e from list L"""
    newL = []
    for i in range(len(L)):
        if e != L[i]:
            newL += [L[i]]
    return newL

def makeGroups(L, number):
    """returns new list with elements group in LoLs of length number. Assume len(L) % number == 0"""
    newL = []
    for i in range(0, len(L), number):
        sublist = L[i:i+number]
        newL += [sublist]
    return newL

def removeAfter(e, L):
    """return a list with all elemets after element e removed"""
    newL = []
    i = 0
    while e != L[i]:
        newL += [L[i]]
        i += 1
    newL += [L[i]]
    return newL
        

class Board:
    """ a datatype representing a Boggle board
        with an arbitrary number of rows and cols
    """
    
    def __init__( self, width, height, DLPos, DWPos, TLPos, TWPos):
        """ the constructor for objects of type Board """
        self.width = width
        self.height = height
        self.DLPos = DLPos
        self.DWPos = DWPos
        self.TLPos = TLPos
        self.TWPos = TWPos
        W = self.width
        H = self.height
        self.data = [ [' ']*W for row in range(H) ]

        # we do not need to return inside a constructor!
        

    def __repr__(self):
        """ this method returns a string representation
            for an object of type Board
        """
        H = self.height
        W = self.width
        s = ''   # the string to return
        
        for row in range(0,H):
            s += ' '   
            for col in range(0,W):
                s += self.data[row][col] + ' '
            s += '\n'

        return s 
    
    def makeWhiteCopy(self):
        """returns a copy of the board"""
        copy = Board(self.width, self.height, self.DLPos, self.DWPos, self.TLPos, self.TWPos)
        W = copy.width
        H = copy.height
        for row in range(H):
            for col in range(W):
                copy.data[row][col] = self.data[row][col]

        copy.makeBoardWhite()
        return copy


    
    def createDoubleBoard(self): 
        """Creates a random double Boggle board"""
        self.createBoard()

        # Double Word Position
        L = [[0,0], [0,1], [0,2], [0,3], [1,0], [1,1], [1,2], [1,3], [2,0], [2,1], [2,2], [2,3], [3,0], [3,1], [3,2], [3,3]]
        x = random.choice(L)
        doubleWordPos = x
        
        colorRed = termcolor.colored(self.data[x[0]][x[1]], 'red')   
        self.data[x[0]][x[1]] = colorRed


        # Double Letter Position
        L.remove(x)
        y = random.sample(L, 2)
        y1 = y[0]
        y2 = y[1]
        doubleLetterPos = [y1, y2]

        colorBlue1 = termcolor.colored(self.data[y1[0]][y1[1]], 'cyan')
        colorBlue2 = termcolor.colored(self.data[y2[0]][y2[1]], 'cyan')
        self.data[y1[0]][y1[1]] = colorBlue1
        self.data[y2[0]][y2[1]] = colorBlue2
        
        self.DLPos = doubleLetterPos
        self.DWPos = doubleWordPos 

    def createTripleBoard(self): 
        """Creates a random triple Boggle board"""
        self.createBoard()

        # Triple Word Position
        L = [[0,0], [0,1], [0,2], [0,3], [1,0], [1,1], [1,2], [1,3], [2,0], [2,1], [2,2], [2,3], [3,0], [3,1], [3,2], [3,3]]
        x = random.choice(L)
        tripleWordPos = x
        
        colorYellow = termcolor.colored(self.data[x[0]][x[1]], 'yellow')   
        self.data[x[0]][x[1]] = colorYellow


        # Triple Letter Position
        L.remove(x)
        y = random.sample(L, 2)
        y1 = y[0]
        y2 = y[1]
        tripleLetterPos = [y1, y2]

        colorGreen1 = termcolor.colored(self.data[y1[0]][y1[1]], 'green')
        colorGreen2 = termcolor.colored(self.data[y2[0]][y2[1]], 'green')
        self.data[y1[0]][y1[1]] = colorGreen1
        self.data[y2[0]][y2[1]] = colorGreen2
        
        self.TLPos = tripleLetterPos
        self.TWPos = tripleWordPos         
        
        
        
    def createBoard(self):
        """Creates a random Boggle board"""
        W = self.width
        H = self.height
        for row in range(0,W):
            for col in range(0,H):
                x = random.randrange(1,99)
                if x <= 9:
                    self.data[row][col] = 'A'
                elif 10 <= x < 12:
                    self.data[row][col] = 'B'
                elif 12 <= x < 14:
                    self.data[row][col] = 'C'
                elif 14 <= x < 18:
                    self.data[row][col] = 'D'
                elif 18 <= x < 30:
                    self.data[row][col] = 'E'        
                elif 30 <= x < 32:
                    self.data[row][col] = 'F'
                elif 32 <= x < 35:
                    self.data[row][col] = 'G'
                elif 35 <= x < 37:
                    self.data[row][col] = 'H'        
                elif 37 <= x < 46:
                    self.data[row][col] = 'I'
                elif 46 <= x < 47:
                    self.data[row][col] = 'J'
                elif 47 <= x < 48:
                    self.data[row][col] = 'K'
                elif 48 <= x < 52:
                    self.data[row][col] = 'L'
                elif 52 <= x < 54:
                    self.data[row][col] = 'M'        
                elif 54 <= x < 60:
                    self.data[row][col] = 'N'
                elif 60 <= x < 68:
                    self.data[row][col] = 'O'
                elif 68 <= x < 70:
                    self.data[row][col] = 'P'        
                elif 70 <= x < 71:
                    self.data[row][col] = 'Q'
                elif 71 <= x < 77:
                    self.data[row][col] = 'R'        
                elif 77 <= x < 81:
                    self.data[row][col] = 'S'
                elif 81 <= x < 87:
                    self.data[row][col] = 'T'
                elif 87 <= x < 91:
                    self.data[row][col] = 'U'        
                elif 91 <= x < 93:
                    self.data[row][col] = 'V'
                elif 93 <= x < 95:
                    self.data[row][col] = 'W'
                elif 95 <= x < 96:
                    self.data[row][col] = 'X'
                elif 96 <= x < 98:
                    self.data[row][col] = 'Y'
                else:
                    self.data[row][col] = 'Z' 

    

    def firstLetter(self, letter):
        """returns row and col of character within the Boggle board. 
        False if character is not present"""
        
        W = self.width
        H = self.height
        L = []
        for row in range(W):                            # initial letter
            for col in range(H):
                if letter == self.data[row][col]:
                    L += [[row,col]]
        
        return L                   
                            

    def checkEight(self, r, c, letter):
        """checks the eight spaces surrounding a spot and returns the positions
        of the input letter within those 8 spaces"""
        W = self.width
        H = self.height
        L = []
        for row in range(r-1, r+2):          # check 8 letters around it
            for col in range(c-1, c+2): 
                if row < 0:
                    pass
                elif row > H-1:
                    pass
                elif col < 0:
                    pass
                elif col > W-1:
                    pass
                elif row == r and col == c:
                    pass
                elif self.data[row][col] == letter:
                    L += [[row,col]]
        if L == []:
            return []
        else:
            return L              
        
   

    def checkWord(self, word):    
        """Returns True if word is in Board"""
        copy = self.makeWhiteCopy()
        if copy.allPaths(word) == []:
            return False
        else:
            return True
        

    def allPaths(self, word):
        """returns all available paths in Board to make word, no repeats"""
        allPathsNoRepeats = []
        allPaths = self.allPathsHelper3(word)
        for path in allPaths:
            if duplicateInList(path) == False:
                allPathsNoRepeats += [path]
        return allPathsNoRepeats


    def allPathsHelper3(self, word):
        """returns all available paths in Board to make word, with repeats"""
        firstLetterPos = self.firstLetter(word[0])
        paths = []

        # Find paths for any starting point
        for i in range(len(firstLetterPos)):
            paths += self.allPathsHelper2(word, [firstLetterPos[i]], [])
        return paths


    def allPathsHelper2(self, word, currentLetterPos, toAdd):
        x = self.allPathsHelper1(word, currentLetterPos, toAdd)
        
        # Group list into lists of individual paths
        paths = makeGroups(x, len(word))
        return paths



    def allPathsHelper1(self, word, currentLetterPos, toAdd):
        paths = []

        # Base case: return position of final letter
        if len(word) == 1:  
            for i in range(len(currentLetterPos)):          
                
                return toAdd + [currentLetterPos[i]]

        
        else:

            # For each of the choices for the current letter
            for i in range(len(currentLetterPos)):
                
                    
                # Find the position of the next Letter
                nextLetterPos = self.checkEight(currentLetterPos[i][0], currentLetterPos[i][1], word[1])                    
                
                # Keep track of position you were last at
                if currentLetterPos[i] in toAdd:
                    return []

                toAdd += [currentLetterPos[i]] 
                
                # For each choice for the next letter
                for j in range(len(nextLetterPos)):    

                    # Find path using a choice for next letter                       
                    paths += self.allPathsHelper1(word[1:], [nextLetterPos[j]], toAdd)

                    # After finding this path, reset toAdd to where you were before finding the last path
                    toAdd = removeAfter(currentLetterPos[i], toAdd)

                return paths                      


    def doubleRoundScrabbleScore(self, word):
        """returns scrabble score for double Round"""
        copy = self.makeWhiteCopy()        

        doubleLetterPos = copy.DLPos
        doubleWordPos = copy.DWPos
        
        allPaths = copy.allPaths(word)        
        highScore = 0        
        # double letter score 
        for path in range(len(allPaths)):
            score = 0
            for i in range(len(word)): 
                
                if allPaths[path][i] ==  doubleLetterPos[0] or \
                    allPaths[path][i] ==  doubleLetterPos[1]:
                    score += 2*letterScore(word[i])
                
                else:
                    score += letterScore(word[i])

                # double word score
            for i in range(len(word)):
                if allPaths[path][i] == doubleWordPos:
                    score *= 2        
                    
            if score > highScore:
                highScore = score

        return highScore
    
    def tripleRoundScrabbleScore(self, word):
        """returns scrabble score for triple Round"""
        copy = self.makeWhiteCopy()

        tripleLetterPos = copy.TLPos
        tripleWordPos = copy.TWPos
        
        allPaths = copy.allPaths(word)        
        highScore = 0        
        # triple letter score 
        for path in range(len(allPaths)):
            score = 0
            for i in range(len(word)): 
                
                if allPaths[path][i] ==  tripleLetterPos[0] or \
                    allPaths[path][i] ==  tripleLetterPos[1]:
                    score += 3*letterScore(word[i])
                
                else:
                    score += letterScore(word[i])

                # triple word score
            for i in range(len(word)):
                if allPaths[path][i] == tripleWordPos:
                    score *= 3        
                    
            if score > highScore:
                highScore = score

        return highScore

    def makeBoardWhite(self):
        """Changes all characters in double or triple round board to default white color"""
        h = self.height
        w = self.width
        for row in range(h):
            for col in range(w):
                self.data[row][col] = escape_ansi(self.data[row][col])
    

def game(numPlayers, roundLength=60):
    """Play WordStreak"""
    print('\n')
    
    round1Score = []
    for i in range(numPlayers):
        round1Score += [0]

    round2Score = []
    for i in range(numPlayers):
        round2Score += [0]

    round3Score = []
    for i in range(numPlayers):
        round3Score += [0]

    totalScore = []
    for i in range(numPlayers):
        totalScore += [0]

    # Round 1
    b1 = Board(4,4,[],[],[],[])
    b1.createBoard()

    for i in range(1, numPlayers+1):        

        print('Player',i, ': Round 1')

        input("Press Enter to start...")
        
        print('\n')
        time.sleep(1)
        print('Game begins in 3...')
        time.sleep(1)
        print('2...')
        time.sleep(1)
        print('1...')
        time.sleep(1)
        print('GO!')
        print('\n')
        print('\n')
        print(b1)        
        
        timeout = time.time() + roundLength
        L = []              
        
        
        while True:  

            if msvcrt.kbhit():  
                word = str.upper(str(input('')))
                if isAWord(word) == False:
                    round1Score[i-1] += 0
                    print('Invalid word!')
                    print(round1Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif b1.checkWord(word) == False:
                    round1Score[i-1] += 0
                    print('Not in the board!')
                    print('Total:', round1Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif word in L:
                    round1Score[i-1] += 0
                    print('Already found!')
                    print('Total:', round1Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif len(word) < 3:
                    round1Score[i-1] += 0
                    print('3-letter word minimum!')
                    print(round1Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                else:
                    round1Score[i-1] += scrabbleScore(word) + lengthScore(word)
                    print('+', scrabbleScore(word) + lengthScore(word))
                    print('Total:', round1Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                    L.append(word)
            else:
                if time.time() > timeout:
                    break
            
        print('STOP! Player',i, 'score for Round 1:', round1Score[i-1])

        time.sleep(3)

        print(50*'\n')
        
        
    for i in range(numPlayers):
        totalScore[i] += round1Score[i]    

    
    playerList = ['']
    for i in range(1, numPlayers+1):
        playerList += ['Player' + str(i)]

    round1ScoreList = ['ROUND 1'] + round1Score
    round2ScoreList = ['ROUND 2'] + round2Score
    round3ScoreList = ['ROUND 3'] + round3Score    
    totalScoreList = ['TOTAL'] + totalScore

    S = ''
    for i in range(numPlayers+1):
        S += '{' + str(i) + ':^10}'


    print('---------'*(numPlayers+1))
    for line in [playerList, round1ScoreList, round2ScoreList, round3ScoreList, totalScoreList]:
        print(S.format(*line))
    print('---------'*(numPlayers+1))
    print('\n')
    input("Press Enter to go to Round 2")


    # Round 2
    print(10*'\n')
    b1 = Board(4,4,[],[],[],[])
    b1.createDoubleBoard()

    for i in range(1, numPlayers+1):        

        print('Player',i, ': Round 2')

        input("Press Enter to start...")
        
        print('\n')
        time.sleep(1)
        print('Game begins in 3...')
        time.sleep(1)
        print('2...')
        time.sleep(1)
        print('1...')
        time.sleep(1)
        print('GO!')
        print('\n')
        print('\n')
        print(b1)        
        
        timeout = time.time() + roundLength
        L = []              
        
        while True:    
            if msvcrt.kbhit():  
                word = str.upper(str(input('')))
                if isAWord(word) == False:
                    round2Score[i-1] += 0
                    print('Invalid word!')
                    print(round2Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif b1.checkWord(word) == False:
                    round2Score[i-1] += 0
                    print('Not in the board!')
                    print('Total:', round2Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif word in L:
                    round2Score[i-1] += 0
                    print('Already found!')
                    print('Total:', round2Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif len(word) < 3:
                    round2Score[i-1] += 0
                    print('3-letter word minimum!')
                    print(round2Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                else:
                    round2Score[i-1] += b1.doubleRoundScrabbleScore(word) + lengthScore(word)
                    print('+', b1.doubleRoundScrabbleScore(word) + lengthScore(word))
                    print('Total:', round2Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                    L.append(word)
            else:
                if time.time() > timeout:
                    break

        print('STOP! Player',i, 'score for Round 2:', round2Score[i-1])

        time.sleep(3)

        print(50*'\n')
        
        
    for i in range(numPlayers):
        totalScore[i] += round2Score[i]    

    
    playerList = ['']
    for i in range(1, numPlayers+1):
        playerList += ['Player' + str(i)]

    round1ScoreList = ['ROUND 1'] + round1Score
    round2ScoreList = ['ROUND 2'] + round2Score
    round3ScoreList = ['ROUND 3'] + round3Score    
    totalScoreList = ['TOTAL'] + totalScore

    

    S = ''
    for i in range(numPlayers+1):
        S += '{' + str(i) + ':^10}'


    print('---------'*(numPlayers+1))
    for line in [playerList, round1ScoreList, round2ScoreList, round3ScoreList, totalScoreList]:
        print(S.format(*line))
    print('---------'*(numPlayers+1))
    print('\n')


    # Round 3

    print(10*'\n')
    b1 = Board(4,4,[],[],[],[])
    b1.createTripleBoard()

    for i in range(1, numPlayers+1):        

        print('Player',i, ': Round 3')

        input("Press Enter to start...")
        
        print('\n')
        time.sleep(1)
        print('Game begins in 3...')
        time.sleep(1)
        print('2...')
        time.sleep(1)
        print('1...')
        time.sleep(1)
        print('GO!')
        print('\n')
        print('\n')
        print(b1)        
        
        timeout = time.time() + roundLength
        L = []              
        
        while True:    
            if msvcrt.kbhit():  
                word = str.upper(str(input('')))
            
                if isAWord(word) == False:
                    round3Score[i-1] += 0
                    print('Invalid word!')
                    print(round3Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif b1.checkWord(word) == False:
                    round3Score[i-1] += 0
                    print('Not in the board!')
                    print('Total:', round3Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif word in L:
                    round3Score[i-1] += 0
                    print('Already found!')
                    print('Total:', round3Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                elif len(word) < 3:
                    round3Score[i-1] += 0
                    print('3-letter word minimum!')
                    print(round3Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                else:
                    round3Score[i-1] += b1.tripleRoundScrabbleScore(word) + lengthScore(word)
                    print('+', b1.tripleRoundScrabbleScore(word) + lengthScore(word))
                    print('Total:', round3Score[i-1])
                    print('\n')
                    print(b1)
                    print('\n')
                    L.append(word)
            else:
                if time.time() > timeout:
                    break

        print('STOP! Player',i, 'score for Round 3:', round3Score[i-1])

        time.sleep(3)

        print(50*'\n')
        
        
    for i in range(numPlayers):
        totalScore[i] += round3Score[i]    

    
    playerList = ['']
    for i in range(1, numPlayers+1):
        playerList += ['Player' + str(i)]

    round1ScoreList = ['ROUND 1'] + round1Score
    round2ScoreList = ['ROUND 2'] + round2Score
    round3ScoreList = ['ROUND 3'] + round3Score    
    totalScoreList = ['TOTAL'] + totalScore
    

    S = ''
    for i in range(numPlayers+1):
        S += '{' + str(i) + ':^10}'


    print('---------'*(numPlayers+1))
    for line in [playerList, round1ScoreList, round2ScoreList, round3ScoreList, totalScoreList]:
        print(S.format(*line))
    print('---------'*(numPlayers+1))
    print('\n')


    # WINNER
    highScore = max(totalScoreList[1:])

    if count(highScore, totalScoreList) > 1:
        print(termcolor.colored('TIE!!', 'green'))

    else:
        highScore = 0
        highScoreIndex = 0
        for i in range(1, len(totalScoreList)):
            if totalScoreList[i] > highScore:
                highscore = totalScoreList[i]
                highScoreIndex = i
        print('PLAYER',i,'WINS!!')


                



       
       
        
    

                       




          
    
