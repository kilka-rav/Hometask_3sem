import numpy as np

def initialize_parameters(n):
    
   parameters = dict()

   parameters['W'] = np.random.randn(n,1) * 0.01
   parameters['b'] = np.zeros((n, 1))
    
   return parameters 

def forward_prop(x, parameters):

    target = np.dot(x, parameters['W']) + parameters['b']

    return target

def compute_loss(y, target, lambd, parameters):

    m = y.shape[0]

    loss = 1/(2*m)*np.sum((y - target)**2) + lambd/(2*m)*np.sum(parameters['W']**2)

    return loss

def backward_prop(x, y, target, parameters,lambd):

    grads = dict()
    m = y.shape[0]

    grads['dW'] = 1/m*np.sum((y-target)*x) + lambd/m * parameters['W']
    grads['db'] = 1/m*np.sum(y-target)

    return grads

def optimize_parameters(parameters, grads, learning_rate):

    parameters['W'] = parameters['W'] - learning_rate * grads['dW']
    parameters['b'] = parameters['b'] - learning_rate * grads['dW']

    return parameters


def model(x, y, learning_rate, lambd, epochs):

    n = x.shape[1]
    m = x.shape[0]
    total_loss = list()

    parameters = initialize_parameters(n)

    for i in range(epochs):
        target = forward_prop(x, parameters)
        loss = compute_loss(y, target, lambd, parameters)
        total_loss.append(loss)
        print('Epoch {}: loss = {}'.format(i, loss))
        grads = backward_prop(x, y, target, parameters,lambd)
        parameters = optimize_parameters(parameters,grads,learning_rate)


    return total_loss, target, y, x

x = np.array([[1,2,3,4,5,6, 7, 8, 9, 10]])
y = x*15.768

loss,target,y,x = model(x,y,0.001,0.001,50)

