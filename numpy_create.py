import numpy as np
def numpy_create(a,b):
	#input tuple
	#if tuple is already 19*19 use folowing sentence to merge them
	c=np.array(a)
	d=np.array(b)
	e=np.array([c,d])
	print e
	print e.shape
	#if tuple is 1*361
	#then using following sentence
	#c=np.array(a)
	#d=np.array(b)
	#c=np.reshape(c,(-1,19))
	#d=np.reshape(d,(-1,19))
	#e=np.array([c,d])
	return e
