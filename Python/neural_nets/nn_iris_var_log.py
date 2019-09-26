# Neural network with variable amount of hidden layers


import numpy as np
import pandas as pd
import random
import time

np.set_printoptions(suppress=True)

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

# tanh is activation fcn for hidden layers
def tanh_deriv(matrix):
    return 1 - np.square(np.tanh(matrix))   

# If y = 1, then dLossDZOut = -1/a * a(1-a) = a-1
# If y = 0, then dLossZOut = 1/(-a) * a(1-a) = a
# This function converts y into the correct constant
def calculateDLossDZOut(prob):
    if (prob == 1):
        return -1
    else:
        return 0

def log_loss(probs, y):
    """computes avg log loss"""
    num_examples = len(y)
    num_output_neurons = len(y[0])
    total_loss = 0

    for row in range(num_examples):
        storeYRow = y[row]
        storeProbsRow = probs[row]
        for col in range(num_output_neurons):
            if (storeYRow[col] == 1):
                total_loss += -np.log(storeProbsRow[col])
            else:
                total_loss += -np.log(1 - storeProbsRow[col])   

    return total_loss / num_examples

def convertOutputValuesToBinaryNodes(y, output_arr_values):
    num_output_neurons = len(output_arr_values)
    length = len(y)
    newY = np.zeros([length, num_output_neurons], dtype=np.float32)
    
    for i in range(length):
        value = y[i][0]
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
def calculate_loss(model, num_examples):
    # FORWARD PROPAGATION
    prevZ = np.matmul(X, model.weights[0])
    prevA = np.tanh(prevZ)

    for  i in range(model.num_hidden_layers - 1):
        prevZ = np.matmul(prevA, model.weights[i+1])
        prevA = np.tanh(prevZ)
    
    prevZ = np.matmul(prevA, model.weights[-1])
    prevA = softmax(prevZ)

    probs = prevA

    # Calculate loss
    return log_loss(probs, y)

def predict_validation_and_calculate_loss(model, num_examples):
    # FORWARD PROPAGATION
    prevZ = np.matmul(x_validation, model.weights[0])
    prevA = np.tanh(prevZ)

    for  i in range(model.num_hidden_layers - 1):
        prevZ = np.matmul(prevA, model.weights[i+1])
        prevA = np.tanh(prevZ)
    
    prevZ = np.matmul(prevA, model.weights[-1])
    prevA = softmax(prevZ)

    probs = prevA

    # Calculate loss
    return probs, log_loss(probs, y_validation)

def showValidationResults(probs):
    pred_y = convertNodesToOutputValues(probs, output_arr_values)  
    actual_y = convertNodesToOutputValues(y_validation, output_arr_values)   
    
    total = 0
    correct = 0
    for i in range(len(pred_y)):
        total += 1
        if (pred_y[i] == actual_y[i]):
            correct += 1
        else:
            print("Predicted: " + str(pred_y[i]) + " | Actual: " + str(actual_y[i]))
            print(x_validation[i])
            print('\n')

    print(correct, "out of", total, "correct.")


# *********************************************************************
# ENTER DATA AND ADJUST PARAMS HERE
# *********************************************************************

df = readFile('banknote.csv')
df = df.iloc[:,:].values        # Convert to numpy array

output_arr_values = [0,1]
num_training = 1200
num_validation = 172
num_attributes = 4

hidden_layer_dims = [5,5,5]
learning_rate = 0.0003
num_iterations = 20000

should_predict = False

# *********************************************************************
# *********************************************************************
np.random.seed()
known_examples = df[:num_training + num_validation, :]

np.random.shuffle(known_examples)

X = known_examples[0:num_training, 0:num_attributes] \
        .astype(np.float32, copy=False)
x_validation = known_examples[num_training:num_training + num_validation, 0:num_attributes] \
        .astype(np.float32, copy=False)
y = convertOutputValuesToBinaryNodes( \
        known_examples[0:num_training, num_attributes:], output_arr_values) 
y_validation = convertOutputValuesToBinaryNodes( \
        known_examples[num_training:num_training + num_validation, num_attributes:], output_arr_values)
x_test = df[num_training+num_validation: , 0:num_attributes] \
        .astype(np.float32, copy=False)

calculateDLossDZOutVF = np.vectorize(calculateDLossDZOut)
log_loss_consts = calculateDLossDZOutVF(y)

num_examples = len(X)
nn_input_dim = len(X[0])
nn_output_dim = len(output_arr_values)

def train_model():
    # Initialize the parameters to random values. We need to learn these.
    # np.random.seed()
    num_hidden_layers = len(hidden_layer_dims)

    # XAVIER INITIALIZATION
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
        # Using log error
        probs = a_arr[-1]
                         
        dLossDZOut = log_loss_consts + probs # for each y: if y=1 -> probs-1, if y=0 -> probs
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
        if it % (num_iterations / 10) == 0:
            loss = calculate_loss(model, num_examples)
            print ('Loss after iteration %d: %s' % (it, '{0:1.9f}'.format(loss)))
    
    probs, validation_loss = \
            predict_validation_and_calculate_loss(model, num_examples)
    
    print('\n')
    print ('Validation Loss: %s' % ('{0:1.3f}'.format(validation_loss)))
    print('\n')
    showValidationResults(probs)
    return model

def predict(model):
    # TODO: we don't actually need an array because we don't need to backpropagate
    
    # FORWARD PROPAGATION
    prevZ = np.matmul(x_test, model.weights[0])
    prevA = np.tanh(prevZ)

    for  i in range(model.num_hidden_layers - 1):
        prevZ = np.matmul(prevA, model.weights[i+1])
        prevA = np.tanh(prevZ)
    
    prevZ = np.matmul(prevA, model.weights[-1])
    prevA = softmax(prevZ)

    probs = prevA

    print('\n')

    predictions = convertNodesToOutputValues(probs, output_arr_values)
    print(probs)
    print(predictions)
    return probs

if __name__ == "__main__":
    model = train_model()
    if (should_predict):
        predict(model)
     
