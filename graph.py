import numpy as np

import cv2
from matplotlib import pyplot as plt
from matplotlib.collections import PatchCollection
from matplotlib.patches import Rectangle

x,y = np.loadtxt('mbr.txt' , delimiter=',' , unpack=True)
x1,y1 = np.loadtxt('point.txt' , delimiter=',' , unpack=True)

"""for i in range(0,len(x)-4):
    centerx=x[i+3]-x[i]
    centery=y[i+3]-y[i]
    plt.plot.Rectangle((x[i]+centerx/2,y[i]+centery/2),centerx,centery)
    i=i+4"""

def make_error_boxes(ax, xdata, ydata, facecolor='r',
                     edgecolor='None', alpha=0.5):

    # Create list for all the error patches
    errorboxes = []

    # Loop over data points; create box from errors at each point
    for i in range(0,len(x),+4):
        centerx=x[i+3]-x[i]
        centery=y[i+3]-y[i]
        rect = Rectangle((x[i],y[i]),centerx,centery)
        errorboxes.append(rect)

    # Create patch collection with specified colour/alpha
    pc = PatchCollection(errorboxes, facecolor=facecolor, alpha=alpha,
                         edgecolor=edgecolor)

    # Add collection to axes
    ax.add_collection(pc)

    # Plot errorbars
    artists = ax.errorbar(xdata, ydata, fmt='None', ecolor='k')

    return artists
# Create figure and axes
fig, ax = plt.subplots(1)

# Call function to create error boxes
_ = make_error_boxes(ax, x, y)
#plt.plot(x2,y2,color="yellow",label="Selection Sort Java")
for i in range(len(x1)):
    plt.plot(x1[i],y1[i],'o',linewidth=3,color=(0.2,0.1,0.4))

#plt.legend()
# plt.plot(s,d,color="red")
#plt.title('Select Sort')
#plt.xlabel('Size')
#plt.ylabel('Time')
plt.show()
