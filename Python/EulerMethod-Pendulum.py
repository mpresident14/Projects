import math
import matplotlib.pyplot as plt

def angAccelIntegrate(numSteps, start, finish, initial):
    """Numerically integrate angular velocity function using numSteps from start to finish with initial condition initial"""
    time = 0
    theta = initial
    velocity = 0    
    acceleration = -(9.8/0.4)*math.sin(theta)
    timeList = [0]
    thetaList = [theta]
    velocityList = [0]

    stepSize = (finish-start)/numSteps

    for i in range(numSteps):

        time += stepSize 
        timeList += [time]

        velocity = velocity + stepSize*acceleration
        velocityList += [velocity]

        theta = theta + stepSize*velocity
        thetaList += [theta]

        acceleration = -(9.8/0.4)*math.sin(theta)
    
    plt.plot(timeList, thetaList)
    plt.axis([start,finish,-initial,initial])
    plt.show()

    plt.plot(timeList, velocityList)
    plt.axis([start,finish,min(velocityList), max(velocityList)])
    plt.show()

    return "velocity:",velocity, "theta", theta

    