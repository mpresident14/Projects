import pandas as pd
import numpy as np
import math

def readFile(filename):
    """read csv file and convert it to array"""
    df = pd.read_csv(filename, header=0)    # read the file
    return df.iloc[:,:].values

def getDistance(X1, X2):
    """calculates distance between two rows of data (based on Euclidean distance)"""
    distance = 0                            
    length = len(X1)
    for i in range(length):
        distance += (X1[i] - X2[i])**2      # differences of the columns squared
    
    distance = math.sqrt(distance)

    return distance

def knnForOne(x_training_data, y_training_data, single_x_test_data, n_neighbors):
    """find the most common neighbor out of k nearest neighbors for 1 row of test data"""
    nearest_neighbors = {}
    length = len(x_training_data)

    for i in range(length):
        X2 = x_training_data[i,:]   # get current row of known data
        Y2 = y_training_data[i]     # get current label of known data
        distance = getDistance(single_x_test_data, X2)   # compare test to known data

        if len(nearest_neighbors) < n_neighbors:        # reach capacity of nearest neighbors
            nearest_neighbors[distance] = Y2
        else:                                           # kick out largest distance

            # Assumes no two distances are exactly the same, or will data point will be overwritten in dictionary
            largest_distance = max(nearest_neighbors)
            if distance < largest_distance:
                del nearest_neighbors[largest_distance]
                nearest_neighbors[distance] = Y2

    # nearest_neighbors is a dictionary with the n nearest neighbors 
    # as values and their distances from single_x_test_data as keys
 
    counts = {}
    for key in nearest_neighbors:                   # initialize counts dictionary
        counts[nearest_neighbors[key]] = 0

    for key in nearest_neighbors:                   # count labels within the nearest neighbors
        counts[nearest_neighbors[key]] += 1

    max_value = max(counts.values())                # find most frequent label within the nearest neighbors
    for key in counts:
        if counts[key] == max_value:
            return key 


def knnForAll(x_training_data, y_training_data, x_test_data, n_neighbors):
    """find the most common neighbor out of k nearest neighbors for multiple rows of test data"""
    y_test_data = []
    for row in x_test_data:             # for multiple rows of test data
        y_test_data += [knnForOne(x_training_data, y_training_data, row, n_neighbors)]

    return y_test_data


def crossValidate(x_training_data, y_training_data, test_size_percentage, k_values):
    """find the value of k that produces the best results for the data"""
    data_length = len(x_training_data)
    foldSize = int(round(data_length * test_size_percentage))   # size of each temporary test data
    groups = int(data_length/foldSize + 1)                      # # of groups

    best_score = 0
    best_k = 0

    for k in k_values:             # Test different values of k
        score = 0
        for i in range(0, data_length, foldSize):      # Switch section of test data
            
            x_temp_test = x_training_data[i:i+foldSize]     # get temporary data to test
            known_y_test = y_training_data[i:i+foldSize]    # we already know their labels
            x_temp_training = np.append(x_training_data[0:i], x_training_data[i+foldSize:], axis=0) # the rest is our temporary training data
            y_temp_training = np.append(y_training_data[0:i], y_training_data[i+foldSize:], axis=0)

            y_temp_test = knnForAll(x_temp_training, y_temp_training, x_temp_test, k)   # labels determined for a current k value

            for i in range(len(known_y_test)):              # Score how well this value of k did based 
                if y_temp_test[i] == known_y_test[i]:       # on how well it matches the known labels
                    score += 1

        print("k:",k,"-- % correct: ","{:0.2f}".format(score/data_length))    # print accuracy for training data    
        if score > best_score:                              # Choose the best k value up to this point
            best_score = score
            best_k = k

    return best_k
          
            
def main():
    df = readFile('iris.csv')

    x_training_data = df[9:, 0:4]
    y_training_data = df[9:, 4]
    x_test_data = df[0:9, 0:4]
    y_test_data = df[0:9, 4]  

    k = crossValidate(x_training_data, y_training_data, 0.2, [1,3,5,7,9])
    print('Best k: ', k)

    return knnForAll(x_training_data, y_training_data, x_test_data, k)