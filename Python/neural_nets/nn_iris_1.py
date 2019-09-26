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

def tanh_deriv(matrix):
    return 1 - np.square(np.tanh(matrix))    

def convertYToOutputValues(y, nn_output_dim):
    length = len(y)
    newY = np.zeros([length, nn_output_dim], dtype=np.float32)
    for i in range(length):
        if y[i] == "setosa":
            newY[i] = np.array([1, 0, 0], dtype=np.float32)
        elif y[i] == "versicolor":
            newY[i] = np.array([0, 1, 0], dtype=np.float32)
        elif y[i] == "virginica":
            newY[i] = np.array([0, 0, 1], dtype=np.float32)

    return newY   

def calculate_loss(model, X, y, num_examples):
    # Forward Propagation
    z1 = X.dot(model['W1'])   # hidden layer
    a1 = np.tanh(z1) # tanh is our activation fcn
    z2 = a1.dot(model['W2'])  # output layer
    probs = softmax(z2) # convert to probs  

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


df = readFile('iris.csv')
df = df.iloc[:,:].values        # Convert to numpy array

# Divide data up
num_output_neurons = 3

X = np.array(df[9:, 0:4], dtype=np.float32)
y = convertYToOutputValues(df[9:, 4:5], num_output_neurons)
x_test_data = np.array(df[0:9, 0:4], dtype=np.float32)
y_test_data = df[0:9, 4]

num_examples = len(X)
nn_input_dim = len(X[0])
nn_hidden_dim = 5
nn_output_dim = num_output_neurons
learning_rate = 0.001

num_iterations = 20000

def train_model():
    # Initialize the parameters to random values. We need to learn these.
    random.seed(datetime.now())
    W1 = np.random.randn(nn_input_dim, nn_hidden_dim) / np.sqrt(nn_input_dim)      # 4 x 5
    W2 = np.random.randn(nn_hidden_dim, nn_output_dim) / np.sqrt(nn_hidden_dim)   # 5 x 3

    model = {}

    for i in range(num_iterations):

        # FORWARD PROPAGATION
        z1 = X.dot(W1)   # hidden layer
        a1 = np.tanh(z1) # tanh is our activation fcn
        z2 = a1.dot(W2)  # output layer
        probs = softmax(z2) # convert to probs

        # BACKPROPAGATION
        # Output layer
        # using mean squared error, derivative is sum( (pred_y - actual_y)^2 )
        dLossDA2 = 2 * (probs - y) # n x 3    
        dA2DZ2 = softmax_deriv(z2) # n x 3
        dLossDZ2 = np.multiply(dLossDA2, dA2DZ2) # n x 3
        dZ2DW2 = a1                 # n x 5

        dLossDW2 = np.matmul(dZ2DW2.T, dLossDZ2) # 5 x 3

        # First layer
        dLossDA1 = np.matmul(dLossDZ2, W2.T)
        dA1DZ1 = tanh_deriv(z1)
        # (dZ1DW1 = X)

        dLossDW1 = np.matmul(X.T, np.multiply(dLossDA1, dA1DZ1)) # 4 x 5

        # UPDATE WEIGHTS
        W1 += -learning_rate * dLossDW1
        W2 += -learning_rate * dLossDW2

        # ASSIGN NEW PARAMETERS TO THE MODEL
        model = {'W1': W1, 'W2': W2}
         
        # Optionally print the loss.
        # This is expensive because it uses the whole dataset, so we don't want to do it too often.
        if i % (num_iterations/10) == 0:
            loss = calculate_loss(model, X, y, num_examples)
            print ('Loss after iteration %d: %s' % (i, '{0:1.3f}'.format(loss)))
    
    return model

def predict(model):
    W1 = model['W1']
    W2 = model['W2']

    # Forward Propagation
    z1 = x_test_data.dot(W1)   # hidden layer
    a1 = np.tanh(z1) # tanh is our activation fcn
    z2 = a1.dot(W2)  # output layer
    probs = softmax(z2) # convert to probs
 
    print('\n')
    np.set_printoptions(suppress=True)
    print(probs)

if __name__ == "__main__":
     model = train_model()
     predict(model)
