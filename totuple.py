import numpy as np
def totuple(a):
    try:
        return tuple(totuple(i) for i in a)
    except TypeError:
        return a
    
a=(1,1,1,1,1,1,1,1)
b=np.array(a)
b=np.reshape(b,(-1,4))
c=(2,2,2,2,2,2,2,2)
d=np.array(c)
f=totuple(b)
print f
