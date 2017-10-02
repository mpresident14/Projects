import random

def listToString(L):
        """converts list to String"""
        S = ''
        for x in L:
            S += str(x)
        return S

def viableWord(word, letterPlaces):
    """returns True if word is a possible solution to LoL letterPlaces"""
    for i in range(len(letterPlaces)):
        if letterPlaces[i][1] != word[letterPlaces[i][0]]:
            return False
    return True
    
    
class Hangman:
    """datatype representing an AI Hangman solver"""

    def __init__(self, numLetters):
        """constructor"""        
        self.numLetters = numLetters
        self.currentBoard = ['_'] * numLetters
        self.data = ['_'] * numLetters

    def __repr__(self):
        """string representation for Hangman object"""
        numLetters = self.numLetters
        S = ''
        S += 3*'\n'
        S += '           '
        for i in range(numLetters):
            S += self.data[i] + ' '

        return S
    

    def inputLetter(self, letter, L):
        """input a correctly guessed letter into the correct spaces of list L"""
        if L != []:           
        
            for x in L:
                self.data[x] = letter  
                self.currentBoard[x] = letter 

    
    def findPossibleWords(self):
        """return a list of all words that fit parameters of the string currentBoard"""
        numLetters = self.numLetters
        currentBoard = self.currentBoard
        
        letterPlaces = []
        for i in range(len(currentBoard)):
            if currentBoard[i] != '_':
                letterPlaces += [[i, currentBoard[i]]]
        
        # Grab list of all words
        f = open('Dictionary.txt')
        text = f.read()
        f.close()
        wordList = text.split()

        # Filter list to words of same length
        sameLengthWords = []
        for word in wordList:
            if len(word) == numLetters:
                sameLengthWords += [word]

       
        # Now filter sameLengthWords based on currentBoard
        possibleWords = []
        for word in sameLengthWords:
            if viableWord(word, letterPlaces) == True:
                possibleWords += [word]

        return possibleWords

    def nextLetter(self, L):
        """returns most likely next letter choice based on currentBoard
        and list L of used letters"""
        possibleWords = self.findPossibleWords()
        currentBoard = self.currentBoard

        # Remove used letters from list
        allLetters = list('ABCDEFGHIJKLMNOPQRSTUVWXYZ')

        for i in range(len(L)):
            if L[i] in allLetters:
                allLetters.remove(L[i])
        
        letterCount = len(allLetters)*[0]

        # Count the number of words each letters appears in
        for word in possibleWords:
            for i in range(len(allLetters)):
                if allLetters[i] not in currentBoard:  # don't count letters already discovered
                    if allLetters[i] in word:
                        letterCount[i] += 1
        
        # Determine which letter appeared in the most words
        highCount = max(letterCount)
        highCountIndexes = []
        
        for j in range(len(letterCount)):
            if letterCount[j] == highCount:
                highCountIndexes += [j]

        # Break any ties randomly
        choice = random.choice(highCountIndexes)

        return allLetters[choice]

def solve(numLetters):
    h = Hangman(numLetters)
    print(h)
    L = []
    count = 0
    strikes = 0
    while count != numLetters and strikes != 6:
        letter = h.nextLetter(L)   
        print(letter)        
        L += [letter]
        letterPositions = [int(x) for x in input().split()]

        if letterPositions == []:
            strikes += 1
            
        h.inputLetter(letter, letterPositions)
        print(h)
        print('\n')
        print(L)
        print('Strikes:', strikes)

        count = 0
        for x in h.data:
            if x != '_':
                count += 1
    if strikes == 6:
        print('FAILURE...')
    if count == numLetters:
        print('SOLVED!')

        

        

# don't always pick most common choice, pick a percentage of the time equivalent to commonness
# dictionary only goes up to 8 letter words
        




    