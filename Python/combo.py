import sys

def getAllCombosHelper(L, k, result):
    if k == 1:
        for item in L[0]:
            result.append([item])
        return result
   
    else:
        for i in range(0, len(L[0])-k+1):
            for item in getAllCombosHelper([L[0][i+1:]], k-1, []):
                result += [ L[0][i:i+1] + item]

    return result

def getAllCombos(L, k):
    return getAllCombosHelper([L], k, [])

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Enter a combo size followed by a list of numbers.")
    else:
        L = [int(sys.argv[i]) for i in range(2, len(sys.argv))]        
        print(getAllCombos(L, int(sys.argv[1])))
