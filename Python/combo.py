def combo(L, k, data):
    if k == 1:
        
        for item in L[0]:
            data.append([item])

        return data
   
    else:
        
        for i in range(0, len(L[0])-k+1):

            for item in combo([L[0][i+1:]], k-1, []):

                #if len(L[0][i:i+1] + item) == 3:
                     # if len of this == k, do function
                print("data before:",data)
                data += [ L[0][i:i+1] + item]
                print("data after:",data)

    return data

# L = []
# for x in [2,3,4,5,6,7,8,9]:
#     L += 4*str(x)
# for x in ['T','J','Q','K','A']:
#     L += 4*str(x)

# L = [L]

# print(len(combo(L,2)))


# def combo(L, k, data):
    
#     if k == 1:
#         temp = []
#         for item in L[0]:
#             temp.append([item])

#         return temp
   
#     else:
#         print("L:", L)
#         for i in range(0, len(L[0])-k+1):

#             for item in combo([L[0][i+1:]], k-1, []):
#                 # print('\n')
#                 # print("data before:", data)
#                 # print("add:", [L[0][i:i+1] + item])
#                 data += [ L[0][i:i+1] + item]
#                 # print("data after:",data)

#     return data