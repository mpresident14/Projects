# Neural network with variable amount of hidden layers


import numpy as np
import pandas as pd
import random
from datetime import datetime

class Model:

    def __init__(self, weights, num_hidden_layers):
        self.weights = weights
        self.num_hidden_layers = num_hidden_layers

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
 

def convertOutputValuesToBinaryNodes(y, output_arr_values):
    num_output_neurons = len(output_arr_values)
    length = len(y)
    newY = np.zeros([length, num_output_neurons], dtype=np.float32)
    
    for i in range(length):
        value = y[i]
        # Find index of output_arr_values that matches y[i]
        for j in range(num_output_neurons):
            if value == output_arr_values[j]:
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

# TODO: we don't actually need an array because we don't need to backpropagate
def calculate_loss(model, X, y, num_examples):
    # FORWARD PROPAGATION
    z_arr = [] # array of matrices of node values for each hidden layer (length = num_hidden_layers + 1)
    a_arr = [] # array of matrices of node values for each hidden layer after applying activation fcn (length = num_hidden_layers + 1)
    z_arr.append(np.matmul(X, model.weights[0]))
    a_arr.append(np.tanh(z_arr[0]))

    for  i in range(model.num_hidden_layers - 1):
        z_arr.append(np.matmul(a_arr[i], model.weights[i+1]))
        a_arr.append(np.tanh(z_arr[i+1]))
    
    z_arr.append(np.matmul(a_arr[-1], model.weights[-1]) )
    a_arr.append(softmax(z_arr[-1]) )

    probs = a_arr[-1]

    # Calculate loss
    loss = np.square(probs - y) # using mean squared error sum( (pred_y - actual_y)^2 )
    total_loss = np.sum(loss)
    return 1. / num_examples * total_loss
             

# *********************************************************************
# ENTER DATA AND ADJUST PARAMS HERE
# *********************************************************************

df = readFile('iris.csv')
df = df.iloc[:,:].values        # Convert to numpy array

output_arr_values = ["setosa", "versicolor", "virginica"]

X = np.array(df[9:, 0:4], dtype=np.float32)
y = convertOutputValuesToBinaryNodes(df[9:, 4:5], output_arr_values)
x_test_data = np.array(df[0:9, 0:4], dtype=np.float32)
# y_validation_data = convertOutputValuesToBinaryNodes(df[0:9, 0:4], output_arr_values)

hidden_layer_dims = [5,5]
learning_rate = 0.001
num_iterations = 20000

# *********************************************************************
# *********************************************************************

num_examples = len(X)
nn_input_dim = len(X[0])
nn_output_dim = len(output_arr_values)

def train_model():
    # Initialize the parameters to random values. We need to learn these.
    random.seed(datetime.now())
    num_hidden_layers = len(hidden_layer_dims)

    weights = [] # (length = num_hidden_layers + 1)
    weights.append(np.random.randn(nn_input_dim, hidden_layer_dims[0]) / np.sqrt(nn_input_dim) ) # Connecting to input layer
    for i in range(num_hidden_layers - 1):
        weights.append(np.random.randn(hidden_layer_dims[i], hidden_layer_dims[i+1]) / np.sqrt(hidden_layer_dims[i]) ) # Connecting hidden layers
    weights.append(np.random.randn(hidden_layer_dims[num_hidden_layers-1], nn_output_dim) / np.sqrt(hidden_layer_dims[num_hidden_layers - 1]) ) # Connecting output layers

    model = Model(weights, num_hidden_layers)

    for it in range(num_iterations):

        # FORWARD PROPAGATION
        z_arr = [] # array of matrices of node values for each hidden layer (length = num_hidden_layers + 1)
        a_arr = [] # array of matrices of node values for each hidden layer after applying activation fcn (length = num_hidden_layers + 1)
        z_arr.append(np.matmul(X, weights[0]))
        a_arr.append(np.tanh(z_arr[0]))

        for  i in range(num_hidden_layers - 1):
            z_arr.append(np.matmul(a_arr[i], weights[i+1]))
            a_arr.append(np.tanh(z_arr[i+1]))
        
        z_arr.append(np.matmul(a_arr[-1], weights[-1]) )
        a_arr.append(softmax(z_arr[-1]) )

        loss_derivs = (num_hidden_layers + 1) * [0]
        # BACKPROPAGATION
        # Output layer
        # using mean squared error, derivative is sum( (pred_y - actual_y)^2 )
        zOut = z_arr[-1]
        probs = a_arr[-1]

        dLossDAOut = 2 * (probs - y)                        # n x 3    
        dAOutDZOut = softmax_deriv(zOut)                    # n x 3
        dLossDZOut = np.multiply(dLossDAOut, dAOutDZOut)    # n x 3                                 

        dLossDWOut = np.matmul(a_arr[-2].T, dLossDZOut)            # 5 x 3
        loss_derivs[-1] = dLossDWOut

        # Hidden layers
        prevDLossDZ = dLossDZOut
        
        for i in range(num_hidden_layers - 1, 0, -1):
            dLossDA = np.matmul(prevDLossDZ, weights[i+1].T)
            dADZ = tanh_deriv(z_arr[i])
            dLossDZ = np.multiply(dLossDA, dADZ)

            dLossDW = np.matmul(a_arr[i-1].T, dLossDZ)
            loss_derivs[i] = dLossDW

            prevDLossDZ = dLossDZ

        # First layer
        dLossDA1 = np.matmul(prevDLossDZ, weights[1].T) 
        dA1DZ1 = tanh_deriv(z_arr[0])   
        dLossDZ1 = np.multiply(dLossDA1, dA1DZ1)

        dLossDW1 = np.matmul(X.T, dLossDZ1)  
        loss_derivs[0] = dLossDW1

        # UPDATE WEIGHTS
        for i in range(num_hidden_layers + 1):
            weights[i] += -learning_rate * loss_derivs[i]
            
        # ASSIGN NEW PARAMETERS TO THE MODEL    
        model.weights = weights  
         
        # Optionally print the loss.
        # This is expensive because it uses the whole dataset, so we don't want to do it too often.
        if it % (num_iterations/10) == 0:
            loss = calculate_loss(model, X, y, num_examples)
            print ('Loss after iteration %d: %s' % (it, '{0:1.3f}'.format(loss)))
    
    return model

def predict(model):
    # TODO: we don't actually need an array because we don't need to backpropagate
    
    # FORWARD PROPAGATION
    z_arr = [] # array of matrices of node values for each hidden layer (length = num_hidden_layers + 1)
    a_arr = [] # array of matrices of node values for each hidden layer after applying activation fcn (length = num_hidden_layers + 1)
    z_arr.append(np.matmul(x_test_data, model.weights[0]))
    a_arr.append(np.tanh(z_arr[0]))

    for  i in range(model.num_hidden_layers - 1):
        z_arr.append(np.matmul(a_arr[i], model.weights[i+1]))
        a_arr.append(np.tanh(z_arr[i+1]))
    
    z_arr.append(np.matmul(a_arr[-1], model.weights[-1]) )
    a_arr.append(softmax(z_arr[-1]) )

    probs = a_arr[-1]

    print('\n')
    np.set_printoptions(suppress=True)

    predictions = convertNodesToOutputValues(probs, output_arr_values)
    print(probs)
    print(predictions)

if __name__ == "__main__":
     model = train_model()
     predict(model)
     
