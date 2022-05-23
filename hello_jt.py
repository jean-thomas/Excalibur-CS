import zarr
import dask.array as da
import numpy as np
from numcodecs import Blosc


np.random.seed(42)

# Generate a random array of 200k elements and split it in chunk of 10000 elt
data = np.random.randint(0, 427, size=8192, dtype='i4')
data = zarr.array(data, chunks=1024, compressor=None)
zarr.save_array('TEST/jt_uncompressed.zarr', data, compressor=None)
print(data.info)

double_check = zarr.open('./TEST/jt_uncompressed.zarr', mode='r', chunks=1024,compressor=None)
print(double_check.info)
result  = np.mean(double_check)
print(result)


# Create a dask array with 10x10 chunks
a = np.arange(20000).reshape(100, 200)
x = da.from_array(a, chunks=(10, 10))

# Laxy max
x_max = da.max(x)

# Compute the maximum, and check that it's correct
if x_max.compute() == np.max(a):
    print("dask works")
