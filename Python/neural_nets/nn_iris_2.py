# Neural network with two hidden layers


import numpy as np
import pandas as pd
import random
from datetime import datetime

def readFile(filename='iris.csv'):
    """read csv file and convert it to array"""
    df = pd.read_csv(filename, header=0)    # read the file
    return df

# This fcn converts raw scores into probabilities between 0 and 1
# Used for multi-class regression
# softmax(x) = e^xi / sum(e^xi) over all i 
# shifted to avoid NaNs
def softmax(matrix):
    shiftX = matrix - np.max(matrix)
    exponentiated = np.exp(shiftX)
    probs = exponentiated / np.sum(exponentiated, axis=1, keepdims=True)
    return probs

def softmax_deriv(matrix):
    soft = softmax(matrix)
    return np.multiply(soft, (1 - soft))

# # Used for two-class logistic regression
# def sigmoid(matrix):
#     return 1 / (1 + np.exp(-matrix))

# def sigmoid_deriv(matrix):
#     sig = sigmoid(matrix)  
#     return np.multiply(sig, (1 - sig));  

# tanh is activation fcn for hidden layers
def tanh_deriv(matrix):
    return 1 - np.square(np.tanh(matrix))    

# def convertOutputValuesToBinaryNodes(y, nn_output_dim):
#     length = len(y)
#     newY = np.zeros([length, nn_output_dim], dtype=np.float32)
#     for i in range(length):
#         if y[i] == "setosa":
#             newY[i] = np.array([1, 0, 0], dtype=np.float32)
#         elif y[i] == "versicolor":
#             newY[i] = np.array([0, 1, 0], dtype=np.float32)
#         elif y[i] == "virginica":
#             newY[i] = np.array([0, 0, 1], dtype=np.float32)

#     return newY   

def convertOutputValuesToBinaryNodes(y, output_arr_values):
    num_output_neurons = len(output_arr_values)
    length = len(y)
    newY = np.zeros([length, num_output_neurons], dtype=np.float32)
    
    for i in range(length):
        value = y[i]
        index = 0
        # Find index of output_arr_values that matches y[i]
        for j in range(num_output_neurons):
            if value == output_arr_values[j]:
                index = j
                break

        newY[i] = np.zeros([1, num_output_neurons], dtype=np.float32)
        newY[i][j] = 1       

    return newY     

def convertNodesToOutputValues(probs, output_arr_values):
    maxIndices = np.argmax(probs, axis=1)
    
    predictions = len(probs) * [0]

    # Put output values corresponding to the max index into the predictions array
    for i in range(len(maxIndices)):
        predictions[i] = output_arr_values[maxIndices[i]]

    return predictions


def calculate_loss(model, X, y, num_examples):
    # Forward Propagation
    z1 = np.matmul(X, model['W1'])   # hidden layer 1
    a1 = np.tanh(z1) # tanh is our activation fcn
    z2 = np.matmul(a1, model['W2']) # hidden layer 2
    a2 = np.tanh(z2)
    zOut = np.matmul(a2, model['WOut'])  # output layer
    probs = softmax(zOut) # convert to probs

    # Calculate loss
    loss = np.square(probs - y) # using mean squared error sum( (pred_y - actual_y)^2 )
    total_loss = np.sum(loss)
    return 1. / num_examples * total_loss

# def find_dLossDSoft(probs, y):
#     dLossDSoft = probs
#     for row in range(len(y)):
#         for col in range(len(y[0])):
#             if y[row, col] == 1:
#                 dLossDSoft[row][col] = -np.divide(1, probs[row][col])
#             else:
#                 dLossDSoft[row][col] = np.divide(1, 1-probs[row][col]) 

#     return dLossDSoft               

# *********************************************************************
# ENTER DATA AND ADJUST PARAMS HERE
# *********************************************************************

df = readFile('iris.csv')
df = df.iloc[:,:].values        # Convert to numpy array

output_arr_values = ["setosa", "versicolor", "virginica"]

X = np.array(df[9:, 0:4], dtype=np.float32)
y = convertOutputValuesToBinaryNodes(df[9:, 4:5], output_arr_values)
x_test_data = np.array(df[0:9, 0:4], dtype=np.float32)

nn_hidden_1_dim = 7
nn_hidden_2_dim = 8
learning_rate = 0.0008
num_iterations = 20000

# *********************************************************************
# *********************************************************************

num_examples = len(X)
nn_input_dim = len(X[0])
nn_output_dim = len(output_arr_values)

def train_model():
    # Initialize the parameters to random values. We need to learn these.
    random.seed(datetime.now())
    W1 = np.random.randn(nn_input_dim, nn_hidden_1_dim) / np.sqrt(nn_input_dim)         # 4 x 5
    W2 = np.random.randn(nn_hidden_1_dim, nn_hidden_2_dim) / np.sqrt(nn_hidden_1_dim)   # 5 x 5
    WOut = np.random.randn(nn_hidden_2_dim, nn_output_dim) / np.sqrt(nn_hidden_2_dim)   # 5 x 3

    model = {}

    for i in range(num_iterations):

        # FORWARD PROPAGATION
        z1 = np.matmul(X, W1)   # hidden layer 1
        a1 = np.tanh(z1) # tanh is our activation fcn
        z2 = np.matmul(a1, W2) # hidden layer 2
        a2 = np.tanh(z2)
        zOut = np.matmul(a2, WOut)  # output layer
        probs = softmax(zOut) # convert to probs

        # BACKPROPAGATION
        # Output layer
        # using mean squared error, derivative is sum( (pred_y - actual_y)^2 )
        dLossDAOut = 2 * (probs - y)                        # n x 3    
        dAOutDZOut = softmax_deriv(zOut)                    # n x 3
        dLossDZOut = np.multiply(dLossDAOut, dAOutDZOut)    # n x 3
        # dZOutDWOut = a2                                  # n x 5

        dLossDWOut = np.matmul(a2.T, dLossDZOut)            # 5 x 3

        # Second layer
        dLossDA2 = np.matmul(dLossDZOut, WOut.T)            # n x 5
        dA2DZ2 = tanh_deriv(z2)                             # n x 5
        dLossDZ2 = np.multiply(dLossDA2, dA2DZ2)            # n x 5
        # dZ2DW2 = a1                                       # n x 5

        dLossDW2 = np.matmul(a1.T, dLossDZ2)                # 5 x 5

        # First layer 
        dLossDA1 = np.matmul(dLossDZ2, W2.T)                # n x 5
        dA1DZ1 = tanh_deriv(z1)                             # n x 5
        dLossDZ1 = np.multiply(dLossDA1, dA1DZ1)            # n x 5
        # dZ1DW1 = X                                        # n x 4

        dLossDW1 = np.matmul(X.T, dLossDZ1)                 # 4 x 5

        # UPDATE WEIGHTS
        W1 += -learning_rate * dLossDW1
        #print(W2.shape)
        #print(dLossDW2.shape)
        W2 += -learning_rate * dLossDW2
        WOut += -learning_rate * dLossDWOut

        # ASSIGN NEW PARAMETERS TO THE MODEL
        model = {'W1': W1, 'W2': W2, 'WOut': WOut}
         
        # Optionally print the loss.
        # This is expensive because it uses the whole dataset, so we don't want to do it too often.
        if i % (num_iterations/10) == 0:
            loss = calculate_loss(model, X, y, num_examples)
            print ('Loss after iteration %d: %s' % (i, '{0:1.3f}'.format(loss)))
    
    return model

def predict(model):
    # Forward Propagation
    z1 = np.matmul(x_test_data, model['W1'])   # hidden layer 1
    a1 = np.tanh(z1) # tanh is our activation fcn
    z2 = np.matmul(a1, model['W2']) # hidden layer 2
    a2 = np.tanh(z2)
    zOut = np.matmul(a2, model['WOut'])  # output layer
    probs = softmax(zOut) # convert to probs
 
    print('\n')
    np.set_printoptions(suppress=True)
    predictions = convertNodesToOutputValues(probs, output_arr_values)
    print(probs)
    print(predictions)

if __name__ == "__main__":
     model = train_model()
     predict(model)
     
