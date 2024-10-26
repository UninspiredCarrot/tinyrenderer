# Theory

## Digital Differential Analyser (DDA) Line Drawing Algorithm

### Parameters
(x<sub>1</sub>, y<sub>1</sub>, x<sub>2</sub>, y<sub>2</sub>)

### Algorithm
1. Find differences between two points
```python
dx = x2 -x1
dy = y2 -y1
```
2. Find maximum difference
```python
if (abs(dx) > abs(dt)):
    step = abs(dx)
else
    step = abs(dy)
```
3. Calculate increments
```python
x_inc = dx/step
y_inc = dy/step
```
4. Loop over each point in line
```python
for i in range step:
    draw_pixel(x1 + x_inc, y1 + y_inc)
```


### Drawbacks
1. Line will not be smooth because of rounding of floats.
2. Problem is the algorithm uses division which generate floats which can't be drawn and also takes extra time to compute.

## Bresenham's Line Drawing Algorithm

### Advantage over DDA
- There is no division which is computationally expensive
- We don't deal with floats only to round them later