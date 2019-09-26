import random
import sys

def listToString(L):
        """converts list to String"""
        S = ''
        for x in L:
            S += str(x)
        return S  
    
class Hangman:
    """datatype representing an AI Hangman solver"""

    def __init__(self, numLetters):
        """constructor"""        
        self.numLetters = numLetters
        self.currentBoard = ['_'] * numLetters
        self.wrongLetters = set()
        self.unusedLetters = list('ABCDEFGHIJKLMNOPQRSTUVWXYZ')

        # Grab list of all words
        f = open('Dictionary.txt')
        text = f.read()
        f.close()
        self.wordList = list(filter(lambda word: len(word) == self.numLetters, text.split()))

    def __repr__(self):
        """string representation for Hangman object"""
        numLetters = self.numLetters
        S = ''
        S += 3*'\n'
        S += '           '
        for i in range(numLetters):
            S += self.currentBoard[i] + ' '

        return S
    
    def inputLetter(self, letter, L):
        """input a correctly guessed letter into the correct spaces of list L"""
        if L != []:           
        
            for x in L:
                self.currentBoard[x] = letter  

    
    def filterPossibleWords(self):
        """return a list of all words that fit parameters of the string currentBoard"""       
        filledInSpaces = []
        for i in range(len(self.currentBoard)):
            if self.currentBoard[i] != '_':
                filledInSpaces.append( (i, self.currentBoard[i]) )
        
        self.wordList = list(filter(lambda word: self.viableWord(word, filledInSpaces), self.wordList))

    def viableWord(self, word, filledInSpaces):
        """returns True if word is a possible solution to filledInSpaces.
        param filledInSpaces : List of (index, letter) representing filled in spaces"""
        
        # Check if it fits blanks
        for (index, letter) in filledInSpaces:
            if letter != word[index]:
                return False

        # Check if it fits unused
        for letter in word:
            if letter in self.wrongLetters:
                return False

        return True

    def nextLetter(self):
        """returns most likely next letter choice based on currentBoard
        and list L of used letters"""
        self.filterPossibleWords()
        print(self.wordList)
        
        letterCount = len(self.unusedLetters) * [0]

        # Count the number of words each letters appears in
        for word in self.wordList:
            for i in range(len(self.unusedLetters)):
                # Don't count letters already discovered
                if self.unusedLetters[i] not in self.currentBoard and self.unusedLetters[i] in word:
                    letterCount[i] += 1
        
        # Determine which letter appeared in the most words
        highCount = max(letterCount)
        highCountIndexes = []
        
        # Gather all the letters that tied for the highest count
        for j in range(len(letterCount)):
            if letterCount[j] == highCount:
                highCountIndexes += [j]

        # Break any ties randomly
        choice = random.choice(highCountIndexes)

        chosenLetter = self.unusedLetters[choice]
        self.unusedLetters.remove(chosenLetter)
        return chosenLetter

def playAgainstComputer(numLetters):
    h = Hangman(numLetters)
    print(h)
    count = 0
    strikes = 0
    while count != numLetters and strikes != 6:
        letter = h.nextLetter()   
        print(letter)        
        letterPositions = [int(x) for x in input().split()]

        if letterPositions == []:
            h.wrongLetters.add(letter)
            strikes += 1
            
        h.inputLetter(letter, letterPositions)
        print(h)
        print('\n')
        print(h.wrongLetters)
        print('Strikes:', strikes)

        count = 0
        for x in h.currentBoard:
            if x != '_':
                count += 1
    if strikes == 6:
        print('FAILURE...')
    if count == numLetters:
        print('SOLVED!')

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Input the length of the word the computer will guess.")
        sys.exit()
        
    playAgainstComputer(int(sys.argv[1]))

# TODO
# don't always pick most common choice, pick a percentage of the time equivalent to commonness
# dictionary only goes up to 8 letter words
        




    