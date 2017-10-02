def mostCommon(L):
    LoL = []
    most = 0
    for i in range(len(L)):
        LoL.append([count(L[i], L), L[i]])
    return max(LoL)[1]

def count(e, L):
    count = 0
    for i in range(len(L)):
        if e == L[i]:
            count += 1
    return count

def sum10(L):
    pairs = []
    for i in range(len(L)):
        for j in range(len(L)):
            if i != j:
                if L[i] + L[j] == 10:
                    pairs.append([L[i], L[j]])

    return pairs